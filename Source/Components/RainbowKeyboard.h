/*
  ==============================================================================

    RainbowKeyboard.h
    Created: 2 Jun 2021 10:27:38pm
    Author:  brady

  ==============================================================================
*/

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_basics/juce_audio_basics.h>

#include "../Utils.h"

/**
  @brief The RainbowKeyboard is for displaying the keys being pressed, but all the actual listening of midi inputs will come from
  editor.

  The RainbowKeyboard is able to detect mouse input and inject midi inputs
*/
class RainbowKeyboard : public juce::Component {
 public:
  RainbowKeyboard(juce::MidiKeyboardState& state);
  ~RainbowKeyboard() override;

  void paint(juce::Graphics&) override;
  void resized() override;

  void mouseMove(const juce::MouseEvent&) override;
  void mouseDrag(const juce::MouseEvent&) override;
  void mouseDown(const juce::MouseEvent&) override;
  void mouseUp(const juce::MouseEvent&) override;
  void mouseEnter(const juce::MouseEvent&) override;
  void mouseExit(const juce::MouseEvent&) override;

  void setMidiNotes(const juce::Array<Utils::MidiNote>& midiNotes);

  // Returns a value between 0.0-1.0 representing the note's x position on the
  // keyboard
  float getPitchXRatio(Utils::PitchClass pitchClass);

 private:
  static constexpr float BLACK_NOTE_SIZE_RATIO = 0.7f;
  static constexpr int MIDI_CHANNEL = 1;
  juce::Random mRandom;

  juce::MidiKeyboardState& mState;

  // holds the velocity of each pitch class, if zero, then note is not played
  std::array<float, Utils::PitchClass::COUNT> mNoteVelocity;

  // These allow using the mouse to click a key
  void updateMouseState(const juce::MouseEvent& e, bool isDown);
  Utils::MidiNote xyMouseToNote(juce::Point<float> pos);
  // Note being currently hovered by the mouse
  Utils::MidiNote mHoverNote;
  // Keeps track of the note being played because of the mouse injected input
  Utils::MidiNote mMouseNote;

  // Notes rectangle are recreated on resize and then just become a LUT
  juce::Rectangle<float> mNoteRectangleMap[Utils::PitchClass::COUNT];
  void fillNoteRectangleMap();

  void drawKey(juce::Graphics& g, Utils::PitchClass pitchClass);

  // if a key is black or white is only a keyboard UI issue
  Utils::PitchClass WHITE_KEYS_PITCH_CLASS[7] = {Utils::PitchClass::C, Utils::PitchClass::D, Utils::PitchClass::E,
                                                 Utils::PitchClass::F, Utils::PitchClass::G, Utils::PitchClass::A,
                                                 Utils::PitchClass::B};

  Utils::PitchClass BLACK_KEYS_PITCH_CLASS[5] = {Utils::PitchClass::Cs, Utils::PitchClass::Ds, Utils::PitchClass::Fs,
                                                 Utils::PitchClass::Gs, Utils::PitchClass::As};

  static inline bool isBlackKey(Utils::PitchClass pitchClass) { return ((1 << (pitchClass)) & 0x054a) != 0; }

  // LUT for animation values to save from recomputing each frame
  static constexpr int ANIMATION_BLOCK_DIVISOR = 12;
  struct AnimationLUT {
    float velocityLine;
    float inverseVelocityLine;
    float noteWidthSplit;
    float blockCount;
    float yDelta;
  };
  std::array<AnimationLUT, Utils::PitchClass::COUNT> mAnimationLUT;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RainbowKeyboard)
};