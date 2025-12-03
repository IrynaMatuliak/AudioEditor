#include "cfadeouteffect.h"

#include <qmath.h>

CFadeOutEffect::CFadeOutEffect()
{

}

void CFadeOutEffect::apply(QVector<float>& audioData, int channels, qint64 startSample, qint64 fadeSamples, ECurveType curveType)
{
    for (qint64 i = 0; i < fadeSamples; i++) {
        double progress = static_cast<double>(i) / fadeSamples;
        double gain = calculateFadeGain(progress, curveType);

        for (int ch = 0; ch < channels; ch++) {
            qint64 index = (startSample + i) * channels + ch;
            if (index < audioData.size()) {
                audioData[index] *= static_cast<float>(gain);
            }
        }
    }
}

double CFadeOutEffect::calculateFadeGain(double progress, ECurveType curveType)
{
    double gain = progress;

    switch (curveType) {
    case ECurveType::LINEAR:
        gain = 1.0 - progress;
        break;
    case ECurveType::EXPONENTIAL:
        gain = (1.0 - progress) * (1.0 - progress);
        break;
    case ECurveType::LOGARITHMIC:
        gain = 1.0 - log10(1 + progress * 9);
        break;
    case ECurveType::SCURVE:
        gain = progress < 0.5 ? 1 - 2 * progress * progress : 2 * (1 - progress) * (1 - progress);
        break;
    case ECurveType::COSINE:
        gain = (1 + cos(progress * M_PI)) / 2;
        break;
    default:
        break;
    }

    return qBound(0.0, gain, 1.0);
}
