#ifndef CNOISEREDUCTIONEFFECT_H
#define CNOISEREDUCTIONEFFECT_H

#include <QVector>
#include <vector>
#include <complex>

class CNoiseReductionEffect
{
public:
    CNoiseReductionEffect();

    void apply(QVector<float>& audioData, int sampleRate, int channels, qint64 startSample, qint64 processSamples,
               double noiseThreshold, double reductionStrength, double smoothing);

private:
    std::vector<float> spectralNoiseReduction(const std::vector<float>& input,
                                              int fftSize, int hopSize,
                                              double noiseThreshold,
                                              double reductionStrength,
                                              double smoothing);

    void fft(std::vector<std::complex<float>>& data, bool inverse = false);
    int nextPowerOfTwo(int n);
};

#endif // CNOISEREDUCTIONEFFECT_H
