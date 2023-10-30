#ifndef CODE_TEMPLATE_MATHFUNCTIONS_H
#define CODE_TEMPLATE_MATHFUNCTIONS_H

#include "parser.h"
using namespace parser;

namespace mathFunctions
{
    float vectorLength(Vec3f &vec);
    Vec3f normalize(const Vec3f &vec);

    Vec3f determinant3(const Vec3f &vec1, const Vec3f &vec2);
    Vec3f crossProduct(const Vec3f &vec1, const Vec3f &vec2);
    float dotProduct(const Vec3f &vec1, const Vec3f &vec2);

    Vec3f addVectors(const Vec3f &vec1, const Vec3f &vec2);
    Vec3f subtractVectors(const Vec3f &vec1, const Vec3f &vec2);

}
#endif //CODE_TEMPLATE_MATHFUNCTIONS_H
