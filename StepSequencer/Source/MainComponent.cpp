#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (0, 2);
    }
    
    playButton.addListener (this);
    playButton.setButtonText ("Start");
    bpmSlider.addListener (this);
    bpmSlider.setRange (60.0f, 360.0f, 1.0f);
    bpmSlider.setValue (120);
    bpmSlider.setTextBoxStyle (juce::Slider::TextBoxRight, true, 80, 20);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    sequencer.createTracks();
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    sequencer.getNextAudioBlock (bufferToFill);
}

void MainComponent::releaseResources()
{
    sequencer.releaseResources();
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    addAndMakeVisible(sequencer);
    sequencer.setBounds (50, 50, getWidth() - 100, getHeight() - 100);
    
    addAndMakeVisible (playButton);
    playButton.setBounds (60, getHeight() - 50, 100, 30);
    
    addAndMakeVisible (bpmSlider);
    bpmSlider.setBounds (160, getHeight() - 50, getWidth() - 235, 30);
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}

void MainComponent::buttonClicked (juce::Button *button)
{
    if (isPlaying)
    {
        isPlaying = false;
        sequencer.stop();
        playButton.setButtonText ("Start");
    }
    else {
        isPlaying = true;
        sequencer.start();
        playButton.setButtonText ("Stop");
    }
}

void MainComponent::sliderValueChanged(juce::Slider *slider)
{
    sequencer.setBPM (slider->getValue());
}
