#pragma once

// Local Includes
#include "oscevent.h"
#include "lineselectioncomponent.h"
#include "lineblendcomponent.h"

// External Includes
#include <nap/component.h>
#include <nap/signalslot.h>
#include <nap/componentptr.h>

#include <rotatecomponent.h>
#include <renderablemeshcomponent.h>
#include <oscinputcomponent.h>

namespace nap
{
	class OSCLaserInputHandlerInstance;

	/**
	* Interprets OSC events associated with laser shapes and actions
	*/
	class OSCLaserInputHandler : public Component
	{
		RTTI_ENABLE(Component)
		DECLARE_COMPONENT(OSCLaserInputHandler, OSCLaserInputHandlerInstance)
	public:
		// property: Link to selection component one
		ComponentPtr<LineSelectionComponent> mSelectionComponentOne = nullptr;

		// property: Link to selection component two
		ComponentPtr<LineSelectionComponent> mSelectionComponentTwo = nullptr;

		virtual void getDependentComponents(std::vector<rtti::TypeInfo>& components) const override;
	};


	class OSCLaserInputHandlerInstance : public ComponentInstance
	{
		RTTI_ENABLE(ComponentInstance)
	public:
		// Default constructor
		OSCLaserInputHandlerInstance(EntityInstance& entity, Component& resource) : ComponentInstance(entity, resource) { }

		// Destructor
		virtual ~OSCLaserInputHandlerInstance();

		/**
		 *	Retrieve necessary components for osc input translation
		 */
		virtual bool init(EntityCreationParameters& entityCreationParams, utility::ErrorState& errorState) override;

	private:
		void handleMessageReceived(const nap::OSCEvent& oscEvent);

		nap::RotateComponentInstance* mRotateComponent = nullptr;
		nap::OSCInputComponentInstance* mInputComponent = nullptr;
		nap::LineBlendComponentInstance* mBlendComponent = nullptr;

		void updateColor(const OSCEvent& event, int index, int channel);
		void updateRotate(const OSCEvent& event);
		void resetRotate(const OSCEvent& event);
		void setIndex(const OSCEvent& event, int index);
		void setBlend(const OSCEvent& event, int index);

		NSLOT(mMessageReceivedSlot, const nap::OSCEvent&, handleMessageReceived)

		LineSelectionComponentInstance* mSelectorOne = nullptr;		// First line selection component
		LineSelectionComponentInstance* mSelectorTwo = nullptr;		// Second line selection component

	};
}
