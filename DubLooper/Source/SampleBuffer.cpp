/*
  ==============================================================================

    SampleBuffer.cpp
    Created: 22 Nov 2021 1:15:21pm
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
                if(listener != nullptr)
                {
                    listener->playingEnded();
                }
            }
            else
            {
                position = 0;
            }
        }
    }
    else if (isRecording)
    {
        auto numOutputChannels = bufferToFill.buffer->getNumChannels();
        auto numSamples = bufferToFill.numSamples;

        // Go through all the channels available
        for (auto channel = 0; channel < numOutputChannels; ++channel)
        {
            // system do not agree on the number of channels.
            auto* channelDataIn = fileBuffer.getWritePointer (channel);
            auto* channelDataOut = bufferToFill.buffer->getReadPointer (channel);

            // mix in sample by sample to the main buffer
            for (auto sample = 0; sample < numSamples; ++sample)
            {
                // if the reading position exceeds the size of buffer, break out of the loop
                if (position + sample >= fileBuffer.getNumSamples()) break;
                channelDataIn[position + sample] += channelDataOut[sample];
            }
        }

        // Update the audio buffer writing position
        position += numSamples;

        // Stop the recording once the reading position reaches the size of buffer
        if (position >= fileBuffer.getNumSamples())
        {
            isRecording = false;
            if(listener != nullptr)
            {
                listener->recordingEnded();
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
    if(!isPlaying)
    {
        isPlaying = true;
        stopRecording();
    }
    position = 0;
}

void SampleBuffer::stopPlaying()
{
    if (isPlaying) isPlaying = false;
}

void SampleBuffer::record(int numChannels, int numSamples)
{
    if(!isRecording)
    {
        fileBuffer.setSize(numChannels, numSamples);
        isRecording = true;
        stopPlaying();
        position = 0;
    }
}

void SampleBuffer::stopRecording()
{
    if(isRecording) isRecording = false;
}

// File Operations ==============================================================================
void SampleBuffer::loadAudioFileFromAsset (juce::String fileName)
{
    std::unique_ptr<juce::InputStream> inputStream;
#if JUCE_ANDROID
    juce::ZipFile apkZip (juce::File::getSpecialLocation (juce::File::invokedExecutableFile));
    jassert(apkZip.getIndexOfFileName ("assets/Sounds/" + fileName) > -1);
    auto* entry = apkZip.getEntry (apkZip.getIndexOfFileName ("assets/Sounds/" + fileName));
    inputStream = std::unique_ptr<juce::InputStream> (apkZip.createStreamForEntry (apkZip.getIndexOfFileName (entry->filename)));
#elif JUCE_IOS
    auto assetsDir = juce::File::getSpecialLocation (juce::File::currentExecutableFile)
                                  .getParentDirectory().getChildFile ("Assets");
    juce::File file (assetsDir.getFullPathName() + "/Sounds/" + fileName);
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

void SampleBuffer::write(juce::String fileName) {
    
    // Get the file stream for saving wave file
    auto parentDir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory);
    juce::File file(parentDir.getFullPathName() + "/" + fileName);
    std::unique_ptr<juce::FileOutputStream> fileStream (file.createOutputStream());
    
    if(fileStream != nullptr)
    {
        // Tell file stream to start from the beginning of the file and delete existing data
        fileStream->setPosition (0);
        fileStream->truncate();
        
        // Create a writer
        juce::WavAudioFormat format;
        std::unique_ptr<juce::AudioFormatWriter> writer;
        writer.reset (format.createWriterFor (fileStream.get(), 44100, 1, 24, {}, 0));
        
        if (writer != nullptr) // Write the buffer content if the writer exits
        {
            fileStream.release(); // passes responsibility for deleting the stream to the writer object
            writer->writeFromAudioSampleBuffer(fileBuffer, 0, fileBuffer.getNumSamples());
        }
    }
}

void SampleBuffer::read(juce::String fileName)
{
    // Get the full file path
    auto parentDir = juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory);
    auto file = parentDir.getChildFile (fileName);
    
    if(file.existsAsFile()) // Check if the file exists
    {
        auto inputStream = file.createInputStream(); // Create an input stream
        
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
}
