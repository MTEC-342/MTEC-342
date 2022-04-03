/*
  ==============================================================================

    Panel.cpp
    Created: 12 Aug 2021 1:27:56pm
    Author:  Akito van Troyer

  ==============================================================================
*/

#include "SliderPanel.h"

SliderPanel::SliderPanel (juce::String name,
                          std::vector<juce::String> parameters,
                          std::vector<float> minVals,
                          std::vector<float> maxVals,
                          std::vector<float> steps,
                          std::vector<float> defaultVals)
{
    label.setText (name, juce::dontSendNotification);
    
    for (int i = 0; i < parameters.size(); ++i)
    {
        sliders.add (new ParameterSlider (parameters[i], minVals[i], maxVals[i], steps[i], defaultVals[i]));
    }
}

void SliderPanel::paint (juce::Graphics& g)
{
    g.setColour (juce::Colours::grey);
    g.drawRect (0, 0, getWidth(), getHeight());

    addAndMakeVisible (label);
    label.setJustificationType (juce::Justification::left);
    label.setBounds (0, 0, getWidth(), getHeight() * 0.2f);
    
    for (int i = 0; i < sliders.size(); ++i)
    {
        addAndMakeVisible (sliders[i]);
        sliders[i]->setBounds (i * getWidth() / sliders.size(), getHeight() * 0.2f, getWidth() / sliders.size(), getHeight() * 0.8f);
    }
}

void SliderPanel::reset (float sampleRate, float rampLengthInSeconds)
{
    for (int i = 0; i < sliders.size(); ++i)
    {
        sliders[i]->reset (sampleRate, rampLengthInSeconds);
    }
}

float SliderPanel::getNextValue (int index)
{
    return sliders[index]->getNextValue();
}
