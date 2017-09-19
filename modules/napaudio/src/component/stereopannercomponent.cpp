#include "stereopannercomponent.h"

// Nap includes
#include <nap/entity.h>

// Audio includes
#include <node/audionodemanager.h>

// RTTI
RTTI_BEGIN_CLASS(nap::audio::StereoPannerComponent)
    RTTI_PROPERTY("Input", &nap::audio::StereoPannerComponent::mInput, nap::rtti::EPropertyMetaData::Required)
    RTTI_PROPERTY("Panning", &nap::audio::StereoPannerComponent::mPanning, nap::rtti::EPropertyMetaData::Default)
RTTI_END_CLASS

RTTI_BEGIN_CLASS_NO_DEFAULT_CONSTRUCTOR(nap::audio::StereoPannerComponentInstance)
    RTTI_CONSTRUCTOR(nap::EntityInstance&, nap::Component&)
RTTI_END_CLASS

namespace nap {
    
    namespace audio {
        
        bool StereoPannerComponentInstance::init(EntityCreationParameters& entityCreationParams, utility::ErrorState& errorState)
        {
            StereoPannerComponent* resource = rtti_cast<StereoPannerComponent>(getComponent());
            
            AudioComponentInstance* input = resource->mInput.get();
            auto& nodeManager = getNodeManager();
            stereoPanner = std::make_unique<StereoPanner>(nodeManager);
            stereoPanner->setPanning(resource->mPanning);
            
            // mono input
            if (input->getChannelCount() == 1)
            {
                stereoPanner->leftInput.connect(input->getOutputForChannel(0));
                stereoPanner->rightInput.connect(input->getOutputForChannel(0));
            }
            
            // stereo input
            else if (input->getChannelCount() > 1)
            {
                stereoPanner->leftInput.connect(input->getOutputForChannel(0));
                stereoPanner->rightInput.connect(input->getOutputForChannel(1));
            }
            
            return true;
        }

        
        OutputPin& StereoPannerComponentInstance::getOutputForChannel(int channel)
        {
            if (channel == 0)
                return stereoPanner->leftOutput;
            else
                return stereoPanner->rightOutput;
        }
        
        
    }
    
}
