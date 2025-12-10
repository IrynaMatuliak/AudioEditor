#include "cequalizereffect.h"

#include <qmath.h>

CEqualizerEffect::CEqualizerEffect()
{

}

void CEqualizerEffect::apply(QVector<float>& audioData, int sampleRate, int channels, qint64 startSample, qint64 processSamples, double lowGain, double midGain, double highGain)
{
    double lowGainLinear = pow(10.0, lowGain / 20.0);
    double midGainLinear = pow(10.0, midGain / 20.0);
    double highGainLinear = pow(10.0, highGain / 20.0);

    for (int ch = 0; ch < channels; ch++) {
        double lowX1 = 0.0, lowX2 = 0.0, lowY1 = 0.0, lowY2 = 0.0;
        double midX1 = 0.0, midX2 = 0.0, midY1 = 0.0, midY2 = 0.0;
        double highX1 = 0.0, highX2 = 0.0, highY1 = 0.0, highY2 = 0.0;

        for (qint64 i = startSample; i < startSample + processSamples; i++) {
            qint64 sampleIndex = i * channels + ch;

            if (sampleIndex >= audioData.size()) continue;

            double input = audioData[sampleIndex];

            input = applyBiquadFilter(input, lowX1, lowX2, lowY1, lowY2,
                                                 sampleRate, 250.0, lowGain, FilterType::LOW);

            input = applyBiquadFilter(input, midX1, midX2, midY1, midY2,
                                                 sampleRate, 1000.0, midGain, FilterType::MID);

            input = applyBiquadFilter(input, highX1, highX2, highY1, highY2,
                                                  sampleRate, 4000.0, highGain, FilterType::HIGH);

            //double output = (lowOutput * lowGainLinear) +
            //                (midOutput * midGainLinear) +
            //                (highOutput * highGainLinear);

            audioData[sampleIndex] = qBound(-1.0f, static_cast<float>(input), 1.0f);
        }
    }
}

double CEqualizerEffect::applyBiquadFilter(double input, double& x1, double& x2, double& y1, double& y2,
                                           int sampleRate, double frequency, double gain, FilterType type)
{
    double b0, b1, b2, a0, a1, a2; // coefficients of a biquad filter, describe its frequency response

    double omega = 2.0 * M_PI * frequency / sampleRate; // Normalized frequency in radians
    double sinOmega = sin(omega);
    double cosOmega = cos(omega);

    double A = pow(10.0, gain / 40.0); // gain on a linear scale
    double alpha = sinOmega / 2.0; // defines the filter bandwidth

    // low shelf filter
    if (type == FilterType::LOW) {
        double beta = 2.0 * sqrt(A) * alpha; // combines the effect of gain and bandwidth
        b0 = A * ((A + 1) - (A - 1) * cosOmega + beta);
        b1 = 2 * A * ((A - 1) - (A + 1) * cosOmega);
        b2 = A * ((A + 1) - (A - 1) * cosOmega - beta);
        a0 = (A + 1) + (A - 1) * cosOmega + beta;
        a1 = -2 * ((A - 1) + (A + 1) * cosOmega);
        a2 = (A + 1) + (A - 1) * cosOmega - beta;
    }
    // peak filter
    // Boosts/attenuates a narrow band of frequencies around the center
    else if (type == FilterType::MID) {
        double bandwidth = 1.0; // means the quality factor Q ≈ 1.41
        alpha = sinOmega * sinh(log(2.0) / 2.0 * bandwidth * omega / sinOmega); // sinh() is used to calculate the width of the band
        b0 = 1.0 + alpha * A;
        b1 = -2.0 * cosOmega;
        b2 = 1.0 - alpha * A;
        a0 = 1.0 + alpha / A;
        a1 = -2.0 * cosOmega;
        a2 = 1.0 - alpha / A;
    }
    // high shelf filter
    else if (type == FilterType::HIGH) {
        double beta = 2.0 * sqrt(A) * alpha;
        b0 = A * ((A + 1) + (A - 1) * cosOmega + beta);
        b1 = -2 * A * ((A - 1) + (A + 1) * cosOmega);
        b2 = A * ((A + 1) + (A - 1) * cosOmega - beta);
        a0 = (A + 1) - (A - 1) * cosOmega + beta;
        a1 = 2 * ((A - 1) - (A + 1) * cosOmega);
        a2 = (A + 1) - (A - 1) * cosOmega - beta;
    }
    else {
        return input;
    }
    b0 /= a0; b1 /= a0; b2 /= a0; // Normalization of coefficients
    a1 /= a0; a2 /= a0;
    double output = b0 * input + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2; // Calculating output using a recursive formula
    x2 = x1;
    x1 = input;
    y2 = y1;
    y1 = output;
    return output;
}
