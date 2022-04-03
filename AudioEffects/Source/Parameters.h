/*
  ==============================================================================

    Parameters.h
    Created: 12 Aug 2021 5:53:08pm
    Author:  Akito van Troyer

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

namespace Parameters {

std::vector<juce::String> effects {"Distortion", "LPF", "Gain", "ADSR", "Reverb"};

enum Effects { distortionIndex, lpfIndex, gainIndex, adsrIndex, reverbIndex };

// Distortion
float curClip = 0.1f;
enum Distortion { clipIndex };
std::vector<juce::String> distNames {"Clip"};
std::vector<float> distMins {0.1f};
std::vector<float> distMaxs {0.9f};
std::vector<float> distSteps {0.01f};
std::vector<float> distDefaults {0.1f};

// LPF
enum LPF { cutoffIndex, resonanceIndex };
std::vector<juce::String> lpfNames {"Cutoff", "Res"};
std::vector<float> lpfMins {20.0f, 0.0f};
std::vector<float> lpfMaxs {5000.0f, 1.0f};
std::vector<float> lpfSteps {0.1f, 0.01f};
std::vector<float> lpfDefaults {2000.0f, 0.7f};

// Gain
enum Gain { levelIndex };
std::vector<juce::String> gainNames {"Gain"};
std::vector<float> gainMins {0.0f};
std::vector<float> gainMaxs {1.0f};
std::vector<float> gainSteps {0.01f};
std::vector<float> gainDefaults {0.5f};

// ADSR
enum ADSR { attackIndex, decayIndex, sustainIndex, releaseIndex };
std::vector<juce::String> adsrNames {"A", "D", "S", "R"};
std::vector<float> adsrMins {0.01f, 0.01f, 0.0f, 0.01f};
std::vector<float> adsrMaxs {2.0f, 1.0f, 2.0f, 2.0f};
std::vector<float> adsrSteps {0.01f, 0.01f, 0.01f, 0.01f};
std::vector<float> adsrDefaults {0.1f, 0.1f, 0.75f, 0.4f};

// Reverb
enum Reverb { roomSizeIndex, mixIndex, widthIndex, dampIndex };
std::vector<juce::String> reverbNames {"Size", "Mix", "Width", "Damp"};
std::vector<float> reverbMins {0.0f, 0.0f, 0.0f, 0.0f};
std::vector<float> reverbMaxs {1.0f, 1.0f, 1.0f, 1.0f};
std::vector<float> reverbSteps {0.01f, 0.01f, 0.01f, 0.01f};
std::vector<float> reverbDefaults {0.5f, 0.5f, 1.0f, 0.5f};
}
