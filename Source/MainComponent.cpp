// Music 256a / CS 476a | fall 2016
// CCRMA, Stanford University
// Author: Travis Geis
// Starter Code: Romain Michon (rmichonATccrmaDOTstanfordDOTedu)
// Description: Simple JUCE additive synthesizer

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include <vector>

#include "../JuceLibraryCode/JuceHeader.h"
#include "Sine.h"
#include "Oscillator.h"

class MainContentComponent :
    public AudioAppComponent,
    public Slider::Listener,
    private ToggleButton::Listener
{
public:
    MainContentComponent() : gain (0.0), onOff (0), samplingRate(0.0)
    {
        numOsc = 4;

        for (int i = 0; i < numOsc; i++) {
          auto freqSlider = addSlider(String("Frequency ") + String(i + 1));
          freqSlider->setRange(50.f, 5000.f);
          freqSlider->setSkewFactorFromMidPoint(500.f);
          freqSlider->setValue(1000.f);

          auto gainSlider = addSlider(String("Gain ") + String(i + 1));
          gainSlider->setRange (0.0, 1.0);
          gainSlider->setValue(0);

          Oscillator *osc = new Oscillator(freqSlider, gainSlider);
          oscillators.push_back(osc);
        }

        // configuring gain slider and adding it to the main window
        gainSlider = addSlider(String("Master Gain"));
        gainSlider->setRange (0.0, 1.0);
        gainSlider->setValue(0.5); // will alsi set the default gain of the sine osc
        gainSlider->addListener (this);

       // configuring on/off button and adding it to the main window
        onOffButton = new ToggleButton();
        onOffButton->addListener(this);
        addAndMakeVisible(onOffButton);

        // configuring on/off label and adding it to the main window
        Label *onOffLabel = new Label();
        addAndMakeVisible(onOffLabel);
        onOffLabel->setText ("On/Off", dontSendNotification);
        onOffLabel->attachToComponent (onOffButton, true);

        setSize (600, 500);
        nChans = 1;
        setAudioChannels (0, nChans); // no inputs, one output
    }

    ~MainContentComponent()
    {
        shutdownAudio();
    }

    void resized() override
    {
      // Each pair of sliders takes up 3 slider heights due to the space between pairs
      // The extra two slider heights are the master gain and the on-off button
      int sliderHeight = getHeight() / (numOsc * 3 + 2);
      int currentHeight = 0;

      int leftMargin = 100;
      int rightMargin = 20;

      int contentWidth = getWidth() - leftMargin - rightMargin;

      bool isEven = false;
      for(std::vector<Slider*>::iterator iter = sliders.begin(); iter != sliders.end(); iter++) {
        auto slider = *iter;
        slider->setBounds(leftMargin, currentHeight, contentWidth, sliderHeight);
        currentHeight += sliderHeight;
        // Add space after slider pair
        if (isEven) {
          currentHeight += sliderHeight;
        }
        isEven = !isEven;
      }
      onOffButton->setBounds(leftMargin, currentHeight, contentWidth, 20);
    }

    void sliderValueChanged (Slider* slider) override {
      if (slider == gainSlider) {
        this->gain = slider->getValue();
      }
    }

    void buttonClicked (Button* button) override
    {
      // turns audio on or off
      if (button == onOffButton) {
        onOff = onOffButton->getToggleState();
      }
    }

    void prepareToPlay (int /*samplesPerBlockExpected*/, double sampleRate) override
    {
        samplingRate = sampleRate;
        for(auto iter = oscillators.begin(); iter != oscillators.end(); iter++) {
          (*iter)->setSamplingRate(sampleRate);
        }
    }

    void releaseResources() override
    {
      // Dispose of oscillators
      for(auto iter = oscillators.begin(); iter != oscillators.end(); iter++) {
        delete *iter;
      }
      // Free all child components
      deleteAllChildren();
    }

    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
      // getting the audio output buffer to be filled
      float* const buffer = bufferToFill.buffer->getWritePointer (0, bufferToFill.startSample);

      // computing one block
      for (int index = 0; index < bufferToFill.numSamples; ++index)
      {
        float sample = 0;
        for (auto iter = oscillators.begin(); iter != oscillators.end(); iter++)
        {
          Oscillator *osc = *iter;
          sample += (1.f / this->numOsc) * osc->tick();
        }
        sample *= gain;
        if (onOff) {
          buffer[index] = sample;
        } else {
          buffer[index] = 0;
        }
      }
    }


private:

    Slider * addSlider (String labelText) {
      Slider *slider = new Slider();
      addAndMakeVisible(slider);
      sliders.push_back(slider);

      Label *label = new Label();
      label->setText(labelText, dontSendNotification);
      label->attachToComponent(slider, true);
      addAndMakeVisible(label);
      labels.push_back(label);

      return slider;
    }

    // UI Elements
    std::vector<Slider*> sliders;
    std::vector<Label*> labels;
    std::vector<Oscillator*> oscillators;

    Slider *gainSlider;
    ToggleButton *onOffButton;

    float gain;

    // Global Variables
    int onOff, samplingRate, nChans, numOsc;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};

Component* createMainContentComponent()     { return new MainContentComponent(); }


#endif  // MAINCOMPONENT_H_INCLUDED