#include "utility/fileutils.h"
#include "projectinfo.h"
#include "simpleserializer.h"


RTTI_BEGIN_CLASS(nap::ProjectInfo)
	RTTI_PROPERTY("title", &nap::ProjectInfo::mTitle, nap::rtti::EPropertyMetaData::Required)
	RTTI_PROPERTY("version", &nap::ProjectInfo::mVersion, nap::rtti::EPropertyMetaData::Required)
	RTTI_PROPERTY("modules", &nap::ProjectInfo::mModuleNames, nap::rtti::EPropertyMetaData::Required)
	RTTI_PROPERTY("modulepaths", &nap::ProjectInfo::mModuleDirs, nap::rtti::EPropertyMetaData::Required)
RTTI_END_CLASS

RTTI_BEGIN_CLASS(nap::ModuleInfo)
		RTTI_PROPERTY("dependencies", &nap::ModuleInfo::mDependencies, nap::rtti::EPropertyMetaData::Required)
RTTI_END_CLASS

namespace nap
{
	std::string ProjectInfo::getDirectory() const
	{
		return nap::utility::getFileDir(mFilename);
	}


	std::vector<std::string> ProjectInfo::getModuleDirectories() const
	{
		std::vector<std::string> dirs;
		auto projectDir = getDirectory();

		// make all paths absolute
		for (const auto& p : mModuleDirs)
		{
			if (utility::isAbsolutePath(p))
				dirs.emplace_back(p);
			else
				dirs.emplace_back(utility::stringFormat("%s/%s", projectDir.c_str(), p.c_str()));
		}

		return dirs;
	}

	bool ProjectInfo::load(const std::string& filename, utility::ErrorState& err)
	{
		mFilename = filename;
		return nap::loadJSONSimple(filename, *this, err);
	}

	bool ModuleInfo::load(const std::string& filename, utility::ErrorState& err)
	{
		mFilename = filename;
		return nap::loadJSONSimple(filename, *this, err);
	}
}
