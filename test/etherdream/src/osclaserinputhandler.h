#pragma once

// Local Includes
#include "oscevent.h"
#include "lineblendcomponent.h"
#include "linecolorcomponent.h"
#include "laseroutputcomponent.h"
#include "linemodulationcomponent.h"
#include "linenoisecomponent.h"
#include "linetracecomponent.h"
#include "lineautoswitchcomponent.h"
#include "xformsmoothcomponent.h"

// External Includes
#include <nap/component.h>
#include <nap/signalslot.h>
#include <nap/componentptr.h>

#include <rotatecomponent.h>
#include <renderablemeshcomponent.h>
#include <oscinputcomponent.h>
#include <transformcomponent.h>


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
		ObjectPtr<LineSelectionComponent> mSelectionComponentOne = nullptr;

		// property: Link to selection component two
		ObjectPtr<LineSelectionComponent> mSelectionComponentTwo = nullptr;

		// property: If the pixel color should be printed
		bool mPrintColor = false;

		virtual void getDependentComponents(std::vector<rtti::TypeInfo>& components) const override;
	};


	/**
	 *	Helper component that translates received OSC events in to specific app actions
	 */
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

		/**
		 *	Sets the entity to be used as the laser output
		 */
		void setLaserOutput(nap::EntityInstance& entity);

	private:
		void handleMessageReceived(const nap::OSCEvent& oscEvent);

		nap::RotateComponentInstance*		mRotateComponent = nullptr;
		nap::OSCInputComponentInstance*		mInputComponent = nullptr;
		nap::LineBlendComponentInstance*	mBlendComponent = nullptr;
		nap::TransformComponentInstance*	mTransformComponent = nullptr;
		nap::LineNoiseComponentInstance*	mNoiseComponent = nullptr;
		nap::LineTraceComponentInstance*	mTraceComponent = nullptr;
		nap::XformSmoothComponentInstance*	mXformSmoother = nullptr;

		void updateStartColor(const OSCEvent& event, const std::vector<std::string>& args);
		void updateEndColor(const OSCEvent& event, const std::vector<std::string>& args);
		void updateXStartColor(const OSCEvent& event, const std::vector<std::string>& args);
		void updateYStartColor(const OSCEvent& event, const std::vector<std::string>& args);
		void updateXEndColor(const OSCEvent& event, const std::vector<std::string>& args);
		void updateYEndColor(const OSCEvent& event, const std::vector<std::string>& args);
		void updateRotate(const OSCEvent& event, const std::vector<std::string>& args);
		void resetRotate(const OSCEvent& event, const std::vector<std::string>& args);
		void setBlend(const OSCEvent& event, const std::vector<std::string>& args);
		void setScale(const OSCEvent& event, const std::vector<std::string>& args);
		void setPosition(const OSCEvent& event, const std::vector<std::string>& args);
		void setPositionX(const OSCEvent& event, const std::vector<std::string>& args);
		void setPositionY(const OSCEvent& event, const std::vector<std::string>& args);
		void setModulation(const OSCEvent& event, const std::vector<std::string>& args);
		void setNoise(const OSCEvent& event, const std::vector<std::string>& args);
		void setColorSync(const OSCEvent& event, const std::vector<std::string>& args);
		void updateTracer(const OSCEvent& event, const std::vector<std::string>& args);
		void resetTracer(const OSCEvent& event, const std::vector<std::string>& args);
		void setIntensity(const OSCEvent& event, const std::vector<std::string>& args);
		void selectNextLine(const OSCEvent& event, const std::vector<std::string>& args);
		void toggleRandom(const OSCEvent& event, const std::vector<std::string>& args);
		void updateColor(const glm::vec2& loc, int position);
		void resetBlend(const OSCEvent& event, const std::vector<std::string>& args);
		void setColorSmoothX(const OSCEvent& event, const std::vector<std::string>& args);
		void setColorSmoothY(const OSCEvent& event, const std::vector<std::string>& args);

		NSLOT(mMessageReceivedSlot, const nap::OSCEvent&, handleMessageReceived)

		LaserOutputComponentInstance* mLaserOutput = nullptr;				// Laser output component
		LineColorComponentInstance* mColorComponent = nullptr;				// Laser line color component
		LineModulationComponentInstance* mModulationComponent = nullptr;	// Laser modulation component
		LineAutoSwitchComponentInstance* mSwitcher = nullptr;				// Switches lines
		bool mPrintColor = false;											// If color should be printed

		// This map holds all the various callbacks based on id
		typedef void (OSCLaserInputHandlerInstance::*LaserEventFunc)(const OSCEvent&, const std::vector<std::string>& args);
		std::unordered_map<std::string, LaserEventFunc> mLaserEventFuncs;

		float mInitialScale = 1.0f;									// Holds the initial scale of the laser spline entity
	};
}
