/*
  ==============================================================================

    Sequencer.cpp
    Created: 6 Nov 2021 6:46:38pm
    Author:  Akito van Troyer

  ==============================================================================
*/

#include "Sequencer.h"

void Sequencer::createTracks()
{
#if JUCE_ANDROID
    juce::ZipFile apkZip (juce::File::getSpecialLocation (juce::File::invokedExecutableFile));
    for (int i = 0; i < apkZip.getNumEntries(); ++i)
    {
        auto* entry = apkZip.getEntry (i);
        if (entry->filename.contains ("assets/Sounds/"))
        {
            tracks.add (new Track (entry->filename, getStep()));
        }
    }
#elif JUCE_IOS
    auto assetsDir = juce::File::getSpecialLocation (juce::File::currentExecutableFile)
                              .getParentDirectory().getChildFile ("Assets");
    auto soundDir = assetsDir.getChildFile ("Sounds");
    auto files = soundDir.findChildFiles (juce::File::TypesOfFileToFind::findFiles, false, "*.wav");
    for (int i = 0; i < files.size(); ++i)
    {
        tracks.add (new Track (files[i].getFullPathName(), getStep()));
    }
#endif
}

void Sequencer::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    for (auto track = 0; track < tracks.size(); ++track)
    {
        tracks[track]->getNextAudioBlock (bufferToFill);
    }
}

void Sequencer::releaseResources()
{
    stop();
    for (auto track = 0; track < tracks.size(); ++track)
    {
        tracks[track]->releaseResources();
    }
}

void Sequencer::tick (int count)
{
    for (auto track = 0; track < tracks.size(); ++track)
    {
        tracks[track]->play (count);
    }
}

void Sequencer::reset()
{
    
}

void Sequencer::paint (juce::Graphics& g)
{
    for (auto track = 0; track < tracks.size(); ++track) {
        addAndMakeVisible (tracks[track]);
        tracks[track]->setBounds (5, 5 + (getHeight() / tracks.size()) * track, getWidth() - 10, getHeight() / tracks.size() - 10);
    }
}
