#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    setSize (600, 400);
}

MainComponent::~MainComponent()
{
    backgroundSlider.removeListener (this);
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (backgroundColor);

    g.setFont (juce::Font (24.0f));
    g.setColour (juce::Colours::grey);
    g.drawText ("Hello, JUCE!", getLocalBounds().reduced(20, 20), juce::Justification::centred, true);
    
    // Change knob and track color
    getLookAndFeel().setColour (juce::Slider::thumbColourId, juce::Colours::red);
    getLookAndFeel().setColour (juce::Slider::trackColourId, juce::Colours::grey);

    // Set the text box style
    backgroundSlider.setTextBoxStyle (juce::Slider::TextBoxRight, true, 100, 20);

    // Set the size of slider
    backgroundSlider.setBounds (20, getHeight() / 2 + 30, getWidth() - 40, 20);
        
    // add the slider to the MainComponent
    addAndMakeVisible (backgroundSlider);
    
    //Add the lister to the slider. MainComponent will be the listener
    backgroundSlider.addListener (this);
}

void MainComponent::resized()
{
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}

void MainComponent::sliderValueChanged(juce::Slider* slider){
    uint8_t color = slider->getValue() * 25.5;
    backgroundColor = juce::Colour(color, color, color);
    repaint();
}
