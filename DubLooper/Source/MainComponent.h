#pragma once

#include <JuceHeader.h>
#include "Metronome.h"
#include "SampleBuffer.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  :
        public juce::AudioAppComponent, private juce::Button::Listener,
        private Metronome::Listener, private SampleBuffer::Listener,
        private juce::ChangeListener
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
    void ticked() override;
    void playingEnded() override;
    void recordingEnded() override;
    void changeListenerCallback (juce::ChangeBroadcaster* source) override;

private:
    //==============================================================================
    Metronome metronome;
    SampleBuffer sampleBuffer;

    juce::TextButton recordButton;
    juce::TextButton playButton;
    juce::TextButton saveButton;
    juce::Label countLabel;

    juce::AudioFormatManager formatManager;
    juce::AudioThumbnailCache thumbnailCache {5};
    juce::AudioThumbnail thumbnail {512, formatManager, thumbnailCache};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
