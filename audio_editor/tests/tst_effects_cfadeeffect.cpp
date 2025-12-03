#include <QCoreApplication>
#include <QtTest>

#include "../AudioEffects/cfadeeffect.h"

class effects_CFadeEffect : public QObject
{
    Q_OBJECT

private slots:
    void test_resolveCurveName_linear();
    void test_resolveCurveName_scurve();
    void test_resolveCurveName_exponential();
    void test_resolveCurveName_logarithmic();
    void test_resolveCurveName_cosin();
};

void effects_CFadeEffect::test_resolveCurveName_linear()
{
    CFadeEffect::ECurveType curveType = CFadeEffect::resolve("linear");
    QCOMPARE(curveType, CFadeEffect::ECurveType::LINEAR);
}

void effects_CFadeEffect::test_resolveCurveName_scurve()
{
    CFadeEffect::ECurveType curveType = CFadeEffect::resolve("scurve");
    QCOMPARE(curveType, CFadeEffect::ECurveType::SCURVE);
}

void effects_CFadeEffect::test_resolveCurveName_exponential()
{
    CFadeEffect::ECurveType curveType = CFadeEffect::resolve("exponential");
    QCOMPARE(curveType, CFadeEffect::ECurveType::EXPONENTIAL);
}

void effects_CFadeEffect::test_resolveCurveName_logarithmic()
{
    CFadeEffect::ECurveType curveType = CFadeEffect::resolve("logarithmic");
    QCOMPARE(curveType, CFadeEffect::ECurveType::LOGARITHMIC);
}

void effects_CFadeEffect::test_resolveCurveName_cosin()
{
    CFadeEffect::ECurveType curveType = CFadeEffect::resolve("cosine");
    QCOMPARE(curveType, CFadeEffect::ECurveType::COSINE);
}

QTEST_MAIN(effects_CFadeEffect)

#include "tst_effects_cfadeeffect.moc"
