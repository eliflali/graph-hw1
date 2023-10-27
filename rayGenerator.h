#ifndef CODE_TEMPLATE_RAYGENERATOR_H
#define CODE_TEMPLATE_RAYGENERATOR_H

#include "parser.h"
using namespace parser;

namespace rayGenerator
{
    Vec3f computeM(const Camera &camera);
    Vec3f computeQ(const Vec3f &m, const Vec3f &u, const Vec3f &v, const Camera &camera);
    Vec3f computeS(const Vec3f &q, const Vec3f &u, const Vec3f &v, float su, float sv);
}

#endif //CODE_TEMPLATE_RAYGENERATOR_H
