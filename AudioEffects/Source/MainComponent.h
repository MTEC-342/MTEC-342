#pragma once

#include <JuceHeader.h>
#include "SliderPanel.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent, private juce::MidiKeyboardStateListener
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
    void handleNoteOn (juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity) override;
    void handleNoteOff (juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity) override;
    
private:
    //==============================================================================
    enum
    {
        oscIndex,
        shaperIndex,
        filterIndex,
        gainIndex
    };
    
    juce::dsp::ProcessorChain<juce::dsp::Oscillator<float>, juce::dsp::WaveShaper<float>, juce::dsp::LadderFilter<float>, juce::dsp::Gain<float>> processorChain;
    juce::dsp::Reverb reverb;
    juce::ADSR adsr;
    
    juce::MidiKeyboardState keyboardState;
    juce::MidiKeyboardComponent keyboardComponent;
    
    juce::OwnedArray<SliderPanel> panels;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
