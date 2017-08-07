#pragma once

#include <rtti/rtti.h>
#include <nap/signalslot.h>
#include <nap/component.h>
#include <utility/dllexport.h>
#include <pybind11/pybind11.h>

namespace nap
{
	class InputService;
	class PythonScriptComponent;

	class NAPAPI PythonScriptComponentInstance : public ComponentInstance
	{
		RTTI_ENABLE(ComponentInstance)
	public:
		PythonScriptComponentInstance(EntityInstance& entity) :
			ComponentInstance(entity)
		{
		}

		virtual void update(double deltaTime) override;
		virtual bool init(const ObjectPtr<Component>& resource, EntityCreationParameters& entityCreationParams, utility::ErrorState& errorState) override;

	private:
		const PythonScriptComponent* mScriptComponent = nullptr;
		pybind11::module mScript;
	};


	/**
	 * The resource class for the PythonScriptComponent.
	 */
	class NAPAPI PythonScriptComponent : public Component
	{
		RTTI_ENABLE(Component)
	public:
		virtual const rtti::TypeInfo getInstanceType() const { return RTTI_OF(PythonScriptComponentInstance); }

		std::string mPath;
	};
}