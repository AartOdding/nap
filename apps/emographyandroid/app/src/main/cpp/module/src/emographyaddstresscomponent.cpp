#include "emographyaddstresscomponent.h"
#include "emographystress.h"

// External Includes
#include <entity.h>

// nap::emographystorestresscomponent run time class definition 
RTTI_BEGIN_CLASS(nap::emography::AddStressSampleComponent)
	RTTI_PROPERTY("DataModel", &nap::emography::AddStressSampleComponent::mDataModel, nap::rtti::EPropertyMetaData::Required)
RTTI_END_CLASS

// nap::emographystorestresscomponentInstance run time class definition 
RTTI_BEGIN_CLASS_NO_DEFAULT_CONSTRUCTOR(nap::emography::AddStressSampleComponentInstance)
	RTTI_CONSTRUCTOR(nap::EntityInstance&, nap::Component&)
RTTI_END_CLASS

//////////////////////////////////////////////////////////////////////////


namespace nap
{
	namespace emography
	{
		void AddStressSampleComponent::getDependentComponents(std::vector<rtti::TypeInfo>& components) const
		{

		}


		bool AddStressSampleComponentInstance::init(utility::ErrorState& errorState)
		{
			mDataModel = &(getComponent<AddStressSampleComponent>()->mDataModel->getInstance());
			return true;
		}


		bool AddStressSampleComponentInstance::addSample(const TimeStamp& timeStamp, float stressValue, int stressState, utility::ErrorState& error)
		{
			std::unique_ptr<StressIntensityReading> intensityReading = std::make_unique<StressIntensityReading>(stressValue, timeStamp.toSystemTime());
			if (!mDataModel->add(*intensityReading, error))
			{
				error.fail("failed to add additional stress sample");
				return false;
			}
			return true;
		}
	}
}