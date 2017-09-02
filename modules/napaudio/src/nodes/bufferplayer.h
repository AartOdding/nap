#pragma once

// Audio includes
#include "audionode.h"
#include "audioservice.h"

namespace nap {
    
    namespace audio {
        
        class BufferPlayer : public AudioNode {
        public:
            BufferPlayer(AudioService& service) : AudioNode(service) { }
        
            AudioOutput audioOutput = { this, &BufferPlayer::calculate };
            
            void play(SampleBufferPtr buffer, DiscreteTimeValue position = 0, ControllerValue speed = 1.);
            void stop();
            
        private:
            void calculate(SampleBuffer& outputBuffer);
  
            bool mPlaying = false;
            DiscreteTimeValue mPosition = 0;
            ControllerValue mSpeed = 1.f;
            SampleBufferPtr mBuffer = nullptr;
        };
        
    }
}
