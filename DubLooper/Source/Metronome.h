/*
  ==============================================================================

    Clock.h
    Created: 22 Nov 2021 1:16:15pm
    Author:  Akito van Troyer

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "SampleBuffer.h"

class Metronome {
public:
    Metronome();
    void prepareToPlay (double sampleRate);
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();
    void tick (int numSamples);
    void start();
    void stop();

    bool getIsTicking()
    {
        return isTicking;
    };

    int getCurrentBeat() const
    {
        return curBeat;
    }

    void resetCurrentBeat()
    {
        curBeat = 0;
    }

    int getInterval() const
    {
        return interval;
    }

    class Listener
    {
    public:
        virtual ~Listener(){}
        virtual void ticked() = 0;
    };

    void setListener (Listener* listener)
    {
        this->listener = listener;
    }
private:

    SampleBuffer sampleBuffer;
    int interval = 0;
    int elapsed = 0;
    int curBeat = 0;
    bool isTicking = false;

    Listener* listener = nullptr;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Metronome)
};
