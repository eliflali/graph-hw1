#include <cmath>
#include "parser.h"

using namespace parser;

float vectorLength(Vec3f vec)
{
    float length = sqrt(pow(vec.x,2) + pow(vec.y,2) + pow(vec.z,2));
    return length;
}

Vec3f normalize(const Vec3f &vec)
{
    Vec3f normalizedVector;
    float vectorLen = vectorLength(vec);

    normalizedVector.x = vec.x/vectorLen;
    normalizedVector.y = vec.y/vectorLen;
    normalizedVector.z = vec.z/vectorLen;

    return normalizedVector;
}

Vec3f determinant3(const Vec3f &vec1, const Vec3f &vec2)
{
    Vec3f determinant;
    determinant.x = vec1.y*vec1.z - vec1.z*vec2.y;
    determinant.y = vec1.z*vec2.x - vec1.x*vec2.z;
    determinant.z = vec1.x*vec2.y - vec1.y*vec2.x;
    return determinant;
}

Vec3f crossProduct(const Vec3f &vec1, const Vec3f &vec2)
{
    return determinant3(vec1, vec2);
}

float dotProduct(const Vec3f &vec1, const Vec3f &vec2)
{
    float product;
    product = vec1.x*vec2.x + vec1.y*vec2.y + vec1.z*vec2.z;
    return product;
}

Vec3f addVectors(const Vec3f &vec1, const Vec3f &vec2)
{
    Vec3f total;
    total.x = vec1.x+vec2.x;
    total.y = vec1.y+vec2.y;
    total.z = vec1.z+vec2.z;
    return total;
}

Vec3f subtractVectors(const Vec3f &vec1, const Vec3f &vec2)
{
    Vec3f total;
    total.x = vec1.x-vec2.x;
    total.y = vec1.y-vec2.y;
    total.z = vec1.z-vec2.z;
    return total;
}