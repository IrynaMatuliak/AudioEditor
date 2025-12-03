#ifndef CFADEOUTEFFECT_H
#define CFADEOUTEFFECT_H

#include <QVector>

#include "cfadeeffect.h"

class CFadeOutEffect : public CFadeEffect
{
public:

    CFadeOutEffect();

    void apply(QVector<float>& audioData, int channels, qint64 startSample, qint64 fadeSamples, ECurveType curveType) override;

    double calculateFadeGain(double progress, ECurveType curveType);

};

#endif // CFADEOUTEFFECT_H
