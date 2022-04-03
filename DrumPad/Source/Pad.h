/*
  ==============================================================================

    Pad.h
    Created: 20 Aug 2021 4:56:20pm
    Author:  Akito van Troyer

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SampleBuffer.h"

class Pad : public juce::ShapeButton, public juce::Button::Listener
{
public:
    Pad (juce::String fileName);
    void buttonStateChanged (juce::Button *button) override;
    void buttonClicked (juce::Button *button) override;
    void paint(juce::Graphics& g) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();

private:
    SampleBuffer sampleBuffer;
    juce::Path rectPath;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pad)
};
