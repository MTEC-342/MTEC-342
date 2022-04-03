/*
  ==============================================================================

    Track.cpp
    Created: 6 Nov 2021 6:50:17pm
    Author:  Akito van Troyer

  ==============================================================================
*/

#include "Track.h"

Track::Track (juce::String fileName, int numBeats): numBeats (numBeats)
{
    // Load audio file
    sampleBuffer.loadAudioFileFromAsset (fileName);
    
    // Create buttons
    for(auto beat = 0; beat < numBeats; ++beat)
    {
        buttons.add (new juce::ShapeButton ("", juce::Colours::green,  juce::Colours::green,  juce::Colours::grey));
        buttons.getLast()->addListener (this);
        beats.add (new bool (false));
    }
}

void Track::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    sampleBuffer.getNextAudioBlock (bufferToFill);
}

void Track::releaseResources()
{
    sampleBuffer.releaseResources();
}

void Track::play (int beat)
{
    if (*beats[beat])
    {
        sampleBuffer.play();
    }
    curBeat = beat;
}

void Track::paint (juce::Graphics& g)
{
    for (int beat = 0 ; beat < beats.size(); ++beat)
    {
        addAndMakeVisible (buttons[beat]);
        juce::Path rectPath;
        rectPath.addRectangle (0, 0, 100, 100);
        buttons[beat]->setShape (rectPath, true, false, false);
        if (*beats[beat]){
            buttons[beat]->setColours (juce::Colours::red,  juce::Colours::red,  juce::Colours::grey);
        }
        else {
            buttons[beat]->setColours (juce::Colours::green,  juce::Colours::green,  juce::Colours::grey);
        }
        buttons[beat]->setBounds (5 + (getWidth() / numBeats) * beat, 0, getWidth() / numBeats - 10, getHeight());
    }
    
    buttons[curBeat]->setColours (juce::Colours::yellow,  juce::Colours::yellow,  juce::Colours::grey);
}

void Track::buttonClicked (juce::Button *button)
{
    auto index = buttons.indexOf ((juce::ShapeButton*) button);
    *beats[index] = !*beats[index];
}
