#include "cfadeeffect.h"

CFadeEffect::ECurveType CFadeEffect::resolve(const QString& curveTypeName)
{
    ECurveType curveType = UNDEFINED;

    if (curveTypeName == "linear") {
        curveType = LINEAR;
    } else if (curveTypeName == "exponential") {
        curveType = EXPONENTIAL;
    } else if (curveTypeName == "logarithmic") {
        curveType = LOGARITHMIC;
    } else if (curveTypeName == "scurve") {
        curveType = SCURVE;
    } else if (curveTypeName == "cosine") {
        curveType = COSINE;
    }

    return curveType;
}
