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
    
    sliders.add (new ParameterSlider ("Frequency", 440.0f, 4400.0f, 0.01f, 440.0f));
    sliders.add (new ParameterSlider ("Tremolo Rate", 0.0f, 10.0f, 0.01f, 2.0f));
    sliders.add (new ParameterSlider ("Vibrato Rate", 0.0f, 10.0f, 0.01f, 2.0f));
    sliders.add (new ParameterSlider ("Vibrato Depth", 0.0f, 1.0f, 0.01f, 0.1f));
    sliders.add (new ParameterSlider ("Panning Rate", 0.0f, 10.0f, 0.01f, 0.1f));
    sliders.add (new ParameterSlider ("Panning Depth", 0.0f, 1.0f, 0.01f, 0.1f));
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
    
    // Set oscillator parameters
    auto& osc = processorChain.template get<oscIndex>();
    osc.initialise ([] (float x) { return std::sin (x); }, 128);
    osc.setFrequency (440);
    
    // Set panning parameters
    auto& pan = processorChain.template get<panIndex>();
    pan.setRule(juce::dsp::PannerRule::sin3dB);
    
    // Tremolo prep
    tlfo.initialise ([] (float x) { return std::sin(x); }, 128);
    tlfo.setFrequency (2.1f);
    tlfo.prepare ({ spec.sampleRate / lfoUpdateRate, spec.maximumBlockSize, spec.numChannels });
    
    // Vibrato prep
    vlfo.initialise ([] (float x) { return std::sin(x); }, 128);
    vlfo.setFrequency (4.0f);
    vlfo.prepare ({ spec.sampleRate / lfoUpdateRate, spec.maximumBlockSize, spec.numChannels });
    
    // Panning prep
    plfo.initialise ([] (float x) { return std::sin(x); }, 128);
    plfo.setFrequency (1.0f);
    plfo.prepare ({ spec.sampleRate / lfoUpdateRate, spec.maximumBlockSize, spec.numChannels });
    
    // Prepare the temporary audio block
    tempBlock = juce::dsp::AudioBlock<float> (heapBlock, spec.numChannels, spec.maximumBlockSize);
    
    // Intialize the sliders
    for(int i = 0; i < sliders.size(); ++i){
        sliders[i]->reset(sampleRate, 1e-3);
    }
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Prepare the temporary audio buffer
    auto numSamples = bufferToFill.numSamples;
    auto output = tempBlock.getSubBlock (0, (size_t) numSamples);
    output.clear();
    
    for (size_t pos = 0; pos < (size_t) numSamples;)
    {
        // Keep track of the sub block size
        auto max = juce::jmin ((size_t) numSamples - pos, lfoUpdateCounter);
        auto block = output.getSubBlock (pos, max);

        // Process samples
        juce::dsp::ProcessContextReplacing<float> context (block);
        processorChain.process (context);

        // Update sample position and lfo counter
        pos += max;
        lfoUpdateCounter -= max;

        if (lfoUpdateCounter <= 0) // Apply lfos when the counter is 0 or less.
        {
            lfoUpdateCounter = lfoUpdateRate;
            
            // Set lfo parameters
            tlfo.setFrequency (sliders[1]->getNextValue());
            vlfo.setFrequency (sliders[2]->getNextValue());
            plfo.setFrequency (sliders[4]->getNextValue());
            
            auto gain = juce::jmap (tlfo.processSample (0.0f), -1.0f, 1.0f, 0.2f, 1.0f);
            auto vdepth = sliders[3]->getNextValue();
            auto pdepth = sliders[5]->getNextValue();
            auto freq = sliders[0]->getNextValue() * juce::jmap (vlfo.processSample (0.0f), -1.0f, 1.0f, 1.0f - vdepth, 1.0f + vdepth);
            auto pan = juce::jmap (plfo.processSample (0.0f), -1.0f, 1.0f, -pdepth, pdepth);
            processorChain.get<gainIndex>().setGainLinear (gain);
            processorChain.get<oscIndex>().setFrequency (freq);
            processorChain.get<panIndex>().setPan (pan);
        }
    }

    // Apply processor chain to the main I/O buffer
    juce::dsp::AudioBlock<float> (*bufferToFill.buffer)
        .getSubBlock ((size_t) bufferToFill.startSample, (size_t) numSamples)
        .add (tempBlock);
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

    auto width = getWidth() / 2;
    auto height = (getHeight() - 100) / 3;
    
    for (int i = 0; i < sliders.size(); ++i)
    {
        addAndMakeVisible (sliders[i]);
        if (i % 2 == 0)
            sliders[i]->setBounds (0, 50 + i * height / 2, width, height);
        else
            sliders[i]->setBounds (getWidth() / 2, 50 + (i-1) * height / 2, width, height);
    }
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}
