/*
  ==============================================================================

    Synth.cpp
    Created: 16 Aug 2021 9:21:07pm
    Author:  Akito van Troyer

  ==============================================================================
*/

#include "Synth.h"

void Synth::prepareToPlay (int samplesPerBlockExpected, double sampleRate, int numChans)
{
    // Intialize processor chain
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlockExpected;
    spec.numChannels = numChans;
    processorChain.prepare (spec);
    
    // Oscillator setup
    auto& osc = processorChain.template get<oscIndex>();
    osc.initialise ([] (float x)
        {
            return juce::jmap (x,
                               float (-juce::MathConstants<float>::pi),
                               float (juce::MathConstants<float>::pi),
                               float (-1),
                               float (1));
        }, 2);
    processorChain.get<oscIndex>().setFrequency (440, true);
    
    // Filter setup
    processorChain.get<filterIndex>().setMode (juce::dsp::LadderFilterMode::LPF24);
    processorChain.get<filterIndex>().setCutoffFrequencyHz (5000.0f);
    processorChain.get<filterIndex>().setResonance (0.7f);
    
    // Gain setup
    processorChain.get<gainIndex>().setGainLinear (0.05f);
    
    // ADSR setup
    adsr.setSampleRate (sampleRate);
    adsr.reset();
    
    this->numChans = numChans;
}

void Synth::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    juce::AudioBuffer<float> audioBuffer = *bufferToFill.buffer;
    auto numSamples = audioBuffer.getNumSamples();

    buffer.makeCopyOf (audioBuffer);
    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::ProcessContextReplacing<float> context (block);
    processorChain.process (context);
    adsr.applyEnvelopeToBuffer (buffer, 0, buffer.getNumSamples());
    
    // Copy audio data to the main audio buffer
    for (int channel = 0; channel < numChans; ++channel)
    {
        auto* channelDataIn = buffer.getReadPointer (channel);
        auto* channelDataOut = audioBuffer.getWritePointer (channel);
        for (int sample = 0; sample < numSamples; ++sample)
        {
            channelDataOut[sample] += channelDataIn[sample];
        }
    }
}
