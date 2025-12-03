#ifndef CFADEEFFECT_H
#define CFADEEFFECT_H

#include <QVector>

class CFadeEffect
{
public:
    enum ECurveType {
        LINEAR,
        EXPONENTIAL,
        LOGARITHMIC,
        SCURVE,
        COSINE,
        UNDEFINED
    };

    virtual ~CFadeEffect() = default;

    virtual void apply(QVector<float>& audioData, int channels, qint64 startSample, qint64 fadeSamples, ECurveType curveType) = 0;

    static ECurveType resolve(const QString& curveTypeName);
};

#endif // CFADEEFFECT_H
