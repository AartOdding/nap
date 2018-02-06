#pragma once

// Nap includes
#include <rtti/objectptr.h>

// Audio includes
#include <core/audioobject.h>
#include <node/gainnode.h>

namespace nap
{
    
    namespace audio
    {
        
        class Gain : public MultiChannelObject
        {
            RTTI_ENABLE(MultiChannelObject)
            
        public:
            Gain() = default;
            
            int mChannelCount = 1;
            std::vector<ControllerValue> mGain = { 1.f };
            std::vector<rtti::ObjectPtr<AudioObject>> mInputs;
            
        private:
            std::unique_ptr<Node> createNode(int channel, NodeManager& nodeManager) override
            {
                auto node = std::make_unique<GainNode>(nodeManager);
                node->setGain(mGain[channel % mGain.size()]);
                for (auto& input : mInputs)
                    if (input != nullptr)
                    {
                        node->inputs.connect(input->getInstance()->getOutputForChannel(channel % input->getInstance()->getChannelCount()));
                    }
                
                return std::move(node);
            }
            
            int getChannelCount() const override { return mChannelCount; }
        };
        
        
    }
    
}

