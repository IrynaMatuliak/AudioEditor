#include <QCoreApplication>
#include <QtTest>

#include "../AudioEffects/cfadeineffect.h"

class effects_CFadeInEffect : public QObject
{
    Q_OBJECT

    static constexpr double precision = 0.0001;
    double round(double value, double p = precision) {
        return std::round(value / p) * p;
    }

    const QVector<float> inputDataM = { 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f };
    const QVector<float> inputDataS = { 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f };

public:
    effects_CFadeInEffect() {};
    ~effects_CFadeInEffect() {};

private slots:

    void test_calculateFadeGain_linear();
    void test_calculateFadeGain_exponential();
    void test_calculateFadeGain_logarithmic();
    void test_calculateFadeGain_scurve();
    void test_calculateFadeGain_cosine();

    void test_calculateFadeGain_apply_mono();
    void test_calculateFadeGain_apply_stereo();
};

void effects_CFadeInEffect::test_calculateFadeGain_linear()
{
    double gain0 = CFadeInEffect().calculateFadeGain(0.0, CFadeEffect::LINEAR);
    QCOMPARE(round(gain0), 0.0);

    double gain1 = CFadeInEffect().calculateFadeGain(0.5, CFadeEffect::LINEAR);
    QCOMPARE(round(gain1), 0.5);

    double gain2 = CFadeInEffect().calculateFadeGain(1.0, CFadeEffect::LINEAR);
    QCOMPARE(round(gain2), 1.0);

    // out-of-bound test
    double gainx = CFadeInEffect().calculateFadeGain(2.0, CFadeEffect::LINEAR);
    QCOMPARE(round(gainx), 1.0);
}

void effects_CFadeInEffect::test_calculateFadeGain_exponential()
{
    double gain0 = CFadeInEffect().calculateFadeGain(0.0, CFadeEffect::EXPONENTIAL);
    QCOMPARE(round(gain0), 0.0);

    double gain1 = CFadeInEffect().calculateFadeGain(0.5, CFadeEffect::EXPONENTIAL);
    QCOMPARE(round(gain1), 0.25);

    double gain2 = CFadeInEffect().calculateFadeGain(1.0, CFadeEffect::EXPONENTIAL);
    QCOMPARE(round(gain2), 1.0);

    // out-of-bound test
    double gainx = CFadeInEffect().calculateFadeGain(2.0, CFadeEffect::EXPONENTIAL);
    QCOMPARE(round(gainx), 1.0);
}

void effects_CFadeInEffect::test_calculateFadeGain_logarithmic()
{
    double gain0 = CFadeInEffect().calculateFadeGain(0.0, CFadeEffect::LOGARITHMIC);
    QCOMPARE(round(gain0), 0.0);

    double gain1 = CFadeInEffect().calculateFadeGain(0.5, CFadeEffect::LOGARITHMIC);
    QCOMPARE(round(gain1), 0.7404);

    double gain2 = CFadeInEffect().calculateFadeGain(1.0, CFadeEffect::LOGARITHMIC);
    QCOMPARE(round(gain2), 1.0);

    // out-of-bound test
    double gainx = CFadeInEffect().calculateFadeGain(2.0, CFadeEffect::LOGARITHMIC);
    QCOMPARE(round(gainx), 1.0);
}

void effects_CFadeInEffect::test_calculateFadeGain_scurve()
{
    double gain0 = CFadeInEffect().calculateFadeGain(0.0, CFadeEffect::SCURVE);
    QCOMPARE(round(gain0), 0.0);

    double gain1 = CFadeInEffect().calculateFadeGain(0.3, CFadeEffect::SCURVE);
    QCOMPARE(round(gain1), 0.18);

    double gain2 = CFadeInEffect().calculateFadeGain(0.6, CFadeEffect::SCURVE);
    QCOMPARE(round(gain2), 0.68);

    double gain3 = CFadeInEffect().calculateFadeGain(1.0, CFadeEffect::SCURVE);
    QCOMPARE(round(gain3), 1.0);

    // out-of-bound test
    double gainx = CFadeInEffect().calculateFadeGain(2.0, CFadeEffect::SCURVE);
    QCOMPARE(round(gainx), 0.0);
}

void effects_CFadeInEffect::test_calculateFadeGain_cosine()
{
    double gain0 = CFadeInEffect().calculateFadeGain(0.0, CFadeEffect::COSINE);
    QCOMPARE(round(gain0), 0.0);

    double gain01 = CFadeInEffect().calculateFadeGain(0.3, CFadeEffect::COSINE);
    QCOMPARE(round(gain01), 0.2061);

    double gain1 = CFadeInEffect().calculateFadeGain(0.5, CFadeEffect::COSINE);
    QCOMPARE(round(gain1), 0.5);

    double gain11 = CFadeInEffect().calculateFadeGain(0.6, CFadeEffect::COSINE);
    QCOMPARE(round(gain11), 0.6545);

    double gain3 = CFadeInEffect().calculateFadeGain(1.0, CFadeEffect::COSINE);
    QCOMPARE(round(gain3), 1.0);

    // out-of-bound test
    double gainx = CFadeInEffect().calculateFadeGain(2.0, CFadeEffect::COSINE);
    QCOMPARE(round(gainx), 0.0);
}

void effects_CFadeInEffect::test_calculateFadeGain_apply_mono()
{
    QVector<float> inputData = inputDataM;
    const QVector<float> expected = { 0.0f, 0.0455, 0.0909, 0.1364, 0.1818, 0.2273, 0.2727, 0.3182, 0.3636, 0.4091, 0.4545 };

    CFadeInEffect().apply(inputData, 1, 0, inputData.size(), CFadeEffect::LINEAR);

    QCOMPARE(inputData.size(), expected.size());
    for (int i=0; i<inputData.size(); ++i) {
        QCOMPARE(round(inputData[i]), round(expected[i]));
    }
}

void effects_CFadeInEffect::test_calculateFadeGain_apply_stereo()
{
    QVector<float> inputData = inputDataS;
    const QVector<float> expected = { 0.0, 0.0, 0.0455, 0.0455, 0.0909, 0.0909, 0.1364, 0.1364, 0.1818, 0.1818, 0.2273, 0.2273, 0.2727, 0.2727, 0.3182, 0.3182, 0.3636, 0.3636, 0.4091, 0.4091, 0.4545, 0.4545 };

    CFadeInEffect().apply(inputData, 2, 0, inputData.size()/2, CFadeEffect::LINEAR);

    QCOMPARE(inputData.size(), expected.size());
    for (int i=0; i<inputData.size(); ++i) {
        QCOMPARE(round(inputData[i]), round(expected[i]));
    }
}

QTEST_MAIN(effects_CFadeInEffect)

#include "tst_effects_cfadeineffect.moc"
