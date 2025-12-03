#ifndef CECHOEFFECT_H
#define CECHOEFFECT_H

#include <QVector>

class CEchoEffect
{
public:
    CEchoEffect();

    void apply(QVector<float>& audioData, int sampleRate, int channels,
               qint64 startSample, qint64 processSamples,
               double delayMs, double decay, int repetitions);
};

#endif // CECHOEFFECT_H
