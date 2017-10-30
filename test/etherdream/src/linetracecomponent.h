#pragma once

// Local Includes
#include "lineblendcomponent.h"

// external includes
#include <nap/entityptr.h>
#include <smoothdamp.h>

namespace nap
{
	struct TraceProperties
	{
		/**
		 * Holds all the properties associated with the tracer
		 */
		float mOffset = 0.0f;				// the offset of the tracer along the line (0-1)
		float mOffsetSmoothTime = 0.1f;		// Offset smooth time 
		float mSpeed  = 0.0f;				// speed at which to move the tracer along the line
		float mSpeedSmoothTime = 0.1f;		// Speed smooth time
		float mLength = 0.25f;				// length of the tracer relative to the source (1.0 = full length)
		float mLengthSmoothTime = 0.1f;		// Length smooth time
	};


	class LineTraceComponentInstance;
	
	class LineTraceComponent : public Component
	{
		RTTI_ENABLE(Component)
		DECLARE_COMPONENT(LineTraceComponent, LineTraceComponentInstance)
	public:
		// Trace properties
		TraceProperties mProperties;

		// property: Link to the tracer mesh that is computed by the instance of this component
		ObjectPtr<nap::PolyLine> mTargetLine;

		// property: Link to the trace visualizer we want to spawn on creation
		ObjectPtr<nap::Entity> mVisualizeEntity;

		// property: Link to the line blend component that holds the line we want to trace
		ObjectPtr<nap::LineBlendComponent> mBlendComponent = nullptr;
	};


	/**
	 * Computes a line that moves along the path of a parent (source) line
	 * The traced mesh is stored in the target and has a fixed amount of points
	 * Only color and position and traced, other attributes are not interpolated
	 */
	class LineTraceComponentInstance : public ComponentInstance
	{
		RTTI_ENABLE(ComponentInstance)
	public:
		LineTraceComponentInstance(EntityInstance& entity, Component& resource) :
			ComponentInstance(entity, resource) {}

		/**
		 *	Initializes this component
		 */
		virtual bool init(EntityCreationParameters& entityCreationParams, utility::ErrorState& errorState) override;

		/**
		 *	Updates the tracer's vertices along the line
		 */
		virtual void update(double deltaTime) override;

		/**
		 *	Resets the tracer's time and therefore position
		 */
		void reset();

		// Properties associated with this line
		TraceProperties mProperties;

		/**
		 *	Set the polyline that stores the trace result
		 */
		void setPolyLine(nap::PolyLine& line);

	private:
		ComponentPtr<LineBlendComponent>	mBlendComponent = { this, &LineTraceComponent::mBlendComponent };		// Component that holds the line we want to modulate
		nap::PolyLine*  mTarget = nullptr;								// Line that is the output of the trace computation
		float mCurrentTime = 0.0f;										// Current time
		nap::TransformComponentInstance* mStartXform = nullptr;			// Transform associated with beginning of trace component
		nap::TransformComponentInstance* mEndXform = nullptr;			// Transform associated with end of trace component

		// Smooths amplitude over time
		math::SmoothOperator<float> mLengthSmoother{ 1.0f, 0.1f };

		// Smooths Speed over time
		math::SmoothOperator<float> mSpeedSmoother{ 0.0f, 0.1f };

		// Smooths offset over time
		math::SmoothOperator<float> mOffsetSmoother{ 0.0f, 0.1f };
	};
}
