/*
  ==============================================================================

    SampleBuffer.cpp
    Created: 13 Dec 2021 11:45:50am
    Author:  Akito van Troyer

  ==============================================================================
*/

#include "SampleBuffer.h"
// Use LOGD to print something in Android logcat
#if JUCE_ANDROID
#include <android/log.h>
#define  LOG_TAG "JUCE"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#endif

void SampleBuffer::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (isPlaying)
    {
        auto numInputChannels = fileBuffer.getNumChannels();
        auto numOutputChannels = bufferToFill.buffer->getNumChannels();
        auto numSamples = bufferToFill.numSamples;

        // Go through all the channels available
        for (auto channel = 0; channel < numOutputChannels; ++channel)
        {
            // % numInputChannels is to compensate when the audio buffer and the main audio
            // system do not agree on the number of channels.
            auto* channelDataIn = fileBuffer.getReadPointer (channel % numInputChannels);
            auto* channelDataOut = bufferToFill.buffer->getWritePointer (channel);

            // mix in sample by sample to the main buffer
            for (auto sample = 0; sample < numSamples; ++sample)
            {
                // if the reading position exceeds the size of buffer, break out of the loop
                if (position + sample >= fileBuffer.getNumSamples()) break;
                channelDataOut[sample] += channelDataIn[position + sample];
            }
        }

        // Update the audio buffer reading position
        position += numSamples;

        // Stop the playback once the reading position reaches the size of buffer
        if (position >= fileBuffer.getNumSamples())
        {
            if (!isLooping)
            {
                isPlaying = false;
            }
            else
            {
                position = 0;
            }
        }
    }
}

void SampleBuffer::releaseResources()
{
    fileBuffer.setSize (0, 0);
}

// Play/Record managing ==============================================================================
void SampleBuffer::enableLoop (bool isLooping)
{
    this->isLooping = isLooping;
}

void SampleBuffer::play()
{
    if(!isPlaying) isPlaying = true;
    position = 0;
}

void SampleBuffer::stop()
{
    if (isPlaying) isPlaying = false;
}

// File Operations ==============================================================================
void SampleBuffer::loadAudioFileFromAsset (juce::String fileName)
{
std::unique_ptr<juce::InputStream> inputStream;
#if JUCE_ANDROID
    juce::ZipFile apkZip (juce::File::getSpecialLocation (juce::File::invokedExecutableFile));
    inputStream = std::unique_ptr<juce::InputStream> (apkZip.createStreamForEntry (apkZip.getIndexOfFileName (fileName)));
#elif JUCE_IOS
    juce::File file (fileName);
    jassert (file.existsAsFile());
    inputStream = file.createInputStream();
#endif
    // Once file opening succeeds, create a reader
    auto* reader = formatManager.createReaderFor (std::move (inputStream));
    if (reader != nullptr)
    {
        // Set the size and channel number for the audio buffer based on the sound file we read
        fileBuffer.setSize ((int) reader->numChannels, (int) reader->lengthInSamples);
        
        // Read the file content and write it in the audio buffer
        reader->read (&fileBuffer, 0, (int) reader->lengthInSamples, 0, true, true);
    }
}
