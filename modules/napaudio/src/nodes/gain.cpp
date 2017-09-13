#include "gain.h"

namespace nap {
    
    namespace audio {
        
        void Gain::process()
        {
            auto& buffer = getOutputBuffer(audioOutput);
            SampleBuffer& inputBuffer = *audioInput.pull();
            
            for (auto i = 0; i < buffer.size(); ++i)
                buffer[i] = inputBuffer[i] * mGain;
        }
        
    }
    
}
