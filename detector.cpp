#include "detector.h"
#include <sstream>
#include <iomanip>
#include <math.h>
#include <iostream>
#include <cstring>

Detector::Detector()
{

}

Detector::~Detector()
{}

int Detector::getFrameSize() const
{
    return 2000; // 500 samples - just for example
}

void Detector::setRate(int rate)
{
    mRate = rate;
}

int Detector::getRate() const
{
    return mRate;
}

void Detector::setThreshold(float threshold)
{
    mThreshold = threshold;
}

float Detector::getThreshold() const
{
    return mThreshold;
}

float Detector::mean(const float* buffer){
    double mAverage = 0.0;
    for (int i = 0; i < getFrameSize(); i++)
        mAverage += buffer[i];

    mAverage /= getFrameSize();

    return mAverage;
}

float Detector::sumFunc(const float* buffer){

    float sensitivity = 0.4;
    float signalMean = mean(buffer);
    double dy[getFrameSize()];
    double dyMean;
    double dyStdDev;

    for (int j = getFrameSize()-1; j > 1; j--) {
        dy[j] = (buffer[j+1] - buffer[j-1]) / 2.;
    }

    double tempSum = 0.;

    for (int i = 1; i < getFrameSize()-1; i++)
        tempSum += dy[i];

    dyMean = tempSum / (getFrameSize() - 2);


    double tempSumOfSquaresOfDeviations = 0;
    for (int i = 0; i < getFrameSize() - 1; i++) {
        double deviation = dy[i] - dyMean;
        tempSumOfSquaresOfDeviations += (deviation * deviation);
    }
    dyStdDev = std::sqrt(tempSumOfSquaresOfDeviations / (getFrameSize()-2));

    float dyThreshold = 2 * dyStdDev / sensitivity + dyMean;
    double sThreshold = 2 * signalMean / sensitivity;

    bool inClick = false;

    for (int i = getFrameSize() - 1; i > 1; i--) {
        if(!inClick){
            if(dy[i] > dyThreshold && buffer[i] > sThreshold){
                inClick = true;
//                clickEnd =

//                std::cout << "Start" << std::endl;
            }
        }
        else{
            if(buffer[i] < sThreshold){
                inClick = false;
//                std::cout << "Finish" << std::endl;

            }
        }
    }
}


void Detector::hightPass(float* buffer){
    for (int i = 0; i < getFrameSize(); ++i) {
        float tempSum = 0.;
        for (int j = i; j < i + 8; ++j) {
            tempSum = buffer[j-1] - 2 * buffer[j] + buffer[j+1];
        }

        if(tempSum < 0)
            tempSum = -tempSum;

        buffer[i] = std::sqrt(tempSum / 6.);
    }
}

void Detector::addFrame(const float* buffer)
{

    float* bufferTemp = new float[getFrameSize()];

    std::memcpy(bufferTemp, buffer, getFrameSize());

    hightPass(bufferTemp);

    sumFunc(bufferTemp);


}

Detector::Result Detector::getResult()
{
    Result r;

    r.mAverageValue = mAverage;
    r.mAverageExceeded = r.mAverageValue > mThreshold;

    return r;
}

std::string Detector::Result::toString() const
{
    std::ostringstream oss;
    oss << "Average: " << std::fixed << std::setw( 7 ) << std::setprecision( 4 )  << mAverageValue << ", exceeded: " << (mAverageExceeded ? "yes" : "no");

    return oss.str();
}
