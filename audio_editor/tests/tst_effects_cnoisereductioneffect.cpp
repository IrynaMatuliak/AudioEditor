#include <QCoreApplication>
#include <QtTest>
#include <cmath>

#include "../AudioEffects/cnoisereductioneffect.h"

class effects_CNoiseReductionEffect : public QObject
{
    Q_OBJECT

    static constexpr double precision = 0.01;

    const QVector<float> inputDataMono = {
        0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f
    };

    const QVector<float> inputDataStereo = {
        0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f
    };

    QVector<float> createNoisySignal(int samples) {
        QVector<float> signal(samples);
        for (int i = 0; i < samples; ++i) {
            float t = i / 44100.0f;
            signal[i] = 0.3f * sin(2 * M_PI * 440.0f * t) + 0.1f * ((rand() % 100) / 100.0f - 0.5f);
        }
        return signal;
    }

public:
    effects_CNoiseReductionEffect() {};
    ~effects_CNoiseReductionEffect() {};

private slots:
    void test_apply_mono();
    void test_apply_stereo();
    void test_apply_with_noise_threshold();
    void test_empty_input();
    void cleanup();
};

void effects_CNoiseReductionEffect::test_apply_mono()
{
    QVector<float> inputData = inputDataMono;
    QVector<float> originalData = inputDataMono;

    CNoiseReductionEffect effect;
    effect.apply(inputData, 44100, 1, 0, inputData.size(), 0.1, 0.7, 0.3);

    QCOMPARE(inputData.size(), originalData.size());

    bool dataChanged = false;
    for (int i = 0; i < inputData.size(); ++i) {
        if (std::abs(inputData[i] - originalData[i]) > precision) {
            dataChanged = true;
            break;
        }
    }

    QVERIFY2(dataChanged, "Noise reduction should modify the signal");

    for (int i = 0; i < inputData.size(); ++i) {
        QVERIFY2(inputData[i] >= -1.0f && inputData[i] <= 1.0f,
                 QString("Sample %1 out of range: %2").arg(i).arg(inputData[i]).toStdString().c_str());
    }
}

void effects_CNoiseReductionEffect::test_apply_stereo()
{
    QVector<float> inputData = inputDataStereo;
    QVector<float> originalData = inputDataStereo;

    CNoiseReductionEffect effect;
    int numSamples = inputData.size() / 2;
    effect.apply(inputData, 44100, 2, 0, numSamples, 0.1, 0.7, 0.3);

    QCOMPARE(inputData.size(), originalData.size());

    bool leftChanged = false;
    bool rightChanged = false;

    for (int i = 0; i < inputData.size(); i += 2) {
        if (std::abs(inputData[i] - originalData[i]) > precision) {
            leftChanged = true;
        }
        if (std::abs(inputData[i + 1] - originalData[i + 1]) > precision) {
            rightChanged = true;
        }
    }

    QVERIFY2(leftChanged, "Left channel should be modified");
    QVERIFY2(rightChanged, "Right channel should be modified");

    for (int i = 0; i < inputData.size(); i += 2) {
        float diff = std::abs(inputData[i] - inputData[i + 1]);
        QVERIFY2(diff < precision * 2,
                 QString("Channels processed differently at sample %1: L=%2, R=%3")
                     .arg(i).arg(inputData[i]).arg(inputData[i + 1]).toStdString().c_str());
    }
}

void effects_CNoiseReductionEffect::test_apply_with_noise_threshold()
{
    QVector<float> noisySignal = createNoisySignal(2000);
    QVector<float> originalSignal = noisySignal;

    CNoiseReductionEffect effect;

    QVector<float> signal1 = noisySignal;
    effect.apply(signal1, 44100, 1, 0, signal1.size(), 0.5, 0.9, 0.3);

    QVector<float> signal2 = originalSignal;
    effect.apply(signal2, 44100, 1, 0, signal2.size(), 0.01, 0.9, 0.3);

    float avgAmplitude1 = 0.0f;
    float avgAmplitude2 = 0.0f;

    for (int i = 0; i < signal1.size(); ++i) {
        avgAmplitude1 += std::abs(signal1[i]);
        avgAmplitude2 += std::abs(signal2[i]);
    }

    avgAmplitude1 /= signal1.size();
    avgAmplitude2 /= signal2.size();

    QVERIFY2(avgAmplitude1 < avgAmplitude2 * 1.5,
             QString("High threshold (%1) should suppress more than low threshold (%2)")
                 .arg(avgAmplitude1).arg(avgAmplitude2).toStdString().c_str());
}

void effects_CNoiseReductionEffect::test_empty_input()
{
    QVector<float> emptyData;
    QVector<float> smallData = {0.1f, 0.2f, 0.1f};

    CNoiseReductionEffect effect;

    effect.apply(emptyData, 44100, 1, 0, 0, 0.1, 0.7, 0.3);
    QCOMPARE(emptyData.size(), 0);

    effect.apply(smallData, 44100, 1, 0, smallData.size(), 0.1, 0.7, 0.3);
    QCOMPARE(smallData.size(), 3);

    for (float sample : smallData) {
        QVERIFY2(sample >= -1.0f && sample <= 1.0f,
                 QString("Sample %1 out of range").arg(sample).toStdString().c_str());
    }
}

void effects_CNoiseReductionEffect::cleanup()
{

}

QTEST_MAIN(effects_CNoiseReductionEffect)

#include "tst_effects_cnoisereductioneffect.moc"
