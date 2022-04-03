#pragma once

#include <JuceHeader.h>
#include "ParameterSlider.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    //==============================================================================
    enum
    {
        oscIndex,
        gainIndex,
        panIndex
    };
    juce::dsp::ProcessorChain<juce::dsp::Oscillator<float>, juce::dsp::Gain<float>, juce::dsp::Panner<float>> processorChain;
    
    // LFOs
    juce::dsp::Oscillator<float> tlfo;
    juce::dsp::Oscillator<float> vlfo;
    juce::dsp::Oscillator<float> plfo;
    
    // For applying LFOs
    static constexpr size_t lfoUpdateRate = 100;
    size_t lfoUpdateCounter = lfoUpdateRate;
    juce::dsp::AudioBlock<float> tempBlock;
    juce::HeapBlock<char> heapBlock;
    
    juce::OwnedArray<ParameterSlider> sliders;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
