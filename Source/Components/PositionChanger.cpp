/*
  ==============================================================================

    PositionChanger.cpp
    Created: 3 Jul 2021 10:08:25pm
    Author:  brady

  ==============================================================================
*/

#include "PositionChanger.h"

//==============================================================================
PositionChanger::PositionChanger() {}

PositionChanger::~PositionChanger() {}

void PositionChanger::paint(juce::Graphics& g) {
  g.fillAll(juce::Colours::black);  // clear the background
  juce::Colour bgColour = mIsActive ? mColour : juce::Colours::darkgrey;

  /* Draw left arrow */
  if (mIsOverLeftArrow) {
    juce::Colour fillColour = mIsClickingArrow ? bgColour : bgColour.interpolatedWith(juce::Colours::black, 0.8);
    g.setColour(fillColour);
    g.fillPath(mLeftPath);
  }
  g.setColour(bgColour);
  g.strokePath(mLeftPath, juce::PathStrokeType(2));
  g.drawArrow(mLeftArrowLine, 2, 6, 6);

  /* Draw right arrow */
  if (mIsOverRightArrow) {
    juce::Colour fillColour = mIsClickingArrow ? bgColour : bgColour.interpolatedWith(juce::Colours::black, 0.8);
    g.setColour(fillColour);
    g.fillPath(mRightPath);
  }
  g.setColour(bgColour);
  g.strokePath(mRightPath, juce::PathStrokeType(2));
  g.drawArrow(mRightArrowLine, 2, 6, 6);

  /* Fill in title section to mask ellipses */
  g.setColour(juce::Colours::black);
  g.fillRect(mTitleRect.reduced(2, 0));

  /* Draw top/bottom borders */
  g.setColour(bgColour);
  g.drawRect(mTitleRect, 2);

  /* Draw position text */
  juce::String posString;
  if (mPosition >= 0 && mNumPositions > 0) {
    int posNum = (mPosition >= 0) ? mPosition + 1 : 0;
    posString = juce::String(posNum) + juce::String(" / ") + juce::String(mNumPositions);
  } else {
    posString = "EMPTY";  // only can fit 5 letters in box, better then a blank boxs
  }
  g.setColour(bgColour);
  g.drawText(posString, mTitleRect, juce::Justification::centred);

  /* Solo button */
  if (mIsOverSolo || mIsSolo) {
    g.setColour(mIsSolo ? juce::Colours::blue : juce::Colours::blue.withAlpha(0.3f));
    g.fillRect(mSoloRect);
  }
  g.setColour(juce::Colours::blue);
  g.drawRect(mSoloRect, 2.0f);
  g.setColour(juce::Colours::white);
  g.drawFittedText("solo", mSoloRect.reduced(4).toNearestInt(), juce::Justification::centred, 1);
}

void PositionChanger::resized() {
  const int arrowWidth = getWidth() * ARROW_PERC;
  const float soloHeight = getHeight() * SOLO_HEIGHT_PERC;
  const float selectorHeight = getHeight() - soloHeight;

  mLeftPath.clear();
  mLeftPath.addEllipse(1, 1, (arrowWidth * 2) - 1, selectorHeight - 2);
  mLeftArrowLine = juce::Line<float>(arrowWidth - (arrowWidth * 0.1), selectorHeight / 2, (arrowWidth * 0.3), selectorHeight / 2);

  const int rightStart = getWidth() * (ARROW_PERC + TITLE_PERC);
  mRightPath.clear();
  mRightPath.addEllipse(rightStart - arrowWidth, 1, (arrowWidth * 2) - 1, selectorHeight - 2);
  mRightArrowLine =
      juce::Line<float>(rightStart + (arrowWidth * 0.1), selectorHeight / 2, getWidth() - (arrowWidth * 0.3), selectorHeight / 2);

  mTitleRect = getLocalBounds().withHeight(selectorHeight).withSizeKeepingCentre(getWidth() * TITLE_PERC, selectorHeight);
  mSoloRect = mTitleRect.translated(0, selectorHeight).withHeight(soloHeight - 2).toFloat();
}

void PositionChanger::setActive(bool isActive) {
  mIsActive = isActive;
  repaint();
}

void PositionChanger::setSolo(bool isSolo) {
  mIsSolo = isSolo;
  repaint();
}

void PositionChanger::setColour(juce::Colour colour) {
  mColour = colour;
  repaint();
}

void PositionChanger::setNumPositions(int numPositions) {
  mNumPositions = numPositions;
  repaint();
}

void PositionChanger::mouseMove(const juce::MouseEvent& e) { updateMouseOver(e, false); }

void PositionChanger::mouseDrag(const juce::MouseEvent& e) { updateMouseOver(e, true); }

void PositionChanger::mouseDown(const juce::MouseEvent& e) { updateMouseOver(e, true); }

void PositionChanger::mouseUp(const juce::MouseEvent& e) { updateMouseOver(e, false); }
void PositionChanger::mouseEnter(const juce::MouseEvent& e) { updateMouseOver(e, false); }

void PositionChanger::mouseExit(const juce::MouseEvent& e) { updateMouseOver(e, false); }

void PositionChanger::updateMouseOver(const juce::MouseEvent& e, bool isDown) {
  auto pos = e.getEventRelativeTo(this).position;
  if (isLeftArrow(pos)) {
    if (!mIsClickingArrow && isDown) {
      positionChanged(false);
    }
    mIsOverLeftArrow = true;
  } else if (isRightArrow(pos)) {
    if (!mIsClickingArrow && isDown) {
      positionChanged(true);
    }
    mIsOverRightArrow = true;
  } else {
    mIsOverLeftArrow = false;
    mIsOverRightArrow = false;
  }
  mIsClickingArrow = isDown;
  if (mSoloRect.contains(pos)) {
    mIsOverSolo = true;
    if (isDown) {
      mIsSolo = !mIsSolo;
      if (onSoloChanged != nullptr) onSoloChanged(mIsSolo);
    }
  } else {
    mIsOverSolo = false;
  }
  repaint();
}

bool PositionChanger::isLeftArrow(juce::Point<float> point) {
  juce::Rectangle<int> arrowRect =
      getLocalBounds().removeFromLeft(getWidth() * ARROW_PERC).withHeight(getHeight() * (1.0f - SOLO_HEIGHT_PERC));
  return arrowRect.contains(point.toInt());
}

bool PositionChanger::isRightArrow(juce::Point<float> point) {
  juce::Rectangle<int> arrowRect =
      getLocalBounds().removeFromRight(getWidth() * ARROW_PERC).withHeight(getHeight() * (1.0f - SOLO_HEIGHT_PERC));
  return arrowRect.contains(point.toInt());
}

void PositionChanger::positionChanged(bool isRight) {
  if (onPositionChanged != nullptr) {
    onPositionChanged(isRight);
  }
}

void PositionChanger::setPositionNumber(int position) {
  mPosition = position;
  repaint();
}