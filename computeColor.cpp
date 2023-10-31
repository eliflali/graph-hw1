#include "computeColor.h"
#include "parser.h"
#include "mathFunctions.h"

using namespace parser;
using namespace mathFunctions;

namespace computeColor
{
    Vec3f computeWi(const Vec3f &pointOnSurface, const Vec3f &lightPosition)  //point on surface is hit point
    {
        Vec3f lightDir = normalize(subtractVectors(lightPosition, pointOnSurface));
        return lightDir;
    }

    Vec3f computeIrradiance(const Vec3f &intensity, const Vec3f &pointOnSurface, const Vec3f &lightPosition) // I(intensity)/r^2
    {
        Vec3f irradiance;
        float r = computeDistance(pointOnSurface, lightPosition);

        irradiance.x = intensity.x/(pow(r,2));
        irradiance.y = intensity.y/(pow(r,2));
        irradiance.z = intensity.z/(pow(r,2));

        return irradiance;
    }

    Vec3f computeAmbient(Vec3f ambientCoeff, Vec3f ambientLight) //ambientCoeff from Material struct in parser.h
    {
        Vec3f ambient;
        ambient.x = ambientCoeff.x * ambientLight.x; // k_a*I_a
        ambient.y = ambientCoeff.y * ambientLight.y; // k_a*I_a
        ambient.z = ambientCoeff.z * ambientLight.z; // k_a*I_a
        return ambient;
    }

    Vec3f computeHalfVector(const Vec3f &wi, const Vec3f &wo)
    {
        Vec3f halfVector;
        halfVector = normalize(addVectors(wi,wo));
        return halfVector;
    }

    Vec3f computeSpecular(Vec3f specularCoeff, const PointLight &light, const Vec3f &normal, const Vec3f &wi) //Blinn-Phong
    {
        Vec3f specular;
        Vec3f halfVector = computeHalfVector(wi, wo); // what's wo??
        float cosalpha = std::max(0.0f, dotProduct(normal, halfVector)) ;
        return specular;
    }

    Vec3f computeDiffuse(Vec3f diffuseCoeff, const PointLight &light, const Vec3f &normal, const Vec3f &pointOnSurface)
    {
        Vec3f diffuse;
        Vec3f irradiance = computeIrradiance(light.intensity, pointOnSurface, light.position);
        Vec3f wi = computeWi(pointOnSurface, light.position);
        float costheta = std::max(0.0f, dotProduct(normal, wi)); // cos(theta)' = max(0, w_i.n)

        diffuse.x = diffuseCoeff.x * costheta * irradiance.x;
        diffuse.y = diffuseCoeff.y * costheta * irradiance.y;
        diffuse.z = diffuseCoeff.z * costheta * irradiance.z;

        return diffuse;
    }
}