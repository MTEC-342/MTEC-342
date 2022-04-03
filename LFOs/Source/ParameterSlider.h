/*
  ==============================================================================

    ParameterSlider.h
    Created: 16 Oct 2021 7:55:39pm
    Author:  Akito van Troyer

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class ParameterSlider : public juce::Component,
    public juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear>,
    private juce::Slider::Listener
{
public:
    ParameterSlider (juce::String name, float minVal, float maxVal, float step, float defaultVal)
    {
        label.setText (name, juce::dontSendNotification);
        slider.setRange (minVal, maxVal, step);
        slider.setValue (defaultVal);
        setTargetValue (defaultVal);
        slider.addListener (this);
    }
    
    ~ParameterSlider()
    {
        slider.removeListener (this);
    }
    
    void sliderValueChanged (juce::Slider *slider) override
    {
        setTargetValue (slider->getValue());
    }
    
    void paint (juce::Graphics& g) override;
    
private:
    juce::Label label;
    juce::Slider slider;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParameterSlider)
};

