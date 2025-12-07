#include "cnoisereductioneffect.h"
#include <vector>
#include <complex>
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

CNoiseReductionEffect::CNoiseReductionEffect()
{
}

int CNoiseReductionEffect::nextPowerOfTwo(int n) {
    int power = 1;
    while (power < n) {
        power <<= 1;
    }
    return power;
}

void CNoiseReductionEffect::fft(std::vector<std::complex<float>>& data, bool inverse)
{
    int n = data.size();
    if (n <= 1) return;

    std::vector<std::complex<float>> even(n/2), odd(n/2);
    for (int i = 0; i < n/2; i++) {
        even[i] = data[i*2];
        odd[i] = data[i*2 + 1];
    }

    fft(even, inverse);
    fft(odd, inverse);

    float angle = 2 * M_PI / n * (inverse ? -1 : 1);
    std::complex<float> w(1), wn(std::cos(angle), std::sin(angle));

    for (int i = 0; i < n/2; i++) {
        data[i] = even[i] + w * odd[i];
        data[i + n/2] = even[i] - w * odd[i];
        if (inverse) {
            data[i] /= 2;
            data[i + n/2] /= 2;
        }
        w *= wn;
    }
}

std::vector<float> CNoiseReductionEffect::spectralNoiseReduction(const std::vector<float>& input,
                                                                 int fftSize, int hopSize,
                                                                 double noiseThreshold,
                                                                 double reductionStrength,
                                                                 double smoothing)
{
    std::vector<float> output(input.size(), 0.0f);
    std::vector<float> window(fftSize, 0.0f);

    for (int i = 0; i < fftSize; i++) {
        window[i] = 0.5f * (1.0f - std::cos(2.0f * M_PI * i / (fftSize - 1)));
    }

    std::vector<float> noiseMagnitudeProfile(fftSize/2 + 1, 0.0f);
    int noiseAnalysisFrames = std::min(10, static_cast<int>(input.size() / hopSize));

    for (int frame = 0; frame < noiseAnalysisFrames; frame++) {
        int start = frame * hopSize;
        if (start + fftSize > input.size()) break;

        std::vector<std::complex<float>> frameData(fftSize);
        for (int i = 0; i < fftSize; i++) {
            frameData[i] = input[start + i] * window[i];
        }

        fft(frameData);

        for (int i = 0; i <= fftSize/2; i++) {
            float magnitude = std::abs(frameData[i]);
            noiseMagnitudeProfile[i] = (noiseMagnitudeProfile[i] * frame + magnitude) / (frame + 1);
        }
    }

    std::vector<std::vector<std::complex<float>>> processedFrames;

    for (int start = 0; start + fftSize <= input.size(); start += hopSize) {
        std::vector<std::complex<float>> frameData(fftSize);
        for (int i = 0; i < fftSize; i++) {
            frameData[i] = input[start + i] * window[i];
        }

        fft(frameData);

        std::vector<std::complex<float>> processedFrame(fftSize);

        for (int i = 0; i < fftSize; i++) {
            float magnitude = std::abs(frameData[i]);
            float phase = std::arg(frameData[i]);

            int bin = (i <= fftSize/2) ? i : fftSize - i;
            float noiseMagnitude = noiseMagnitudeProfile[bin];

            float snr = 0.0f;
            if (noiseMagnitude > 0.0f) {
                snr = magnitude / noiseMagnitude;
            }

            float gain = 1.0f;
            if (snr < static_cast<float>(noiseThreshold)) {
                float attenuation = static_cast<float>(reductionStrength) *
                                    (1.0f - snr / static_cast<float>(noiseThreshold));
                gain = 1.0f - attenuation;
                gain = std::max<float>(0.0f, std::min<float>(1.0f, gain));
            }

            if (smoothing > 0 && i > 0 && i < fftSize - 1) {
                float prevGain = 1.0f, nextGain = 1.0f;

                int prevBin = (i-1 <= fftSize/2) ? i-1 : fftSize - (i-1);
                int nextBin = (i+1 <= fftSize/2) ? i+1 : fftSize - (i+1);

                float prevSNR = (noiseMagnitudeProfile[prevBin] > 0) ?
                                    std::abs(frameData[i-1]) / noiseMagnitudeProfile[prevBin] : 1.0f;
                float nextSNR = (noiseMagnitudeProfile[nextBin] > 0) ?
                                    std::abs(frameData[i+1]) / noiseMagnitudeProfile[nextBin] : 1.0f;

                if (prevSNR < static_cast<float>(noiseThreshold)) {
                    prevGain = 1.0f - static_cast<float>(reductionStrength) *
                                          (1.0f - prevSNR / static_cast<float>(noiseThreshold));
                }
                if (nextSNR < static_cast<float>(noiseThreshold)) {
                    nextGain = 1.0f - static_cast<float>(reductionStrength) *
                                          (1.0f - nextSNR / static_cast<float>(noiseThreshold));
                }

                gain = (1.0f - static_cast<float>(smoothing)) * gain +
                       static_cast<float>(smoothing) * 0.5f * (prevGain + nextGain);
            }

            float newMagnitude = magnitude * gain;
            processedFrame[i] = std::polar(newMagnitude, phase);
        }

        processedFrames.push_back(processedFrame);
    }

    std::vector<float> synthesis(input.size(), 0.0f);
    std::vector<float> overlapWeights(input.size(), 0.0f);

    for (size_t frameIdx = 0; frameIdx < processedFrames.size(); frameIdx++) {
        int start = frameIdx * hopSize;

        std::vector<std::complex<float>> frameData = processedFrames[frameIdx];
        fft(frameData, true);

        for (int i = 0; i < fftSize && start + i < synthesis.size(); i++) {
            synthesis[start + i] += std::real(frameData[i]) * window[i];
            overlapWeights[start + i] += window[i] * window[i];
        }
    }

    for (size_t i = 0; i < synthesis.size(); i++) {
        if (overlapWeights[i] > 0.0f) {
            output[i] = synthesis[i] / overlapWeights[i];
        }
    }

    return output;
}

void CNoiseReductionEffect::apply(QVector<float>& audioData, int sampleRate, int channels, qint64 startSample, qint64 processSamples,
                                  double noiseThreshold, double reductionStrength, double smoothing)
{
    const int FFT_SIZE = 1024;
    const int HOP_SIZE = FFT_SIZE / 4;

    for (int ch = 0; ch < channels; ch++) {
        std::vector<float> channelData;
        channelData.reserve(processSamples);

        for (qint64 i = startSample; i < startSample + processSamples; i++) {
            qint64 index = i * channels + ch;
            if (index < audioData.size()) {
                channelData.push_back(audioData[index]);
            }
        }

        if (channelData.empty()) continue;

        std::vector<float> processedData = spectralNoiseReduction(channelData, FFT_SIZE, HOP_SIZE,
                                                                  noiseThreshold, reductionStrength, smoothing);

        for (size_t i = 0; i < processedData.size() && i < static_cast<size_t>(processSamples); i++) {
            qint64 index = (startSample + i) * channels + ch;
            if (index < audioData.size()) {
                audioData[index] = processedData[i];
            }
        }
    }
}
