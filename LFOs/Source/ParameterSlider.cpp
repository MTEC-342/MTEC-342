/*
  ==============================================================================

    ParameterSlider.cpp
    Created: 16 Oct 2021 7:55:40pm
    Author:  Akito van Troyer

  ==============================================================================
*/

#include "ParameterSlider.h"

void ParameterSlider::paint (juce::Graphics& g)
{
    addAndMakeVisible (label);
    label.attachToComponent (&slider, false);
    label.setJustificationType (juce::Justification::centred);
    
    addAndMakeVisible (slider);
    slider.setSliderStyle (juce::Slider::Rotary);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, true, getWidth() * 0.5f, 20);
    slider.setBounds (0, getHeight() * 0.2f, getWidth(), getHeight() * 0.8f);
}
