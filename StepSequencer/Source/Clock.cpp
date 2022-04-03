/*
  ==============================================================================

    Clock.cpp
    Created: 3 Nov 2021 10:49:59am
    Author:  Akito van Troyer

  ==============================================================================
*/

#include "Clock.h"

void Clock::run(){
    while (!threadShouldExit()) {
        elapsedTime = juce::Time::getMillisecondCounter();
        
        if(elapsedTime - time >= interval.get()) {
            count.set ((count.get() + 1) % step.get());
            
            if (listener != nullptr)
            {
                listener->tick (count.get());
            }

            time = elapsedTime;
        }
        
        wait(1);
    }
}

void Clock::start()
{
    if (!isRunning)
    {
        startThread (8);
        isRunning = true;
    }
}

void Clock::stop()
{
    if(isRunning)
    {
        stopThread (500);
        isRunning = false;
        count.set (-1);
        if (listener != nullptr)
        {
            listener->reset();
        }
    }
}
