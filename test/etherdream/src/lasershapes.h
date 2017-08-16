#pragma once

#include "lasershapecomponent.h"

namespace nap
{
	class LaserDotComponentInstance;

	//////////////////////////////////////////////////////////////////////////
	// Laser DOT
	//////////////////////////////////////////////////////////////////////////

	class LaserDotComponent : public LaserShapeComponent
	{
		RTTI_ENABLE(LaserShapeComponent)
	public:
		virtual const rtti::TypeInfo getInstanceType() const override
		{
			return RTTI_OF(LaserDotComponentInstance);
		}
	};

	class LaserDotComponentInstance : public LaserShapeComponentInstance
	{
		RTTI_ENABLE(LaserShapeComponentInstance)
	public:
		// Constructor
		LaserDotComponentInstance(EntityInstance& entity) : LaserShapeComponentInstance(entity) 
		{ }

		// Init
		virtual bool init(const ObjectPtr<Component>& resource, EntityCreationParameters& entityCreationParams, utility::ErrorState& errorState) override;

		// Update
		virtual void update(double deltaTime);
	};


	//////////////////////////////////////////////////////////////////////////
	// Laser Square
	//////////////////////////////////////////////////////////////////////////

	class LaserSquareComponentInstance;

	class LaserSquareComponent : public LaserShapeComponent
	{
		RTTI_ENABLE(LaserShapeComponent)
	public:
		virtual const rtti::TypeInfo getInstanceType() const override
		{
			return RTTI_OF(LaserSquareComponentInstance);
		}
	};

	class LaserSquareComponentInstance : public LaserShapeComponentInstance
	{
		RTTI_ENABLE(LaserShapeComponentInstance)
	public:
		// Constructor
		LaserSquareComponentInstance(EntityInstance& entity) : LaserShapeComponentInstance(entity)
		{ }

		// Init
		virtual bool init(const ObjectPtr<Component>& resource, EntityCreationParameters& entityCreationParams, utility::ErrorState& errorState) override;

		// Update
		virtual void update(double deltaTime);

	private:
		void fillSquare(float inLocX, float inLocY, float inBrightNess, float inSize);
	};
}
