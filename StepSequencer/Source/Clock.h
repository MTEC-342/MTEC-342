/*
  ==============================================================================

    Clock.h
    Created: 3 Nov 2021 10:49:59am
    Author:  Akito van Troyer

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class Clock : private juce::Thread {
public:
    
    Clock():Thread ("ClockThread"), time (-1), elapsedTime (0) {}
    ~Clock(){
        stopThread (500);
    }
    
    void run() override;
    void start();
    void stop();
    
    void setBPM (const float bpm){
        this->interval = 6000.0f / bpm * 4;
    }
        
    class Listener
    {
    public:
        virtual ~Listener() {}
        
        // Called when the counter value changes.
        virtual void tick (int count) = 0;
        
        // Resets the clock
        virtual void reset () = 0;
    };

    void setListener (Listener* listener)
    {
        this->listener = listener;
    }
    
    void setStep (const int count)
    {
        step.set (count);
    }
    
    int getStep()
    {
        return step.get();
    }
    
private:
    juce::Atomic<int> interval = 6000.0f / 120 * 4;
    juce::Atomic<int> count;
    juce::Atomic<int> step = 16;
    juce::uint32 time;
    juce::uint32 elapsedTime;
    bool isRunning = false;
    
    Listener* listener;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Clock)
};
