/*
  ==============================================================================

    Synth.h
    Created: 16 Aug 2021 9:21:07pm
    Author:  Akito van Troyer

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class Synth
{
public:
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate, int numChans);
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill);

    void on()
    {
        adsr.noteOn();
    }

    void off()
    {
        adsr.noteOff();
    }
    
    void setFrequency (const float frequency)
    {
        processorChain.get<oscIndex>().setFrequency (frequency);
    }
    
    void setCutoffFrequency (const float cutoff)
    {
        processorChain.get<filterIndex>().setCutoffFrequencyHz (cutoff);
    }
    
private:
    enum
    {
        oscIndex,
        filterIndex,
        gainIndex
    };
    
    juce::AudioBuffer<float> buffer;
    int numChans { 2 };
    juce::dsp::ProcessorChain<juce::dsp::Oscillator<float>, juce::dsp::LadderFilter<float>, juce::dsp::Gain<float>> processorChain;
    juce::ADSR adsr;
};
