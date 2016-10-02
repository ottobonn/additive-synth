#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include "../JuceLibraryCode/JuceHeader.h"
#include "Sine.h"

/**
 * Defines a generic oscillator interface for use with GUI sliders.
 * Each oscillator contains a Sine object, but the type of wave is not
 * coupled to the interface, so adding more wave types is easy.
 */

class Oscillator :
  public Slider::Listener
{

public:

  Oscillator (Slider *offsetSlider, Slider *gainSlider):
    offsetSlider(offsetSlider),
    gainSlider(gainSlider),
    gain(0.f),
    baseFrequency(0.f),
    offsetSemitones(0.f)
  {
    offsetSlider->addListener(this);
    gainSlider->addListener(this);
  }

  void sliderValueChanged (Slider* slider) override {
    float value = slider->getValue();
    if (slider == offsetSlider){
      this->setOffsetSemitones(value);
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

  void setBaseFrequency (float frequency) {
    this->baseFrequency = frequency;
    this->updateWaveFrequency();
  }

  void setOffsetSemitones (float semitones) {
    this->offsetSemitones = semitones;
    this->updateWaveFrequency();
  }

  double tick (){
    return (this->sine).tick() * this->gain;
  }

private:
  Sine sine;
  float gain;
  float baseFrequency, offsetSemitones;

  Slider *gainSlider, *offsetSlider;

  void updateWaveFrequency () {
    (this->sine).setFrequency(this->baseFrequency * pow(2, this->offsetSemitones / 12));
  }

};

#endif
