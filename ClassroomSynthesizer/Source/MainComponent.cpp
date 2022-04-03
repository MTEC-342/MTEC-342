#include "MainComponent.h"

// Use LOGD to print something in Android logcat
#if JUCE_ANDROID
#include <android/log.h>
#define  LOG_TAG "JUCE"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#endif

//==============================================================================
MainComponent::MainComponent()
    : keyboardComponent (keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
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

    juce::String buttonNames[] = {"SIN", "SAW", "SQR", "TRI"};

    for(int i = 0; i < 4; i++)
    {
        // Initialize Sliders
        sliders.add (new juce::Slider (juce::Slider::SliderStyle::Rotary, juce::Slider::NoTextBox));
        if (i != 2)
        { // ASR durations
            sliders[i]->setValue (0.1f);
            sliders[i]->setRange (0.01f, 2.0f, 0.01f);
        }
        else
        { // For sustain level
            sliders[i]->setValue (1.0f);
            sliders[i]->setRange (0.0f, 1.0f, 0.01f);
        }
        sliders[i]->addListener (this);

        // Initialize Buttons
        buttons.add (new juce::DrawableButton (buttonNames[i], juce::DrawableButton::ButtonStyle::ImageFitted));
        auto onImg = loadImage (buttonNames[i] + ".png");
        juce::DrawableImage on;
        on.setImage (onImg);

        auto offImg = loadImage (buttonNames[i] + "OFF.png");
        juce::DrawableImage off;
        off.setImage (offImg);

        buttons[i]->setImages (
                &on, &on, &on, &on, &off, &off, &off, &off
        );

        buttons[i]->setClickingTogglesState (true);
        buttons[i]->addListener (this);
    }

    // Turn sine wave on
    buttons[0]->setToggleState(true, juce::dontSendNotification);

    keyboardState.addListener (this);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
    keyboardState.removeListener (this);
    for (int i = 0; i < 4; i++)
    {
        sliders[i]->removeListener (this);
        buttons[i]->removeListener (this);
    }
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

    // Initialize ADSR
    adsr.setSampleRate(sampleRate);
    adsr.reset();
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    juce::AudioBuffer<float> buffer = *bufferToFill.buffer;
    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::ProcessContextReplacing<float> context (block);
    processorChain.process (context);

    // Apply ADSR
    adsr.applyEnvelopeToBuffer (buffer, 0, buffer.getNumSamples());
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

    int padding = 10;
    juce::Rectangle<int> sliderArea { padding, padding, getWidth() / 2, getHeight() / 2 - padding * 2 };
    juce::Rectangle<int> buttonArea {getWidth() / 2 + padding, getHeight() / 4 - getWidth() / 16, getWidth() / 2, getWidth() / 8 };

    for (int i = 0; i < 4; ++i) {
        addAndMakeVisible(sliders[i]);
        sliders[i]->setBounds(sliderArea.removeFromLeft(getWidth() / 8));
        sliders[i]->setPopupDisplayEnabled (true, false, this);
        sliders[i]->setTextValueSuffix (" sec");

        addAndMakeVisible(buttons[i]);
        buttons[i]->setBounds(buttonArea.removeFromLeft(getWidth() / 8 - padding));
        buttonArea.removeFromLeft(padding);
        buttons[i]->setColour (juce::DrawableButton::backgroundColourId,   juce::Colours::transparentBlack);
        buttons[i]->setColour (juce::DrawableButton::backgroundOnColourId,   juce::Colours::transparentBlack);
    }

    addAndMakeVisible (keyboardComponent);
    keyboardComponent.setKeyWidth (getWidth() / 14); // Get 14 white keys to display
    keyboardComponent.setBounds (0,  getHeight() / 2, getWidth(), getHeight() / 2);
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}

//==============================================================================
void MainComponent::handleNoteOn (juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity)
{
    processorChain.get<oscIndex>().setFrequency (juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber));
    processorChain.get<gainIndex>().setGainLinear (velocity);
    adsr.noteOn();
}

void MainComponent::handleNoteOff (juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity)
{
    adsr.noteOff();
}

juce::Image MainComponent::loadImage (juce::String fileName)
{
    std::unique_ptr<juce::InputStream> inputStream;
    #if JUCE_ANDROID
        juce::ZipFile apkZip (juce::File::getSpecialLocation (juce::File::invokedExecutableFile));
        inputStream = std::unique_ptr<juce::InputStream> (apkZip.createStreamForEntry (apkZip.getIndexOfFileName ("assets/Images/" + fileName)));
    #elif JUCE_IOS
        auto assetsDir = juce::File::getSpecialLocation (juce::File::currentExecutableFile)
                          .getParentDirectory().getChildFile ("Assets");
        auto imageDir = assetsDir.getChildFile ("Images");
        auto imageFile = imageDir.getChildFile (fileName);
        
        jassert (imageFile.existsAsFile());

        inputStream = imageFile.createInputStream();
    #endif

    return juce::ImageFileFormat::loadFrom (*inputStream);
}

void MainComponent::sliderValueChanged (juce::Slider *slider)
{
    auto parameters = adsr.getParameters();
    auto parameter = slider->getValue();

    if (slider == sliders[0])
        parameters.attack = parameter;
    else if (slider == sliders[1])
        parameters.decay = parameter;
    else if (slider == sliders[2])
        parameters.sustain = parameter;
    else
        parameters.release = parameter;

    adsr.setParameters (parameters);
}

void MainComponent::buttonClicked (juce::Button* button)
{
    // Turn all buttons off
    for (int i = 0; i < 4; ++i)
    {
        buttons[i]->setToggleState (false, juce::dontSendNotification);
    }

    // Generate waveform and turn on the button based on the user's choice
    if (button == buttons[0])
    {
        generateSine();
        buttons[0]->setToggleState (true, juce::dontSendNotification);
    }
    else if (button == buttons[1])
    {
        generateSawtooth();
        buttons[1]->setToggleState (true, juce::dontSendNotification);
    }
    else if (button == buttons[2])
    {
        generateSquare();
        buttons[2]->setToggleState (true, juce::dontSendNotification);
    }
    else
    {
        generateTriangle();
        buttons[3]->setToggleState (true, juce::dontSendNotification);
    }
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
