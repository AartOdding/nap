/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

// Local Includes
#include "uniformcontainer.h"
#include "texture2d.h"

// External Includes
#include <rtti/rttiutilities.h>

RTTI_BEGIN_CLASS(nap::UniformContainer)
	RTTI_FUNCTION("findUniform", (nap::UniformStructInstance* (nap::UniformContainer::*)(const std::string&)) &nap::UniformContainer::findUniform)
	RTTI_FUNCTION("findSampler", (nap::SamplerInstance* (nap::UniformContainer::*)(const std::string&)) &nap::UniformContainer::findSampler)
RTTI_END_CLASS


namespace nap
{
	UniformStructInstance* UniformContainer::findUniform(const std::string& name)
	{
		for (auto& instance : mRootStructs)
			if (instance->getDeclaration().mName == name)
				return instance.get();
		return nullptr;
	}


	UniformStructInstance& UniformContainer::getUniform(const std::string& name)
	{
		UniformStructInstance* instance = findUniform(name);
		assert(instance != nullptr);
		return *instance;
	}


	UniformStructInstance& UniformContainer::createRootStruct(const UniformStructDeclaration& declaration, const UniformCreatedCallback& uniformCreatedCallback)
	{
		std::unique_ptr<UniformStructInstance> instance = std::make_unique<UniformStructInstance>(declaration, uniformCreatedCallback);
		UniformStructInstance* result = instance.get();
		mRootStructs.emplace_back(std::move(instance));
		return *result;
	}


	void UniformContainer::addSamplerInstance(std::unique_ptr<SamplerInstance> instance)
	{
		mSamplerInstances.emplace_back(std::move(instance));
	}


	SamplerInstance* UniformContainer::findSampler(const std::string& name) const
	{
		for (auto& sampler : mSamplerInstances)
			if (sampler->getDeclaration().mName == name)
				return sampler.get();
		return nullptr;
	}
}