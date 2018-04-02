#if defined(TARGET_WIN)
#else
# include <unistd.h>
#endif

#include <stdio.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <math.h>
#include <atomic>
#include <fstream>

#include <memory.h>

#include "detector.h"
#include "Audio_WavFile.h"

void showHelp()
{
  std::cout << "Usage is: new_detector <path to file with mono audio>" << std::endl
            << "This demo works with 16bits mono PCM .wav files only." << std::endl;
}

int main(int argc, char* argv[])
{
    std::string filename;
    WavFileReader reader;

    if (argc < 2)
    {
      showHelp();
      return EXIT_FAILURE;
    }

    // Open audio file
    try
    {
      reader.open(argv[1]);
    }
    catch (std::exception& e)
    {
      std::cerr << e.what() << std::endl;
      std::cerr << "Demo accepts only 16-bits mono PCM .wav files." << std::endl;
      return EXIT_FAILURE;
    }

    if (!reader.isOpened())
    {
      std::cerr << "File " << argv[1] << " not found." << std::endl
                << "Demo accepts only 16-bits mono PCM .wav files." << std::endl;
      return EXIT_FAILURE;
    }

    // Allocate buffer
    int length = reader.size() / 2;
    short *buf = new short[length];
    float *buf_float = new float[length];
    reader.read(buf, (unsigned)length);
    reader.close();
    std::cout << filename << " file properties:" << std::endl;
    std::cout << "  samples = " << length << std::endl
              << "  rate = " << reader.rate() << std::endl;

    // Convert audio data to float
    for (int sampleIndex = 0; sampleIndex < length; sampleIndex++)
      buf_float[sampleIndex] = (float)buf[sampleIndex] / 32768.0f;


    
    // Now read audio data is in buf_float array
    // Delete 16 bits PCM buffer
    delete[] buf; buf = nullptr;

    // Instantiate new detector
    Detector detector;

    std::ofstream writer;
    writer.open("/home/michael/audio.txt");

    for (int sampleIndex = 0; sampleIndex < length - detector.getFrameSize(); sampleIndex += detector.getFrameSize())
    {
      // Find time interval
      int start = (float)sampleIndex / (reader.rate() / 1000);
      int end = start + (float(detector.getFrameSize()) / float(reader.rate() / 1000));

      // Send available frames to detector
      detector.addFrame(buf_float + sampleIndex);

      for (int i = 0; i < detector.getFrameSize(); ++i) {
          writer << buf_float[sampleIndex + i] * 1e3 << ",";
      }

      writer <<std::endl;

      // Get results for given frame
      std::cout << std::setfill(' ') << std::setw(6) << start << "ms - "
                << std::setfill(' ') << std::setw(6) << end << "ms: "
                << detector.getResult().toString() << std::endl;
    }

    delete[] buf_float;

#if defined(TARGET_WIN)
    std::cout << "Press 1 and Enter to finish app." << std::endl;
    char t[64];
    std::cin >> t;
#endif
}
