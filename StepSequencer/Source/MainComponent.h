#pragma once

#include <JuceHeader.h>
#include "Sequencer.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent, private juce::Button::Listener, private juce::Slider::Listener
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

    void buttonClicked (juce::Button *button) override;
    void sliderValueChanged(juce::Slider *slider) override;
    
private:
    //==============================================================================
    Sequencer sequencer;
    juce::TextButton playButton;
    juce::Slider bpmSlider;
    bool isPlaying = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
