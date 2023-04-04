/*
  ==============================================================================

    FilterControl.h
    Created: 3 Aug 2021 4:46:28pm
    Author:  nrmvl

  ==============================================================================
*/

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../Utils.h"
#include "../Parameters.h"

//==============================================================================
/*
 */
class FilterControl : public juce::Component {
 public:
  FilterControl();
  ~FilterControl() override;

  void paint(juce::Graphics&) override;
  void resized() override;

  void mouseMove(const juce::MouseEvent& event) override;
  void mouseExit(const juce::MouseEvent& event) override;
  void mouseUp(const juce::MouseEvent& event) override;

  // static enum FilterType { NONE, LOWPASS, HIGHPASS, BANDPASS };
  void setActive(bool isActive);
  void setCutoff(float cutoff);
  void setResonance(float resonance);
  void setColour(juce::Colour colour);
  // void setFilterType(Utils::FilterType filterType);
  void setFilterType(int filterTypeIndex);
  float filterTypeToCutoff(Utils::FilterType filterType);

  std::function<void(Utils::FilterType filterType)> onFilterTypeChange = nullptr;

 private:
  /* Parameters */
  bool mIsActive = false;
  float mCutoff = 0.0f;
  float mResonance = ParamDefaults::FILTER_RESONANCE_DEFAULT;
  Utils::FilterType mFilterType = Utils::FilterType::LOWPASS;
  static constexpr auto FILTER_TYPE_BUTTON_HEIGHT = 30;
  juce::Colour mColour;
  int mFilter = 0;
  Utils::FilterType mCurHoverFilterType = Utils::NO_FILTER;
  Utils::FilterType mCurSelectedFilterType = Utils::NO_FILTER;

  // UI values saved on resize
  juce::Rectangle<float> mLowPassRect;
  juce::Rectangle<float> mLowPassTextRect;
  juce::Rectangle<float> mHighPassRect;
  juce::Rectangle<float> mHighPassTextRect;
  juce::Rectangle<float> mBandPassRect;
  juce::Rectangle<float> mBandPassTextRect;
  int mBtnPadding;
  int mBtnResPadding;  // with MAX_RES_HEIGHT

  static constexpr auto PADDING_SIZE = 5;
  static constexpr auto MAX_RES_HEIGHT = 20;
  static constexpr auto RES_WIDTH = 20;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterControl)
};