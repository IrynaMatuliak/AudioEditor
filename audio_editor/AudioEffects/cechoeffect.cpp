#include "cechoeffect.h"

CEchoEffect::CEchoEffect()
{

}

void CEchoEffect::apply(QVector<float>& audioData, int sampleRate, int channels, qint64 startSample, qint64 processSamples, double delayMs, double decay, int repetitions)
{
    int delaySamples = static_cast<int>((delayMs * sampleRate) / 1000.0);

    if (delaySamples <= 0) {
        delaySamples = static_cast<int>(0.4 * sampleRate);
    }
    if (repetitions <= 0) {
        repetitions = 4;
    }

    QVector<float> outputData = audioData;

    for (int ch = 0; ch < channels; ch++) {
        QVector<float> delayBuffer(delaySamples * repetitions + 1000, 0.0f);
        int writePos = 0;

        for (qint64 i = startSample; i < startSample + processSamples; i++) {
            qint64 idx = i * channels + ch;
            float inputSample = audioData[idx];
            float outputSample = inputSample;

            for (int rep = 1; rep <= repetitions; rep++) {
                int readPos = (writePos - delaySamples * rep + delayBuffer.size()) % delayBuffer.size();
                float echoSample = delayBuffer[readPos] * static_cast<float>(pow(decay, rep));
                outputSample += echoSample;
            }
            outputSample = qBound(-1.0f, outputSample, 1.0f);
            outputData[idx] = outputSample;
            delayBuffer[writePos] = outputSample;
            writePos = (writePos + 1) % delayBuffer.size();
        }
    }
    audioData = outputData;
}
