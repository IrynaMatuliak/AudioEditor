#include <QCoreApplication>
#include <QtTest>

#include "../AudioEffects/cequalizereffect.h"

class effects_CEqualizerEffect : public QObject
{
    Q_OBJECT

    static constexpr double precision = 0.0001;
    double round(double value, double p = precision) {
        return std::round(value / p) * p;
    }

    const QVector<float> inputDataM = { 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f };
    const QVector<float> inputDataS = { 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f };

public:
    effects_CEqualizerEffect() {};
    ~effects_CEqualizerEffect() {};

private slots:

    void test_apply_mono();
    void test_apply_stereo();
};

void effects_CEqualizerEffect::test_apply_mono()
{
    QVector<float> inputData = inputDataM;
    const QVector<float> expected = { 0.5596, 0.5426, 0.5271, 0.5178, 0.5156, 0.5189, 0.5253, 0.5324, 0.5384, 0.5426, 0.5449 };

    CEqualizerEffect().apply(inputData, 44100, 1, 0, inputData.size(), 1.1, 1.1, 1.1);

    QCOMPARE(inputData.size(), expected.size());
    for (int i=0; i<inputData.size(); ++i) {
        QCOMPARE(round(inputData[i]), round(expected[i]));
    }
}

void effects_CEqualizerEffect::test_apply_stereo()
{
    QVector<float> inputData = inputDataS;
    const QVector<float> expected = { 0.5596, 0.5596, 0.5426, 0.5426, 0.5271, 0.5271, 0.5178, 0.5178, 0.5156, 0.5156, 0.5189, 0.5189, 0.5253, 0.5253, 0.5324, 0.5324, 0.5384, 0.5384, 0.5426, 0.5426, 0.5449, 0.5449 };

    CEqualizerEffect().apply(inputData, 44100, 2, 0, inputData.size()/2, 1.1, 1.1, 1.1);

    QCOMPARE(inputData.size(), expected.size());
    for (int i=0; i<inputData.size(); ++i) {
        QCOMPARE(round(inputData[i]), round(expected[i]));
    }
}

QTEST_MAIN(effects_CEqualizerEffect)

#include "tst_effects_cequalizereffect.moc"
