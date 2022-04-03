/*
  ==============================================================================

    Track.h
    Created: 6 Nov 2021 6:50:17pm
    Author:  Akito van Troyer

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "SampleBuffer.h"

class Track: public juce::Component, private juce::Button::Listener
{
public:
    Track (juce::String fileName, int numBeats);
    void buttonClicked (juce::Button *button) override;
    
    void paint(juce::Graphics& g) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();
    
    void play (int beat);
    
private:
    SampleBuffer sampleBuffer;
    
    int numBeats;
    int curBeat = 0;
    juce::OwnedArray<bool> beats;
    juce::OwnedArray<juce::ShapeButton> buttons;
    bool isPlaying = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Track)
};
