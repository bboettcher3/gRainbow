/*
  ==============================================================================

    LFOPanel.cpp
    Created: 23 Jun 2021 8:34:54pm
    Author:  brady

  ==============================================================================
*/

#include "LFOPanel.h"
#include "Utils/Utils.h"
#include "Utils/Colour.h"
#include "Modulators.h"

LFOPanel::LFOPanel(int modIdx, Parameters& parameters)
: mParameters(parameters),
mModLFO(mParameters.global.modLFOs[modIdx]),
mSliderRate(mParameters, mParameters.global.modLFOs[modIdx].rate, ParamRanges::LFO_RATE, false),
mSliderPhase(mParameters, mModLFO.phase),
mBtnMap(mParameters, mModLFO) {
    
  mBufDepth.resize(NUM_LFO_SAMPLES, 0.0f);
    
  int shapeId = 1;
  for (const LFOModSource::Shape &shape : LFOModSource::LFO_SHAPES) {
    mChoiceShape.addItem(shape.name, shapeId++);
  }
  mChoiceShape.setColour(juce::ComboBox::ColourIds::backgroundColourId, mModLFO.colour);
  mChoiceShape.setColour(juce::ComboBox::ColourIds::textColourId, mModLFO.colour);
  mChoiceShape.setSelectedId(1, juce::dontSendNotification);
  mChoiceShape.onChange = [this]() {
    ParamHelper::setParam(mModLFO.shape, mChoiceShape.getSelectedId() - 1);
  };
  mChoiceShape.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(mChoiceShape);
    
  // Default slider settings
  std::vector<std::reference_wrapper<juce::Slider>> sliders = { mSliderRate, mSliderPhase };
  for (auto& slider : sliders) {
    slider.get().setNumDecimalPlacesToDisplay(2);
    slider.get().setPopupDisplayEnabled(true, true, this);
    slider.get().setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, mModLFO.colour);
    addAndMakeVisible(slider.get());
  }
  mSliderRate.setSync(false);
  mSliderRate.setRange(ParamRanges::LFO_RATE.start, ParamRanges::LFO_RATE.end, 0.01);
  mSliderRate.setSuffix("hz");
  mSliderRate.setDoubleClickReturnValue(true, ParamDefaults::LFO_RATE_DEFAULT);
  mSliderPhase.setRange(ParamRanges::LFO_PHASE.start, ParamRanges::LFO_PHASE.end, 0.01);
  mSliderPhase.setDoubleClickReturnValue(true, ParamDefaults::LFO_PHASE_DEFAULT);
  
  // Default button settings
  std::vector<std::reference_wrapper<juce::TextButton>> buttons = { mBtnSync, mBtnBipolar, mBtnRetrigger, mBtnMap };
  for (auto& button : buttons) {
    button.get().setToggleable(true);
    button.get().setColour(juce::TextButton::textColourOffId, mModLFO.colour);
    button.get().setColour(juce::TextButton::textColourOnId, juce::Colours::black);
    button.get().setColour(juce::TextButton::buttonColourId, mModLFO.colour);
    button.get().setColour(juce::TextButton::buttonOnColourId, mModLFO.colour);
    addAndMakeVisible(button.get());
  }
  mBtnSync.setButtonText("hz");
  mBtnSync.onClick = [this]() {
    ParamHelper::setParam(mModLFO.sync, !mBtnSync.getToggleState());
  };
  mBtnBipolar.setButtonText("bipolar");
  mBtnBipolar.onClick = [this]() {
    ParamHelper::setParam(mModLFO.bipolar, !mBtnBipolar.getToggleState());
    mBufDepth.clear();
    mBufDepth.resize(NUM_LFO_SAMPLES, 0.0f); // Reset sample buffer so that they don't spill into the UI
  };
  mBtnRetrigger.setButtonText("free");
  mBtnRetrigger.onClick = [this]() {
    ParamHelper::setParam(mModLFO.retrigger, !mBtnRetrigger.getToggleState());
  };

  // Default label settings
  std::vector<std::reference_wrapper<juce::Label>> labels = { mLabelShape, mLabelRate, mLabelPhase };
  for (auto& label : labels) {
    label.get().setColour(juce::Label::ColourIds::textColourId, mModLFO.colour);
    label.get().setJustificationType(juce::Justification::centredTop);
    label.get().setFont(juce::Font(14));
    addAndMakeVisible(label.get());
  }
  mLabelShape.setText("shape", juce::dontSendNotification);
  mLabelRate.setText("rate", juce::dontSendNotification);
  mLabelPhase.setText("phase", juce::dontSendNotification);
  
  // Add listeners for relevant params
  mModLFO.shape->addListener(this);
  mModLFO.rate->addListener(this);
  mModLFO.phase->addListener(this);
  mModLFO.sync->addListener(this);
  mModLFO.bipolar->addListener(this);
  mModLFO.retrigger->addListener(this);
    
  mParamHasChanged.store(true); // Init param values
  
  startTimer(Utils::UI_REFRESH_INTERVAL);
}

LFOPanel::~LFOPanel() {
  // Remove listeners before destruction
  mModLFO.shape->removeListener(this);
  mModLFO.rate->removeListener(this);
  mModLFO.phase->removeListener(this);
  mModLFO.sync->removeListener(this);
  mModLFO.bipolar->removeListener(this);
  mModLFO.retrigger->removeListener(this);
  stopTimer();
}

void LFOPanel::visibilityChanged() {
  if (!isVisible()) {
    mBtnMap.resetMappingStatus();
  }
}

void LFOPanel::parameterValueChanged(int, float) { mParamHasChanged.store(true); }

void LFOPanel::timerCallback() {
  if (mParamHasChanged.load()) {
    mParamHasChanged.store(false);
    mBtnSync.setToggleState(mModLFO.sync->get(), juce::dontSendNotification);
    mBtnSync.setButtonText(mBtnSync.getToggleState() ? "sync" : "hz");
    mBtnBipolar.setToggleState(mModLFO.bipolar->get(), juce::dontSendNotification);
    mBtnBipolar.setButtonText(mBtnBipolar.getToggleState() ? "bipolar" : "unipolar");
    mBtnRetrigger.setToggleState(mModLFO.retrigger->get(), juce::dontSendNotification);
    mBtnRetrigger.setButtonText(mBtnRetrigger.getToggleState() ? "retrigger" : "free");
    mChoiceShape.setSelectedId(mModLFO.shape->getIndex() + 1, juce::dontSendNotification);
    mSliderRate.setValue(mModLFO.rate->get(), juce::dontSendNotification);
    mSliderRate.setSync(mBtnSync.getToggleState());
    mSliderRate.setRange(mSliderRate.getRange(), mBtnSync.getToggleState() ? mSliderRate.getRange().getLength() / (ParamRanges::SYNC_DIV_MAX) : 0.01);
    mSliderPhase.setValue(mModLFO.phase->get(), juce::dontSendNotification);
    mLabelPhase.setEnabled(mBtnRetrigger.getToggleState());
    mSliderPhase.setEnabled(mBtnRetrigger.getToggleState());
  }
  // Repaint LFO
  updateLfoPath();
}

void LFOPanel::paint(juce::Graphics& g) {
  g.setColour(Utils::PANEL_COLOUR);
  g.fillRoundedRectangle(getLocalBounds().expanded(0, 20).translated(0, -20).toFloat(), 10);
  
  g.setColour(Utils::BG_COLOUR);
  g.fillRect(mVizRect);
  
  // Draw bipolar/unipolar line
  g.setColour(mModLFO.colour.withAlpha(0.5f));
  const int poleY = mBtnBipolar.getToggleState() ? mVizRect.getCentreY() : mVizRect.getBottom();
  g.drawHorizontalLine(poleY, mVizRect.getX(), mVizRect.getRight());
  
  // Draw LFO path
  g.setColour(mModLFO.colour);
  g.strokePath(mLfoPath, juce::PathStrokeType(2, juce::PathStrokeType::JointStyle::curved));
}

void LFOPanel::resized() {
  auto r = getLocalBounds().reduced(Utils::PADDING);
  
  auto rightPanel = r.removeFromRight(r.getWidth() * 0.25f);
  
  // Labels
  auto labelPanel = r.removeFromBottom(Utils::LABEL_HEIGHT);
  const int labelWidth = labelPanel.getWidth() / 3;
  mLabelShape.setBounds(labelPanel.removeFromLeft(labelWidth));
  mLabelRate.setBounds(labelPanel.removeFromLeft(labelWidth));
  mLabelPhase.setBounds(labelPanel.removeFromLeft(labelWidth));
  labelPanel.removeFromLeft(labelWidth);

  // Sliders
  auto knobPanel = r.removeFromBottom(Utils::KNOB_HEIGHT);
  const int knobWidth = knobPanel.getWidth() / 3;
  mChoiceShape.setBounds(knobPanel.removeFromLeft(knobWidth).withTrimmedBottom(Utils::PADDING));
  mSliderRate.setBounds(knobPanel.removeFromLeft(knobWidth).withSizeKeepingCentre(Utils::KNOB_HEIGHT * 2, Utils::KNOB_HEIGHT));
  mSliderPhase.setBounds(knobPanel.removeFromLeft(knobWidth).withSizeKeepingCentre(Utils::KNOB_HEIGHT * 2, Utils::KNOB_HEIGHT));
  
  r.removeFromRight(Utils::PADDING);
  r.removeFromBottom(Utils::PADDING);
    
  mVizRect = r.toFloat();
  
  // Map button
  mBtnMap.setBounds(rightPanel.removeFromTop(mVizRect.getHeight() / 2));
  rightPanel.removeFromTop(Utils::PADDING);
  
  // Sync, polarity and retrigger buttons
  auto btnPanel = rightPanel;
  int btnWidth = btnPanel.getWidth();
  int btnHeight = ((btnPanel.getHeight() - Utils::PADDING * 2) / 3);
  mBtnSync.setBounds(btnPanel.removeFromTop(btnHeight).withSizeKeepingCentre(btnWidth, btnHeight));
  btnPanel.removeFromTop(Utils::PADDING);
  mBtnBipolar.setBounds(btnPanel.removeFromTop(btnHeight).withSizeKeepingCentre(btnWidth, btnHeight));
  btnPanel.removeFromTop(Utils::PADDING);
  mBtnRetrigger.setBounds(btnPanel.removeFromTop(btnHeight).withSizeKeepingCentre(btnWidth, btnHeight));
}

void LFOPanel::updateLfoPath() {
  // Update LFO value buffer
  mBufDepth[mBufDepthWrPos] = mModLFO.getOutput();
  // Increment write pos
  mBufDepthWrPos = (mBufDepthWrPos == (NUM_LFO_SAMPLES - 1)) ? 0 : mBufDepthWrPos + 1;
  
  // Create LFO path
  mLfoPath.clear();
//  float periodSec = 1.0 / mSliderRate.getValue();
//  if (mBtnSync.getToggleState()) {
//    float div = std::pow(2, juce::roundToInt(ParamRanges::SYNC_DIV_MAX * ParamRanges::LFO_RATE.convertTo0to1(mSliderRate.getValue())));
//    // Find synced period using fixed 120 bpm and 4 beats per bar (different from actual synthesis, just for vis)
//    periodSec = (1.0f / 120) * 60.0f * (4 / div);
//  }
//  const float numPeriods = WINDOW_SECONDS / periodSec;
  // Draw lfo shape
  const float pxPerSamp = mVizRect.getWidth() / NUM_LFO_SAMPLES;
  int maxDepthPx = mVizRect.getHeight();
  float centerY = mVizRect.getCentreY();
  if (!mBtnBipolar.getToggleState()) {
    centerY = mVizRect.getBottom();
  }
  float curX = mVizRect.getX();
  int curIdx = mBufDepthWrPos;
  mLfoPath.startNewSubPath(curX, centerY - (mBufDepth[curIdx] * maxDepthPx));
  for (int i = 0; i < NUM_LFO_SAMPLES - 1; ++i) {
    curX += pxPerSamp;
    curIdx = (curIdx == (NUM_LFO_SAMPLES - 1)) ? 0 : curIdx + 1;
    const int depthPx = mBufDepth[curIdx] * maxDepthPx;
    const float y = centerY - depthPx;
    mLfoPath.lineTo(curX, y);
  }

  repaint();
}