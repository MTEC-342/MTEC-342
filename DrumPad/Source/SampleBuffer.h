/*
  ==============================================================================

    SampleBuffer.h
    Created: 13 Dec 2021 11:45:50am
    Author:  Akito van Troyer

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class SampleBuffer
{
public:
    SampleBuffer()
    {
        // Register the audio formats that come with JUCE
        formatManager.registerBasicFormats();
    }

    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();
    
    // Play/Record functions
    void play();
    void stop();
    void record (int numChannels, int numSamples);
    void enableLoop (bool isLooping);
    
    // File managing
    void loadAudioFileFromAsset (juce::String fileName);

    bool getIsPlaying() const
    {
        return isPlaying;
    }

    int getNumSamples() const
    {
        return fileBuffer.getNumSamples();
    }

    juce::AudioBuffer<float> getBuffer()
    {
        return fileBuffer;
    }
    
private:

    juce::AudioFormatManager formatManager;
    juce::AudioBuffer<float> fileBuffer;

    int position = 0;
    bool isPlaying = false;
    bool isLooping = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleBuffer)
};
