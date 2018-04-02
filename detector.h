#ifndef __DETECTOR_H
#define __DETECTOR_H

#include <string>

class Detector
{
public:
  Detector();
  ~Detector();

  // Returns desired frame size in samples.
  int getFrameSize() const;

  // Setter/getter for samplerate. Many detectors do not need it at all - but maybe it will be useful
  void setRate(int rate);
  int getRate() const;

  // It is for our example detector only
  void setThreshold(float threshold);
  float getThreshold() const;

  // Sends audio frame to detector. Its length is getFrameSize() samples
  void addFrame(const float* buffer);

  // Result of detector's work. It is just example. Replace with something useful.
  struct Result
  {
    bool mAverageExceeded = false;
    float mAverageValue = 0.0f;

    std::string toString() const;
  };

  // Returns work of detector. Can be called after each addFrame().
  Result getResult();

private:
  float mean(const float* buffer);
  float sumFunc(const float* buffer);
  void hightPass(float* buffer);

private:
  int mRate = 0;

  float mAverage = 0.0f;
  float mThreshold = 0.1f;
};
#endif

