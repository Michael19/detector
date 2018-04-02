/* Copyright(C) 2007-2017 VoIPobjects (voipobjects.com)
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Audio_WavFile.h"

#include <memory.h>

#ifndef WORD
# define WORD unsigned short
#endif
#ifndef DWORD
# define DWORD unsigned int
#endif

typedef struct {
  WORD  wFormatTag;
  WORD  nChannels;
  DWORD nSamplesPerSec;
  DWORD nAvgBytesPerSec;
  WORD  nBlockAlign;
  WORD  wBitsPerSample;
  WORD  cbSize;
}WAVEFORMATEX;
#define WAVE_FORMAT_PCM 1

#define AUDIO_CHANNELS (1)


#define LOCK_INSTANCE std::unique_lock<std::recursive_mutex> lock(mFileMtx)

// ---------------------- WavFileReader -------------------------
WavFileReader::WavFileReader()
:mHandle(NULL), mRate(0)
{
  mDataOffset = 0;
}

WavFileReader::~WavFileReader()
{
}

#define THROW_READERROR     throw std::runtime_error(".wav access error");

std::string WavFileReader::readChunk()
{
  char name[5];
  if (fread(name, 1, 4, mHandle) != 4)
    THROW_READERROR;

  name[4] = 0;
  std::string result = name;
  unsigned size;
  if (fread(&size, 4, 1, mHandle) != 1)
    THROW_READERROR;

  if (result == "fact")
    fread(&mDataLength, 4, 1, mHandle);
  else
  if (result != "data")
    fseek(mHandle, size, SEEK_CUR);
  else
    mDataLength = size;
	
  return result;
}

bool WavFileReader::open(const std::string& filename)
{
  LOCK_INSTANCE;

  try
  {
    mHandle = fopen(filename.c_str(), "rb");
    if (NULL == mHandle)
      return false;
    
    // Read the .WAV header
    char riff[4];
    if (fread(riff, 4, 1, mHandle) < 1)
		  THROW_READERROR;
  	
    if (!(riff[0] == 'R' && riff[1] == 'I' && riff[2] == 'F' && riff[3] == 'F'))
		  THROW_READERROR;

    // Read the file size
    unsigned int filesize = 0;
    if (fread(&filesize, 4, 1, mHandle) < 1)
      THROW_READERROR;

    char wavefmt[9];
    if (fread(wavefmt, 8, 1, mHandle) < 1)
      THROW_READERROR;

    wavefmt[8] = 0;
    if (strcmp(wavefmt, "WAVEfmt ") != 0)
      THROW_READERROR;

    unsigned fmtSize = 0;
    if (fread(&fmtSize, 4, 1, mHandle) < 1)
		  THROW_READERROR;

    unsigned fmtStart = (unsigned)ftell(mHandle);

    unsigned short formattag  = 0;
    if (fread(&formattag, 2, 1, mHandle) < 1)
      THROW_READERROR;
    
    if (formattag != 1/*WAVE_FORMAT_PCM*/)
      THROW_READERROR;

    mChannels = 0;
    if (fread(&mChannels, 2, 1, mHandle) < 1)
		  THROW_READERROR;
    
    mRate = 0;
    if (fread(&mRate, 4, 1, mHandle) < 1)
      THROW_READERROR;

    unsigned int avgbytespersec = 0;
    if (fread(&avgbytespersec, 4, 1, mHandle) < 1)
      THROW_READERROR;
    
    unsigned short blockalign = 0;
    if (fread(&blockalign, 2, 1, mHandle) < 1)
      THROW_READERROR;
    
    mBits = 0;
    if (fread(&mBits, 2, 1, mHandle) < 1)
      THROW_READERROR;
    
    if (mBits !=8 && mBits != 16)
      THROW_READERROR;

    // Read the "chunk"
    fseek(mHandle, fmtStart + fmtSize, SEEK_SET);
    //unsigned pos = ftell(mHandle);
    mDataLength = 0;
    while (readChunk() != "data")
      ;
  	
    mFileName = filename;
    mDataOffset = (unsigned)ftell(mHandle);
  }
  catch(...)
  {
    fclose(mHandle); mHandle = NULL;
  }
  return isOpened();
}

void WavFileReader::close()
{
  LOCK_INSTANCE;

  if (NULL != mHandle)
    fclose(mHandle);
  mHandle = NULL;
}

int WavFileReader::rate() const
{
  return mRate;
}

unsigned WavFileReader::read(short* buffer, unsigned samples)
{
  LOCK_INSTANCE;

  if (!mHandle)
    return 0;

  // Get number of samples that must be read from source file
  int read = fread(buffer, 1, samples * 2, mHandle);// / mChannels / (mBits / 8);

  return read / 2 / AUDIO_CHANNELS;
}

bool WavFileReader::isOpened()
{
  LOCK_INSTANCE;

  return (mHandle != 0);
}

void WavFileReader::rewind()
{
  LOCK_INSTANCE;

  if (mHandle)
    fseek(mHandle, mDataOffset, SEEK_SET);
}

std::string WavFileReader::filename() const
{
  LOCK_INSTANCE;

  return mFileName;
}

unsigned WavFileReader::size() const
{
  LOCK_INSTANCE;

  return mDataLength;
}
