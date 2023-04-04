/*
  ==============================================================================

    NoteGrid.cpp
    Created: 24 Aug 2021 3:54:04pm
    Author:  brady

  ==============================================================================
*/

#include "NoteGrid.h"
#include "../Utils.h"

//==============================================================================
NoteGrid::NoteGrid(ParamsNote& paramsNote) : mParamsNote(paramsNote) { setFramesPerSecond(REFRESH_RATE_FPS); }

NoteGrid::~NoteGrid() {}

void NoteGrid::paint(juce::Graphics& g) {
  g.fillAll(juce::Colours::black);  // clear the background

  juce::Colour mainColour = juce::Colours::white;

  // Main title
  g.setColour(mainColour);
  g.fillRoundedRectangle(mTitleEdgeRect, 10.0f);
  g.setColour(juce::Colours::black);
  g.drawText(juce::String(MAIN_TITLE), mTitleRect, juce::Justification::centred);

  for (int i = 0; i < Utils::PitchClass::COUNT; ++i) {
    // Note column
    juce::Colour noteColour = Utils::getRainbow12Colour(i);
    g.setColour(noteColour);
    g.fillRect(mColumn.withPosition(mGridRect.getTopLeft() + juce::Point<int>(i * mSquareSize + 1, 1)).reduced(1.0f));
    // Note title
    g.drawRoundedRectangle(
        mColumn.withPosition(mNoteNamesRect.getTopLeft() + juce::Point<int>(i * mSquareSize + 1, 1)).toFloat().reduced(1.0f), 10.0f,
        1.0f);
    g.setColour(juce::Colours::white);
    g.drawText(PITCH_CLASS_NAMES[i], mNoteNamesRect.getX() + i * mSquareSize, mNoteNamesRect.getY(), mSquareSize, mSquareSize,
               juce::Justification::centred);
    for (int j = 0; j < NUM_GENERATORS; ++j) {
      juce::Point<int> squarePos = mGridRect.getTopLeft() + juce::Point<int>(i * mSquareSize + 1, j * mSquareSize + 1);

      // Clear rect area first to eliminate note colour
      g.setColour(juce::Colours::black);
      g.fillRect(mSquare.withPosition(squarePos).reduced(2.0f));

      // Fill and label candidate if enabled, draw frame if not
      g.setColour(Utils::getRainbow12Colour(i));
      if (mParamsNote.notes[i]->generators[j]->enable->get()) {
        g.fillRect(mSquare.withPosition(squarePos).reduced(2.0f));
        g.setColour(juce::Colours::black);
        g.drawText(juce::String(mParamsNote.notes[i]->generators[j]->candidate->get() + 1), mSquare.withPosition(squarePos),
                   juce::Justification::centred);
      } else {
        g.drawRect(mSquare.withPosition(squarePos).reduced(1.0f), 2.0f);
      }
    }
  }

  // Outline rect
  g.setColour(mainColour);
  g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(1.0f), 10.0f, 2.0f);
}

void NoteGrid::resized() {
  mGridRect = getLocalBounds().reduced(PADDING_SIZE);
  mSquareSize = mGridRect.getWidth() / static_cast<float>(Utils::PitchClass::COUNT);
  mSquare = juce::Rectangle<int>(mSquareSize, mSquareSize);
  mColumn = juce::Rectangle<int>(mSquareSize, mSquareSize * NUM_GENERATORS);
  mTitleRect = mGridRect.removeFromTop(TITLE_HEIGHT);
  mTitleEdgeRect = mTitleRect.reduced(0, PADDING_SIZE / 2).toFloat();
  mNoteNamesRect = mGridRect.removeFromTop(TITLE_HEIGHT);
}