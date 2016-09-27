#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include "../JuceLibraryCode/JuceHeader.h"
#include "Sine.h"

class Oscillator :
  public Slider::Listener
{

public:

  Oscillator (Slider *frequencySlider, Slider *gainSlider):
    frequencySlider(frequencySlider), gainSlider(gainSlider), gain(0.f)
  {
    frequencySlider->addListener(this);
    gainSlider->addListener(this);
  }

  void sliderValueChanged (Slider* slider) override {
    float value = slider->getValue();
    if (slider == frequencySlider){
      (this->sine).setFrequency(value);
    } else if (slider == gainSlider){
      this->gain = value;
    }
  }

  void setSamplingRate (int samplingRate) {
    (this->sine).setSamplingRate(samplingRate);
  }

  int getSamplingRate () {
    return (this->sine).getSamplingRate();
  }

  void setFrequency (float frequency) {
    (this->sine).setFrequency(frequency);
  }

  double tick (){
    return (this->sine).tick() * this->gain;
  }

private:
  Sine sine;
  float gain;

  Slider *gainSlider, *frequencySlider;

};

#endif
