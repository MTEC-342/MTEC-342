#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent, private juce::Button::Listener
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
    void buttonClicked (juce::Button* button) override;
    
private:
    //==============================================================================
    juce::dsp::ProcessorChain<juce::dsp::Oscillator<float>, juce::dsp::Gain<float>> processorChain;
    enum
    {
        oscIndex,
        gainIndex
    };

    void generateSine();
    void generateSawtooth();
    void generateSquare();
    void generateTriangle();
    
    juce::TextButton sineButton;
    juce::TextButton sawtoothButton;
    juce::TextButton squareButton;
    juce::TextButton triangleButton;

    juce::AudioVisualiserComponent oscilloscope { 2 };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
