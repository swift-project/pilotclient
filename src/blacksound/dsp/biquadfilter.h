#ifndef BIQUADFILTER_H
#define BIQUADFILTER_H

#include "blacksound/blacksoundexport.h"

class BiQuadFilter
{
public:
    BiQuadFilter() = default;

    float transform(float inSample);
    void setCoefficients(double aa0, double aa1, double aa2, double b0, double b1, double b2);
    void setLowPassFilter(float sampleRate, float cutoffFrequency, float q);
    void setPeakingEq(float sampleRate, float centreFrequency, float q, float dbGain);
    void setHighPassFilter(float sampleRate, float cutoffFrequency, float q);

    static BiQuadFilter lowPassFilter(float sampleRate, float cutoffFrequency, float q);
    static BiQuadFilter highPassFilter(float sampleRate, float cutoffFrequency, float q);
    static BiQuadFilter peakingEQ(float sampleRate, float centreFrequency, float q, float dbGain);

private:
    double a0;
    double a1;
    double a2;
    double a3;
    double a4;

    // state
    float x1 = 0.0;
    float x2 = 0.0;
    float y1 = 0.0;
    float y2 = 0.0;
};

#endif // BIQUADFILTER_H
