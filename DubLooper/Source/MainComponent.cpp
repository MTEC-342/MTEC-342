#include "MainComponent.h"

// Use LOGD to print something in Android logcat
#if JUCE_ANDROID
#include <android/log.h>
#define  LOG_TAG "JUCE"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#endif

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
        setAudioChannels (1, 2);
    }

    // Change block size to minimize delay for tapping
    juce::AudioDeviceManager::AudioDeviceSetup setup = deviceManager.getAudioDeviceSetup();
    setup.bufferSize = 64;

    playButton.setButtonText ("Play");
    playButton.addListener (this);

    recordButton.setButtonText ("Record");
    recordButton.addListener (this);

    sampleBuffer.setListener (this);
    sampleBuffer.enableLoop (true);
    metronome.setListener (this);

    saveButton.setButtonText ("Save");
    saveButton.onClick = [this] { sampleBuffer.write ("DubLooper.wav"); };

    // read audio file if it exists
    sampleBuffer.read ("DubLooper.wav");

    // Initialize the thumbnail
    formatManager.registerBasicFormats();
    thumbnail.addChangeListener (this);
    thumbnail.reset(1, 44100, sampleBuffer.getNumSamples());
    thumbnail.addBlock(0, sampleBuffer.getBuffer(), 0, sampleBuffer.getNumSamples());
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    metronome.prepareToPlay (sampleRate);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Clear buffer when recording is not happening to avoid feedback
    if (!sampleBuffer.getIsRecording())
    {
        bufferToFill.clearActiveBufferRegion();
    }

    // Wait for five ticks since record button is pressed.
    if(!sampleBuffer.getIsRecording() && metronome.getCurrentBeat() >= 5)
    {
        sampleBuffer.record(2, metronome.getInterval() * 16);
    }
    // Recording and Playback
    sampleBuffer.getNextAudioBlock (bufferToFill);

    // Clear buffer when recording is happening to avoid feedback
    if (sampleBuffer.getIsRecording())
    {
        bufferToFill.clearActiveBufferRegion();
    }

    // Playback
    metronome.tick (bufferToFill.numSamples);
    metronome.getNextAudioBlock (bufferToFill);
}

void MainComponent::releaseResources()
{
    metronome.releaseResources();
    sampleBuffer.releaseResources();
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    addAndMakeVisible (playButton);
    playButton.setBounds (10, getHeight() - 100, 100, 50);
    addAndMakeVisible (recordButton);
    recordButton.setBounds (getWidth() / 2 - 50, getHeight() - 100, 100, 50);
    addAndMakeVisible (saveButton);
    saveButton.setBounds (getWidth() - 110, getHeight() - 100, 100, 50);

    addAndMakeVisible (countLabel);
    countLabel.setBounds (getWidth() / 2 - 50, getHeight() - 150, 100, 50);
    countLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    countLabel.setFont (juce::Font (48.0f, juce::Font::bold));
    countLabel.setJustificationType (juce::Justification::centred);

    if (metronome.getIsTicking())
    {
        countLabel.setText (juce::String (std::abs (4 - metronome.getCurrentBeat())), juce::NotificationType::dontSendNotification);
    }
    else
    {
        countLabel.setText ("", juce::NotificationType::dontSendNotification);
    }

    // Draw thumbnail
    g.setColour (juce::Colours::white);
    g.fillRect (getLocalBounds().reduced (0, 200));
    g.setColour (juce::Colours::grey);
    thumbnail.drawChannels (g, getLocalBounds().reduced (0, 200), 0, thumbnail.getTotalLength(), 1.0f);
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}

void MainComponent::buttonClicked (juce::Button* button)
{
    if (button == &playButton){
        if (sampleBuffer.getIsPlaying())
        {
            sampleBuffer.stopPlaying();
            playButton.setButtonText("Play");
        }
        else
        {
            sampleBuffer.play();
            playButton.setButtonText("Stop");
        }
    }
    else if (button == &recordButton)
    {
        if (sampleBuffer.getIsRecording())
        {
            metronome.stop();
            sampleBuffer.stopRecording();
            recordButton.setButtonText("Record");
            playButton.setEnabled (true);
        }
        else
        {
            metronome.start();
            recordButton.setButtonText("Stop");
            playButton.setEnabled (false);
            if (sampleBuffer.getIsPlaying())
            {
                sampleBuffer.stopPlaying();
                playButton.setButtonText("Play");
            }
        }
    }
}

void MainComponent::playingEnded()
{
    const juce::MessageManagerLock mmLock;
    playButton.setButtonText("Play");
}

void MainComponent::recordingEnded()
{
    metronome.stop();

    const juce::MessageManagerLock mmLock;
    recordButton.setButtonText("Record");
    playButton.setEnabled (true);

    // Update thumbnail
    thumbnail.reset(1, 44100, sampleBuffer.getNumSamples());
    thumbnail.addBlock(0, sampleBuffer.getBuffer(), 0, sampleBuffer.getNumSamples());
}

void MainComponent::ticked()
{
    const juce::MessageManagerLock mmLock;
    repaint();
}

void MainComponent::changeListenerCallback (juce::ChangeBroadcaster* source)
{
    repaint();
}
