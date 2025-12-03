#include <QCoreApplication>
#include <QtTest>

#include "../AudioEffects/cechoeffect.h"

class effects_CEchoEffect : public QObject
{
    Q_OBJECT

    static constexpr double precision = 0.0001;
    double round(double value, double p = precision) {
        return std::round(value / p) * p;
    }

    const QVector<float> inputDataM = { 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f };
    const QVector<float> inputDataS = { 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f };

public:
    effects_CEchoEffect() {};
    ~effects_CEchoEffect() {};

private slots:

    void test_calculateFadeGain_apply_mono();
    void test_calculateFadeGain_apply_stereo();
};

void effects_CEchoEffect::test_calculateFadeGain_apply_mono()
{
    QVector<float> inputData = inputDataM;
    const QVector<float> expected = { 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5 };

    CEchoEffect().apply(inputData, 44100, 1, 0, inputData.size(), 1.0, 0.5, 3);

    QCOMPARE(inputData.size(), expected.size());
    for (int i=0; i<inputData.size(); ++i) {
        QCOMPARE(round(inputData[i]), round(expected[i]));
    }
}

void effects_CEchoEffect::test_calculateFadeGain_apply_stereo()
{
    QVector<float> inputData = inputDataS;
    const QVector<float> expected = { 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5 };

    CEchoEffect().apply(inputData, 44100, 2, 0, inputData.size()/2, 1.0, 0.5, 3);

    QCOMPARE(inputData.size(), expected.size());
    for (int i=0; i<inputData.size(); ++i) {
        QCOMPARE(round(inputData[i]), round(expected[i]));
    }
}

QTEST_MAIN(effects_CEchoEffect)

#include "tst_effects_cechoeffect.moc"
