#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent,
private juce::MidiKeyboardStateListener, private juce::Button::Listener, private juce::Slider::Listener
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

    //==============================================================================
    // Listeners for keyboard, buttons, and sliders
    void handleNoteOn (juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity) override;
    void handleNoteOff (juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity) override;
    void buttonClicked (juce::Button *) override;
    void sliderValueChanged (juce::Slider *slider) override;

private:
    //==============================================================================
    juce::dsp::ProcessorChain<juce::dsp::Oscillator<float>, juce::dsp::Gain<float>> processorChain;

    enum
    {
        oscIndex,
        gainIndex
    };

    // Keyboard
    juce::MidiKeyboardState keyboardState;
    juce::MidiKeyboardComponent keyboardComponent;
    juce::ADSR adsr;

    // Sliders and buttons
    juce::OwnedArray<juce::Slider> sliders;
    juce::OwnedArray<juce::DrawableButton> buttons;

    juce::Image loadImage (juce::String fileName);
    
    void generateSine();
    void generateSawtooth();
    void generateSquare();
    void generateTriangle();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

