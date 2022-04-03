/*
  ==============================================================================

    Clock.cpp
    Created: 22 Nov 2021 1:16:15pm
    Author:  Akito van Troyer

  ==============================================================================
*/

#include "Metronome.h"

// Use LOGD to print something in Android logcat
#if JUCE_ANDROID
#include <android/log.h>
#define  LOG_TAG "JUCE"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#endif

Metronome::Metronome ()
{
    sampleBuffer.loadAudioFileFromAsset ("CH.wav");
}

void Metronome::prepareToPlay (double sampleRate)
{
    interval = 60.0f / 120.0f * sampleRate;
}

void Metronome::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    sampleBuffer.getNextAudioBlock (bufferToFill);
}

void Metronome::releaseResources()
{
    sampleBuffer.releaseResources();
}

void Metronome::tick (int numSamples)
{
    if(isTicking)
    {
        elapsed += numSamples;
        if(elapsed >= interval)
        {
            elapsed -= interval;
            sampleBuffer.play();
            curBeat++;
            if (listener != nullptr)
            {
                listener->ticked();
            }
        }
    }
}

void Metronome::start()
{
    if(!isTicking) {
        isTicking = true;
        sampleBuffer.play();
        curBeat = 1;
    }
}

void Metronome::stop()
{
    if(isTicking)
    {
        isTicking = false;
        elapsed = 0;
        resetCurrentBeat();
    }
}
