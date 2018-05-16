#pragma once

#include <audio/core/audionode.h>
#include <audio/utility/rampedvalue.h>
#include <audio/utility/safeptr.h>

namespace nap
{
    namespace audio
    {
        
        /**
         * A wavetable that can be used as waveform data for an oscillator.
         * Contains a buffer with one cycle of samples for a periodic waveform.
         */
        class NAPAPI WaveTable
        {
        public:
            /**
             * Constructor takes the size of the waveform buffer.
             */
            WaveTable(long size);
            
            /**
             * Constructor takes the size of the waveform buffer
             */
            WaveTable(long size, SampleBuffer& spectrum, int stepSize);
            
            /**
             * Normalize the waveform so the "loudest" sample has amplitude 1.f
             */
            void normalize();
            
            /**
             * Subscript operator to access the waveform's samples
             */
            inline SampleValue& operator[](long index);
            inline SampleValue operator[](long index) const;
            
            /**
             * Read from the waveform at a certain index between 0 and @getSize()
             */
            inline SampleValue interpolate(double index) const;
            
            /**
             * Returns the size of the waveform buffer
             */
            long getSize() const { return mData.size(); }
            
        private:
            SampleBuffer mData;
        };

        
        /**
         * Oscillator that generates an audio signal from a periodic waveform and a frequency
         */
        class NAPAPI OscillatorNode : public Node
        {
            RTTI_ENABLE(Node)
            
        public:
            /**
             * Constructor takes the waveform of the oscillator
             */
            OscillatorNode(NodeManager& aManager, SafePtr<WaveTable> aWave);
            
            /**
             * Set the frequency in Hz
             */
            void setFrequency(ControllerValue frequency, TimeValue rampTime = 0);
            
            /**
             * Set the amplitude of the generated wave
             */
            void setAmplitude(ControllerValue amplitude, TimeValue rampTime = 0);
            
            /**
             * Sets the phase of the oscillator as a value between 0 and 1
             */
            void setPhase(ControllerValue phaseOffset);
            
            /**
             * Set a new waveform for the oscillator
             */
            void setWave(SafePtr<WaveTable>& aWave);
            
            /**
             * Return the frequency in Hz
             */
            ControllerValue getFrequency() const { return mFrequency.getValue(); }
            
            /**
             * Return the amplitude of the oscillation
             */
            ControllerValue getAmplitude() const { return mAmplitude.getValue(); }
            
            /**
             * Return the phase as a value between 0 and 1. representing a fraction of the current position in the wave table.
             */
            ControllerValue getPhase() const { return mPhaseOffset / float(mWave->getSize()); }

            InputPin fmInput; ///< Input pin to control frequency modulation.
            OutputPin output = { this }; ///< Audio output pin.

        private:
            void process() override;
            void sampleRateChanged(float sampleRate) override;

            SafePtr<WaveTable> mWave = nullptr;

            RampedValue<ControllerValue> mFrequency = { 0 };
            RampedValue<ControllerValue> mAmplitude = { 1.f };
            std::atomic<ControllerValue> mStep = { 0 };
            std::atomic<ControllerValue> mPhaseOffset = { 0 };
            
            ControllerValue mPhase = 0;
        };
    }
}

