/* Copyright(C) 2007-2017 VoIPobjects (voipobjects.com)
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#ifndef __AUDIO_WAVFILE_H
#define __AUDIO_WAVFILE_H

#include <stdio.h>
#include <string>
#include <memory>
#include <mutex>

class WavFileReader
{
protected:
  FILE* mHandle;
  short mChannels;
  short mBits;
  int mRate;
  std::string mFileName;
  mutable std::recursive_mutex mFileMtx;
  unsigned mDataOffset;
  unsigned mDataLength;
  std::string readChunk();
public:
  WavFileReader();
  ~WavFileReader();

  bool open(const std::string& filename);
  void close();
  bool isOpened();
  void rewind();
  int rate() const;

  // This method returns number of read samples
  unsigned read(short* buffer, unsigned samples);
  std::string filename() const;
  unsigned size() const;
};

typedef std::shared_ptr<WavFileReader> PWavFileReader;


#endif
