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
        setAudioChannels (0, 2);
    }

    // Change block size to minimize delay for tapping
    juce::AudioDeviceManager::AudioDeviceSetup setup = deviceManager.getAudioDeviceSetup();
    setup.bufferSize = 64;
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    createPads();
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    for (int i = 0; i < pads.size(); ++i)
    {
        pads[i]->getNextAudioBlock (bufferToFill);
    }
}

void MainComponent::releaseResources()
{
    for (int i = 0; i < pads.size(); ++i)
    {
        pads[i]->releaseResources();
    }
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    int numColumns = 3;
    auto buttonWidth = getWidth() / numColumns;

    for (int i = 0 ; i < pads.size(); ++i)
    {
        addAndMakeVisible (pads[i]);
        pads[i]->setBounds (5 + (i % numColumns) * buttonWidth, 105 + (i / numColumns) * buttonWidth, buttonWidth - 10, buttonWidth - 10);
    }
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}

void MainComponent::createPads()
{
#if JUCE_ANDROID
    juce::ZipFile apkZip (juce::File::getSpecialLocation (juce::File::invokedExecutableFile));
    auto numFiles = apkZip.getNumEntries();
    for (int i = 0; i < numFiles; ++i)
    {
        auto* entry = apkZip.getEntry (i);
        if (entry->filename.contains("assets/Sounds/"))
        {
            pads.add (new Pad (entry->filename));
        }
    }
#elif JUCE_IOS
    auto assetsDir = juce::File::getSpecialLocation (juce::File::currentExecutableFile)
                              .getParentDirectory().getChildFile ("Assets");
    auto soundDir = assetsDir.getChildFile ("Sounds");
    auto files = soundDir.findChildFiles(juce::File::TypesOfFileToFind::findFiles, false, "*.wav");
    for (int i = 0; i < files.size(); ++i)
    {
        pads.add (new Pad (files[i].getFullPathName()));
    }
#endif
}
