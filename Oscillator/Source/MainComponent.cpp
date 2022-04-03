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
    else {
        // Specify the number of input and output channels that we want to open
        setAudioChannels(0, 2);
    }

    freqSlider.setRange (20.0f, 6000.0f, 1.0f);
    freqSlider.setValue (440);

    gainSlider.setRange (0.0f, 1.0f, 0.01f);
    gainSlider.setValue (0.5f);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlockExpected;
    spec.numChannels = 2;
    processorChain.prepare (spec);

    auto& osc = processorChain.template get<oscIndex>();
    osc.initialise ([] (float x)
                    {
                        return std::sin (x);
                    }, 128);

    processorChain.get<oscIndex>().setFrequency (440, true);
    processorChain.get<gainIndex>().setGainLinear (0.5f);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    juce::dsp::AudioBlock<float> block (*(bufferToFill.buffer));
    juce::dsp::ProcessContextReplacing<float> context (block);
    processorChain.process (context);
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    addAndMakeVisible (freqLabel);
    freqLabel.setText ("Frequency:", juce::dontSendNotification);
    freqLabel.attachToComponent (&freqSlider, true);
    freqLabel.setJustificationType (juce::Justification::right);

    addAndMakeVisible (gainLabel);
    gainLabel.setText ("Gain:", juce::dontSendNotification);
    gainLabel.attachToComponent (&gainSlider, true);
    gainLabel.setJustificationType (juce::Justification::right);

    freqSlider.setTextBoxStyle (juce::Slider::TextBoxRight, true, 80, 20);
    freqSlider.setBounds (100, getHeight() / 2, getWidth() - 120, 20);
    freqSlider.setTextValueSuffix(" Hz");

    gainSlider.setTextBoxStyle (juce::Slider::TextBoxRight, true, 80, 20);
    gainSlider.setBounds (100, getHeight() / 2 + 30, getWidth() - 120, 20);

    addAndMakeVisible (freqSlider);
    addAndMakeVisible (gainSlider);

    freqSlider.addListener (this);
    gainSlider.addListener (this);
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}

void MainComponent::sliderValueChanged (juce::Slider *slider)
{
    if (slider == &freqSlider)
        processorChain.get<oscIndex>().setFrequency (slider->getValue());
    else
        processorChain.get<gainIndex>().setGainLinear (slider->getValue());
}