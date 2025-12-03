#ifndef CEQUALIZEREFFECT_H
#define CEQUALIZEREFFECT_H

#include <QVector>

class CEqualizerEffect
{
public:
    CEqualizerEffect();

    enum FilterType {
        LOW,
        MID,
        HIGH
    };

    double applyBiquadFilter(double input, double& x1, double& x2, double& y1, double& y2,
                             int sampleRate, double frequency, double gain, FilterType type);

    void apply(QVector<float>& audioData, int sampleRate, int channels,
               qint64 startSample, qint64 processSamples,
               double lowGain, double midGain, double highGain);

};

#endif // CEQUALIZEREFFECT_H
