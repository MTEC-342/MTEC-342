/*
  ==============================================================================

    SampleBuffer.h
    Created: 22 Nov 2021 1:15:21pm
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
    void stopPlaying();
    void record (int numChannels, int numSamples);
    void stopRecording();
    void enableLoop (bool isLooping);
    
    // File managing
    void loadAudioFileFromAsset (juce::String fileName);
    void write (juce::String fileName);
    void read (juce::String fileName);

    bool getIsPlaying() const
    {
        return isPlaying;
    }

    bool getIsRecording() const
    {
        return isRecording;
    }

    int getNumSamples() const
    {
        return fileBuffer.getNumSamples();
    }

    juce::AudioBuffer<float> getBuffer()
    {
        return fileBuffer;
    }

    class Listener
    {
    public:
        virtual ~Listener(){}
        virtual void playingEnded() = 0;
        virtual void recordingEnded() = 0;
    };

    void setListener(Listener* listener)
    {
        this->listener = listener;
    }

private:

    juce::AudioFormatManager formatManager;
    juce::AudioBuffer<float> fileBuffer;

    int position = 0;
    bool isPlaying = false;
    bool isLooping = false;
    bool isRecording = false;

    Listener* listener = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleBuffer)
};
