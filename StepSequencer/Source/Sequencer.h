/*
  ==============================================================================

    Sequencer.h
    Created: 6 Nov 2021 6:46:38pm
    Author:  Akito van Troyer

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Clock.h"
#include "Track.h"

class Sequencer : public juce::Component, public Clock, private Clock::Listener
{
public:
    Sequencer()
    {
        setListener (this);
    }
    void createTracks();
    void paint (juce::Graphics& g) override;
    void tick (int count) override;
    void reset() override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();
    
private:
    juce::OwnedArray<Track> tracks;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Sequencer)
};
