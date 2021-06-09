/*
  ==============================================================================

    PitchDetector.h
    Created: 24 Apr 2021 2:07:10pm
    Author:  brady

    Pitch detection algorithm using HPCP algorithm from:

    TONAL DESCRIPTION OF MUSIC AUDIO SIGNALS
    Gutierrez 2006

    and

    Melody Extraction Using Chroma-Level Note Tracking and Pitch Mapping
    Weiwei Zhang , Zhe Chen and Fuliang Yin 2018

    http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.76.8192&rep=rep1&type=pdf

    also Melody Detection in Polyphonic Audio
    Rui Pedro Pinto de Carvalho e Paiva 2006

    https://www.researchgate.net/publication/255908118

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "Fft.h"

class PitchDetector : juce::Thread {
 public:
  static constexpr auto MIN_MIDINOTE = 43;
  static constexpr auto MAX_MIDINOTE = 91;

  PitchDetector();
  ~PitchDetector();

  static enum PitchClass { NONE = -1, C, Cs, D, Ds, E, F, Fs, G, Gs, A, As, B };

  typedef struct Pitch {
    PitchClass pitchClass;
    float posRatio; // position in track from 0-1
    float duration; // note duration in samples
    float gain; // pitch salience
    Pitch(PitchClass pitchClass, float posRatio, float duration, float gain)
        : pitchClass(pitchClass), posRatio(posRatio), duration(duration), gain(gain) {}
  } Pitch;

  std::function<void(std::vector<std::vector<float>>&)> onPitchesUpdated =
      nullptr;

  void processBuffer(juce::AudioBuffer<float>* fileBuffer, double sampleRate);
  std::vector<Pitch>& getPitches() { return mPitches; }

  void run() override;

 private:
  // FFT
  static constexpr auto FFT_SIZE = 4096;
  static constexpr auto HOP_SIZE = 512;
  // HPCP
  static constexpr auto REF_FREQ = 440;
  static constexpr auto MAX_SPEC_PEAKS = 60;
  static constexpr auto NUM_PITCH_CLASSES = 120;
  static constexpr auto HPCP_WINDOW_LEN = 1.34f;
  static constexpr auto NUM_HARMONIC_WEIGHTS = 3;
  static constexpr auto MIN_FREQ = 80;
  static constexpr auto MAX_FREQ = 3500;
  static constexpr auto HARMONIC_PRECISION = 0.00001;
  static constexpr auto MAGNITUDE_THRESHOLD = 0.00001;
  // Pitch segmenting
  static constexpr auto NUM_ACTIVE_SEGMENTS = 3;
  static constexpr auto MAX_DEVIATION_CENTS = 90;
  static constexpr auto INVALID_BIN = -1;
  static constexpr int  NUM_DEVIATION_BINS =
      (NUM_PITCH_CLASSES / 12.0) * (MAX_DEVIATION_CENTS / 100.0);
  static constexpr auto MAX_IDLE_TIME_MS = 62.5;
  static constexpr auto MIN_NOTE_TIME_MS = 125;

  typedef struct PitchSegment {
    float binNum; // Bin number in HPCP
    int startFrame; // Start frame of segment
    int idleFrame; // Start frame of when segment began being idle (or -1 when active)
    float salience; // Confidence level from 0-1
    PitchSegment() : binNum(0), startFrame(0), idleFrame(-1), salience(0.0) {}
  } PitchSegment;

  typedef struct Peak {
    float binNum;  // Bin number in frame
    float gain;
    Peak() : binNum(-1), gain(0) {}
    Peak(float binNum, float gain) : binNum(binNum), gain(gain) {}
  } Peak;

  typedef struct HarmonicWeight {
    float semitone;
    float gain;
    HarmonicWeight(int semitone, float harmonicStrength)
        : semitone(semitone), gain(harmonicStrength) {}
  } HarmonicWeight;

  juce::AudioBuffer<float>* mFileBuffer = nullptr;
  Fft mFft;
  double mSampleRate;
  // HPCP fields
  std::vector<HarmonicWeight> mHarmonicWeights;
  std::vector<std::vector<float>> mHPCP;  // harmonic pitch class profile
  std::vector<Pitch> mPitches;
  std::vector<std::vector<float>> mPitchesTest;
  // Pitch segment fields
  std::array<PitchSegment, NUM_ACTIVE_SEGMENTS> mSegments;

  void computeHPCP();
  void segmentPitches();
  void estimatePitches();
  PitchClass getPitchClass(float binNum); // Finds the closest pitch class
  Peak interpolatePeak(int frame, int bin);
  void interpolatePeak(const float leftVal, const float middleVal,
                       const float rightVal, int currentBin, float& resultVal,
                       float& resultBin) const;
  std::vector<Peak> getPeaks(int numPeaks, std::vector<float>& frame);
  void initHarmonicWeights();
};