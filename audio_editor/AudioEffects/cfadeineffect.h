#ifndef CFADEINEFFECT_H
#define CFADEINEFFECT_H

#include <QVector>
#include "cfadeeffect.h"

class CFadeInEffect : public CFadeEffect
{
public:

    CFadeInEffect();

    void apply(QVector<float>& audioData, int channels, qint64 startSample, qint64 fadeSamples, ECurveType curveType) override;

    double calculateFadeGain(double progress, ECurveType curveType);

};

#endif // CFADEINEFFECT_H
