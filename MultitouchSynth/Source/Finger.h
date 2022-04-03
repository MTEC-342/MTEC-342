/*
  ==============================================================================

    Finger.h
    Created: 16 Aug 2021 9:20:59pm
    Author:  Akito van Troyer

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Synth.h"

class Finger
{
public:
    Finger(juce::Rectangle<int> bound) : bound (bound) {}
    
    void pushPoint (juce::Point<float> newPoint, float pressure);
    void draw (juce::Graphics& g);
    
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate, int numChans);
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill);
    
    void clearPath()
    {
        path.clear();
    }
    
    void setIsTouching(const bool isTouching)
    {
        this->isTouching = isTouching;
    }
    
    bool getIsTouching() const
    {
        return isTouching;
    }
    
    void setLastPoint (const juce::Point<float> point)
    {
        lastPoint = point;
    }
    
    void play()
    {
        synth.on();
    }
    
    void stop()
    {
        synth.off();
    }
    
private:
    juce::Rectangle<int> bound;
    juce::Path path;
    juce::Colour color  { juce::Colour::fromHSV (juce::Random::getSystemRandom().nextFloat(), 0.5f, 0.8f, 1.0f) };
    juce::Point<float> lastPoint, currentPosition;
    bool isTouching { false };
    Synth synth;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Finger)
};
