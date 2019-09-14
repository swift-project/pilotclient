#ifndef BIQUADFILTER_H
#define BIQUADFILTER_H

#include "blacksound/blacksoundexport.h"

// filter type enumeration
enum class BiQuadFilterType {
    None,
    LowPass,
    HighPass,
    BandPass,
    Notch,
    Peak,
    LowShelf,
    HighShelf
};

#define BQN 3

class BiQuadFilter
{
public:
    BiQuadFilter() = default;

    BiQuadFilter(BiQuadFilterType type,
                 int fs = 44100,
                 double fc = 1000,
                 double Q = 0.7071,
                 double peakGain = 0);

    float process(float input);

    void clear();

private:
    void calculate();

    double m_A[BQN];
    double m_B[BQN];
    float m_X[BQN];
    float m_Y[BQN];
    unsigned int m_index = 0;
    double m_fs = 44100.0;
    BiQuadFilterType m_type = BiQuadFilterType::None;
    double m_fc = 1000.0;
    double m_Q = 0.7071;
    double m_peakGain = 0.0;
};

#endif // BIQUADFILTER_H
