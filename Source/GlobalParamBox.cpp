/*
  ==============================================================================

    GlobalParamBox.cpp
    Created: 30 Jul 2021 2:50:47pm
    Author:  brady

  ==============================================================================
*/

#include "GlobalParamBox.h"

#include <JuceHeader.h>

//==============================================================================
GlobalParamBox::GlobalParamBox(ParamGlobal& paramGlobal)
    : mParamGlobal(paramGlobal) {
  juce::Colour mainColour = juce::Colours::white;
  /* Amp envelope viz */
  mEnvelopeAmp.setActive(true);
  mEnvelopeAmp.setColour(mainColour);
  addAndMakeVisible(mEnvelopeAmp);

  /* Knob params */
  auto rotaryParams = juce::Slider::RotaryParameters();
  rotaryParams.startAngleRadians = 1.4f * juce::MathConstants<float>::pi;
  rotaryParams.endAngleRadians = 2.6f * juce::MathConstants<float>::pi;
  rotaryParams.stopAtEnd = true;

  /* Gain */
  mSliderGain = std::make_unique<
      Utils::AttachedComponent<juce::Slider, juce::SliderParameterAttachment> >(
      *mParamGlobal.gain, *this,
      [mainColour, rotaryParams](juce::Slider& slider) {
        slider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId,
                         mainColour);
        slider.setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId,
                         mainColour);
        slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
        slider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
        slider.setRotaryParameters(rotaryParams);
        slider.setRange(0.0, 1.0, 0.01);
      });
  mSliderGain->component.onValueChange = [this] {
    mEnvelopeAmp.setGain(mSliderGain->component.getValue());
  };
  mLabelGain.setText("Gain", juce::dontSendNotification);
  mLabelGain.setJustificationType(juce::Justification::centredTop);
  addAndMakeVisible(mLabelGain);

  /* Attack */
  mSliderAttack = std::make_unique<
      Utils::AttachedComponent<juce::Slider, juce::SliderParameterAttachment> >(
      *mParamGlobal.attack, *this,
      [mainColour, rotaryParams](juce::Slider& slider) {
        slider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId,
                         mainColour);
        slider.setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId,
                         mainColour);
        slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
        slider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
        slider.setRotaryParameters(rotaryParams);
        slider.setRange(ParamRanges::ATTACK.start, ParamRanges::ATTACK.end,
                        0.01);
        slider.setTextValueSuffix("s");
      });
  mSliderAttack->component.onValueChange = [this] {
    mEnvelopeAmp.setAttack(ParamRanges::ATTACK.convertTo0to1(mSliderAttack->component.getValue()));
  };
  mLabelAttack.setText("Attack", juce::dontSendNotification);
  mLabelAttack.setJustificationType(juce::Justification::centredTop);
  addAndMakeVisible(mLabelAttack);

  /* Decay */
  mSliderDecay = std::make_unique<
      Utils::AttachedComponent<juce::Slider, juce::SliderParameterAttachment> >(
      *mParamGlobal.decay, *this,
      [mainColour, rotaryParams](juce::Slider& slider) {
        slider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId,
                         mainColour);
        slider.setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId,
                         mainColour);
        slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
        slider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
        slider.setRotaryParameters(rotaryParams);
        slider.setRange(ParamRanges::DECAY.start, ParamRanges::DECAY.end, 0.01);
        slider.setTextValueSuffix("s");
      });
  mSliderDecay->component.onValueChange = [this] {
    mEnvelopeAmp.setDecay(ParamRanges::DECAY.convertTo0to1(mSliderDecay->component.getValue()));
  };
  mLabelDecay.setText("Decay", juce::dontSendNotification);
  mLabelDecay.setJustificationType(juce::Justification::centredTop);
  addAndMakeVisible(mLabelDecay);

  /* Sustain */
  mSliderSustain = std::make_unique<
      Utils::AttachedComponent<juce::Slider, juce::SliderParameterAttachment> >(
      *mParamGlobal.sustain, *this,
      [mainColour, rotaryParams](juce::Slider& slider) {
        slider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId,
                         mainColour);
        slider.setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId,
                         mainColour);
        slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
        slider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
        slider.setRotaryParameters(rotaryParams);
        slider.setRange(0.0, 1.0, 0.01);
      });
  mSliderSustain->component.onValueChange = [this] {
    mEnvelopeAmp.setSustain(mSliderSustain->component.getValue());
  };
  mLabelSustain.setText("Sustain", juce::dontSendNotification);
  mLabelSustain.setJustificationType(juce::Justification::centredTop);
  addAndMakeVisible(mLabelSustain);

  /* Release */
  mSliderRelease = std::make_unique<
      Utils::AttachedComponent<juce::Slider, juce::SliderParameterAttachment> >(
      *mParamGlobal.release, *this,
      [mainColour, rotaryParams](juce::Slider& slider) {
        slider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId,
                         mainColour);
        slider.setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId,
                         mainColour);
        slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
        slider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
        slider.setRotaryParameters(rotaryParams);
        slider.setRange(ParamRanges::RELEASE.start, ParamRanges::RELEASE.end,
                        0.01);
        slider.setTextValueSuffix("s");
      });
  mSliderRelease->component.onValueChange = [this] {
    mEnvelopeAmp.setRelease(ParamRanges::RELEASE.convertTo0to1(mSliderRelease->component.getValue()));
  };
  mLabelRelease.setText("Release", juce::dontSendNotification);
  mLabelRelease.setJustificationType(juce::Justification::centredTop);
  addAndMakeVisible(mLabelRelease);
}

GlobalParamBox::~GlobalParamBox() {}

void GlobalParamBox::paint(juce::Graphics& g) {
  g.fillAll(juce::Colours::black);

  juce::Colour mainColour = juce::Colours::white;

  // Global param title
  juce::Rectangle<float> titleRect =
      juce::Rectangle<float>(0.0f, 0.0f, getWidth(), MAIN_TITLE_HEIGHT)
          .reduced(PADDING_SIZE, PADDING_SIZE);
  g.setColour(mainColour);
  g.fillRoundedRectangle(titleRect, 10.0f);

  g.setColour(juce::Colours::black);
  g.drawText(juce::String(MAIN_TITLE), titleRect, juce::Justification::centred);

  // Amp env section title
  juce::Rectangle<float> ampEnvTitleRect =
      juce::Rectangle<float>(
          0.0f,
          mEnvelopeAmp.getY() - SECTION_TITLE_HEIGHT - (PADDING_SIZE / 2.0f),
          getWidth(), SECTION_TITLE_HEIGHT)
          .reduced(PADDING_SIZE, PADDING_SIZE / 2.0f);
  g.setColour(mainColour);
  g.fillRoundedRectangle(ampEnvTitleRect, 10.0f);
  g.setColour(juce::Colours::black);
  g.drawText(juce::String(SECTION_AMP_ENV_TITLE), ampEnvTitleRect,
             juce::Justification::centred);

  // Outline rect
  g.setColour(mainColour);
  g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(1.0f), 10.0f, 2.0f);
}

void GlobalParamBox::resized() {
  auto r = getLocalBounds();
  // Add insets
  r.removeFromTop(PADDING_SIZE);
  r.removeFromLeft(PADDING_SIZE);
  r.removeFromRight(PADDING_SIZE);
  r.removeFromBottom(PADDING_SIZE);

  r.removeFromTop(MAIN_TITLE_HEIGHT);

  // Amp envelope
  r.removeFromTop(SECTION_TITLE_HEIGHT);
  auto knobWidth = r.getWidth() / NUM_AMP_ENV_PARAMS;
  auto ampEnvPanel = r.removeFromTop(ENVELOPE_HEIGHT);
  auto gainPanel = ampEnvPanel.removeFromRight(knobWidth);
  mLabelGain.setBounds(gainPanel.removeFromBottom(LABEL_HEIGHT));
  mSliderGain->component.setBounds(gainPanel.withSizeKeepingCentre(
      gainPanel.getWidth(), gainPanel.getWidth() / 2.0f));
  mEnvelopeAmp.setBounds(ampEnvPanel);
  r.removeFromTop(PADDING_SIZE);

  // Amp env knobs
  auto knobPanel = r.removeFromTop(knobWidth / 2);
  mSliderAttack->component.setBounds(knobPanel.removeFromLeft(knobWidth));
  mSliderDecay->component.setBounds(knobPanel.removeFromLeft(knobWidth));
  mSliderSustain->component.setBounds(knobPanel.removeFromLeft(knobWidth));
  mSliderRelease->component.setBounds(knobPanel.removeFromLeft(knobWidth));

  auto labelPanel = r.removeFromTop(LABEL_HEIGHT);
  mLabelAttack.setBounds(labelPanel.removeFromLeft(knobWidth));
  mLabelDecay.setBounds(labelPanel.removeFromLeft(knobWidth));
  mLabelSustain.setBounds(labelPanel.removeFromLeft(knobWidth));
  mLabelRelease.setBounds(labelPanel.removeFromLeft(knobWidth));
}