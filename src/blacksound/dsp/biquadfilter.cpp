#include "biquadfilter.h"

#include <QtMath>
#include <algorithm>

// return buffer index
#define BUFFIX(n,k) ((n + k + BQN) % BQN)

BiQuadFilter::BiQuadFilter(BiQuadFilterType type, int fs, double fc, double Q, double peakGain) :
    m_fs(fs),
    m_type(type),
    m_fc(fc),
    m_Q(Q),
    m_peakGain(peakGain)
{
    clear();
    calculate();
}

float BiQuadFilter::process(float input)
{
    unsigned int n = m_index;

    // put input on to buffer
    m_X[BUFFIX(n,0)] = input;

    // process input
    m_Y[BUFFIX(n,0)] =
        m_B[0] * m_X[BUFFIX(n,  0)] +
        m_B[1] * m_X[BUFFIX(n, -1)] +
        m_B[2] * m_X[BUFFIX(n, -2)] -
        m_A[1] * m_Y[BUFFIX(n, -1)] -
        m_A[2] * m_Y[BUFFIX(n, -2)];

    // write output
    float output = m_Y[BUFFIX(n, 0)];

    // step through buffer
    m_index = BUFFIX(n, 1);

    return output;
}

void BiQuadFilter::clear()
{
    for (int n = 0; n < BQN; n++)
    {
        m_X[n] = 0.0;
        m_Y[n] = 0.0;
    }
}

void BiQuadFilter::calculate()
{
    double AA = qPow(10.0, m_peakGain / 40.0);
    double w0 = 2.0 * M_PI * m_fc / m_fs;
    double alpha = qSin(w0) / (2.0 * m_Q);

    double cos_w0 = qCos(w0);
    double sqrt_AA = qSqrt(AA);

    // source : http://www.musicdsp.org/files/Audio-EQ-Cookbook.txt

    switch (m_type) {
        case BiQuadFilterType::LowPass:
            m_B[0] = (1.0 - cos_w0) / 2.0;
            m_B[1] = 1.0 - cos_w0;
            m_B[2] = (1.0 - cos_w0) / 2.0;
            m_A[0] = 1 + alpha;
            m_A[1] = -2.0 * cos_w0;
            m_A[2] = 1.0 - alpha;
            break;
        case BiQuadFilterType::HighPass:
            m_B[0] = (1.0 + cos_w0) / 2.0;
            m_B[1] = -(1.0 + cos_w0);
            m_B[2] = (1.0 + cos_w0) / 2.0;
            m_A[0] = 1.0 + alpha;
            m_A[1] = -2.0 * cos_w0;
            m_A[2] = 1.0 - alpha;
            break;
        case BiQuadFilterType::BandPass: // (constant 0 dB peak gain)
            m_B[0] = alpha;
            m_B[1] = 0.0;
            m_B[2] = -alpha;
            m_A[0] = 1.0 + alpha;
            m_A[1] = -2.0 * cos_w0;
            m_A[2] = 1.0 - alpha;
            break;
        case BiQuadFilterType::Notch:
            m_B[0] = 1.0;
            m_B[1] = -2.0 * cos_w0;
            m_B[2] = 1.0;
            m_A[0] = 1.0 + alpha;
            m_A[1] = -2.0 * cos_w0;
            m_A[2] = 1.0 - alpha;
            break;
        case BiQuadFilterType::Peak:
            m_B[0] = 1.0 + alpha*AA;
            m_B[1] = -2.0 * cos_w0;
            m_B[2] = 1.0 - alpha*AA;
            m_A[0] = 1.0 + alpha/AA;
            m_A[1] = -2.0 * cos_w0;
            m_A[2] = 1.0 - alpha/AA;
            break;
        case BiQuadFilterType::LowShelf:
            m_B[0] = AA*( (AA+1.0) - (AA-1.0) * cos_w0 + 2.0 * sqrt_AA * alpha );
            m_B[1] = 2.0*AA*( (AA-1) - (AA+1.0) * cos_w0 );
            m_B[2] = AA*( (AA+1.0) - (AA-1.0) * cos_w0 - 2.0 * sqrt_AA * alpha );
            m_A[0] = (AA+1.0) + (AA-1.0) * cos_w0 + 2.0 * sqrt_AA * alpha;
            m_A[1] = -2.0*( (AA-1.0) + (AA+1.0) * cos_w0 );
            m_A[2] = (AA+1.0) + (AA-1.0) * cos_w0 - 2.0 * sqrt_AA * alpha;
            break;
        case BiQuadFilterType::HighShelf:
            m_B[0] = AA*( (AA+1.0) + (AA-1.0) * cos_w0 + 2.0 * sqrt_AA * alpha );
            m_B[1] = -2.0*AA*( (AA-1.0) + (AA+1.0) * cos_w0 );
            m_B[2] = AA*( (AA+1.0) + (AA-1.0) * cos_w0 - 2.0 * sqrt_AA * alpha );
            m_A[0] = (AA+1.0) - (AA-1.0) * cos_w0 + 2.0 * sqrt_AA * alpha;
            m_A[1] = 2.0*( (AA-1.0) - (AA+1.0) * cos_w0 );
            m_A[2] = (AA+1.0) - (AA-1.0) * cos_w0 - 2.0 * sqrt_AA * alpha;
            break;
        case BiQuadFilterType::None:
            m_B[0] = 1.0;
            m_B[1] = 0.0;
            m_B[2] = 0.0;
            m_A[0] = 1.0;
            m_A[1] = 0.0;
            m_A[2] = 0.0;
    }

    // normalize
    double norm = m_A[0];
    for (int i = 0; i < BQN; i++) {
        m_A[i] /= norm;
        m_B[i] /= norm;
    }
}
