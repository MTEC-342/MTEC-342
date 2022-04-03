/*
  ==============================================================================

    Pad.cpp
    Created: 20 Aug 2021 4:56:20pm
    Author:  Akito van Troyer

  ==============================================================================
*/

#include "Pad.h"

// Use LOGD to print something in Android logcat
#if JUCE_ANDROID
#include <android/log.h>
#define  LOG_TAG "JUCE"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#endif

Pad::Pad (juce::String fileName)
    : juce::ShapeButton ("", juce::Colours::red, juce::Colours::red, juce::Colours::grey)
{
    // Load audio file
    sampleBuffer.loadAudioFileFromAsset (fileName);
    
    // Tell the button that this class is the listener
    addListener (this);
}

void Pad::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    sampleBuffer.getNextAudioBlock (bufferToFill);
}

void Pad::releaseResources()
{
    sampleBuffer.releaseResources();
}

void Pad::paint(juce::Graphics& g)
{
    this->juce::ShapeButton::paint (g);
    rectPath.addRectangle (0, 0, 100, 100);
    setShape (rectPath, true, false, false);
}

void Pad::buttonStateChanged (juce::Button *button)
{
    if (button->getState() == juce::Button::ButtonState::buttonDown)
    {
        sampleBuffer.play();
    }
}

void Pad::buttonClicked (juce::Button *button){}
