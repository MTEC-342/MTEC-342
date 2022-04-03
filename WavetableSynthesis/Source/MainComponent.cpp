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

    generateSine();

    processorChain.get<oscIndex>().setFrequency (440, true);
    processorChain.get<gainIndex>().setGainLinear (0.5f);
    
    oscilloscope.setNumChannels(2);
    oscilloscope.setSamplesPerBlock(2);
    oscilloscope.setBufferSize (128);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    juce::dsp::AudioBlock<float> block (*(bufferToFill.buffer));
    juce::dsp::ProcessContextReplacing<float> context (block);
    processorChain.process (context);
    
    oscilloscope.pushBuffer (*(bufferToFill.buffer));
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
    
    int numButtons = 4;
    int edgeSpace = 20;
    int spacing = 5;
    
    juce::Rectangle<int> layoutArea { edgeSpace, getHeight() / 2 - 50, getWidth() - edgeSpace, 25 };
    auto buttonWidth = (getWidth() - (edgeSpace * 2 + spacing * (numButtons - 1))) / numButtons;
    
    sineButton.setBounds (layoutArea.removeFromLeft (buttonWidth));
    layoutArea.removeFromLeft(spacing);
    sawtoothButton.setBounds (layoutArea.removeFromLeft (buttonWidth));
    layoutArea.removeFromLeft(spacing);
    squareButton.setBounds (layoutArea.removeFromLeft (buttonWidth));
    layoutArea.removeFromLeft(spacing);
    triangleButton.setBounds (layoutArea.removeFromLeft (buttonWidth));
    
    addAndMakeVisible (sineButton);
    addAndMakeVisible (sawtoothButton);
    addAndMakeVisible (squareButton);
    addAndMakeVisible (triangleButton);
    
    sineButton.setButtonText ("Sine");
    sawtoothButton.setButtonText ("Sawtooth");
    squareButton.setButtonText ("Square");
    triangleButton.setButtonText ("Triangle");
    
    sineButton.addListener(this);
    sawtoothButton.addListener(this);
    squareButton.addListener(this);
    triangleButton.addListener(this);
    
    addAndMakeVisible (oscilloscope);
    oscilloscope.setBounds (0, getHeight() / 2, getWidth(), getHeight() / 2);
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}

void MainComponent::generateSine()
{
    auto& osc = processorChain.template get<oscIndex>();
    osc.initialise ([] (float x)
    {
        return std::sin (x);
    }, 128);
}

void MainComponent::generateSawtooth()
{
    auto& osc = processorChain.template get<oscIndex>();
    osc.initialise ([] (float x)
    {
        return juce::jmap (x,
                           float (-juce::MathConstants<float>::pi),
                           float (juce::MathConstants<float>::pi),
                           float (-1),
                           float (1));
    }, 2);
}

void MainComponent::generateSquare()
{
    auto& osc = processorChain.template get<oscIndex>();
    osc.initialise ([] (float x)
    {
        auto val = std::sin (x);
        return (0.0f < val) - (val < 0.0f);
    }, 128);
}

void MainComponent::generateTriangle()
{
    auto& osc = processorChain.template get<oscIndex>();
    osc.initialise ([] (float x)
    {
        auto val = juce::jmap (x,
                               float (-juce::MathConstants<float>::pi),
                               float (juce::MathConstants<float>::pi),
                               float (0),
                               float (1));
        return 2.0f * std::fabs (2.0f * (val - std::floor (val + 0.5f))) - 1.0f;
    }, 128);
}

void MainComponent::buttonClicked (juce::Button* button)
{
    if (button == &sineButton)
        generateSine();
    else if (button == &sawtoothButton)
        generateSawtooth();
    else if (button == &squareButton)
        generateSquare();
    else
        generateTriangle();
}
