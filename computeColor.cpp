#include "computeColor.h"
#include "parser.h"
#include "mathFunctions.h"
#include "intersectCalculator.h"
#include <iostream>

using namespace parser;
using namespace mathFunctions;
using namespace intersectCalculator;

namespace computeColor
{
    Vec3f computeSphereNormal(const Vec3f &p, const Vec3f &c, const float &r)
    {
        Vec3f n = subtractVectors(p, c); // (p - c)
        n.x = n.x/r;
        n.y = n.y/r;
        n.z = n.z/r;
        return n;
    }

    Vec3f computeTriangleNormal(const Vec3f &a, const Vec3f &b, const Vec3f &c) // can be used for meshes
    {
        Vec3f ab = subtractVectors(b, a); // b-a
        Vec3f ac = subtractVectors(c, a); // c-a
        Vec3f n = crossProduct(ab, ac); // (b-a) x (c-a)
        n = normalize(n);
        return n;
    }

    Vec3f computeWi(const Vec3f &pointOnSurface, const Vec3f &lightPosition)  //point on surface is hit point
    {
        Vec3f wi = normalize(subtractVectors(lightPosition, pointOnSurface));
        return wi;
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

    Vec3f computeHalfVector(const Vec3f &wi, const Vec3f &wo)
    {
        Vec3f halfVector;
        halfVector = normalize(addVectors(wi,wo));
        return halfVector;
    }

    Vec3f computeAmbient(Vec3f ambientCoeff, Vec3f ambientLight) //ambientCoeff from Material struct in parser.h
    {
        Vec3f ambient;
        ambient.x = ambientCoeff.x * ambientLight.x; // k_a*I_a
        ambient.y = ambientCoeff.y * ambientLight.y; // k_a*I_a
        ambient.z = ambientCoeff.z * ambientLight.z; // k_a*I_a
        return ambient;
    }

    Vec3f computeSpecular(Vec3f specularCoeff, float phongExponent, const PointLight &light,const Ray &ray, const Vec3f &normal, const Vec3f &pointOnSurface) //Blinn-Phong
    {
        Vec3f specular;
        Vec3f wi = computeWi(pointOnSurface, light.position);
        Vec3f halfVector = computeHalfVector(wi, ray.direction);
        Vec3f irradiance = computeIrradiance(light.intensity, pointOnSurface, light.position);
        float cosalpha = std::max(0.0f, dotProduct(normal, halfVector)) ;
        float cosalphaPow = pow(cosalpha, phongExponent);
        specular.x = specularCoeff.x * cosalphaPow * irradiance.x;
        specular.y = specularCoeff.y * cosalphaPow * irradiance.y;
        specular.z = specularCoeff.z * cosalphaPow * irradiance.z;

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

    bool checkShadow(const PointLight &light, const Vec3f &pointOnSurface, const Scene &scene, const Vec3f &normal)
    {
        //s(t)= (x + n*epsilon) + t * w_i
        bool isShadow = false;
        float epsilon = scene.shadow_ray_epsilon;
        Vec3f wi = subtractVectors(light.position, pointOnSurface);
        float length = sqrt(pow(wi.x,2) + pow(wi.y,2) + pow(wi.z,2));
        Ray shadowRay;
        shadowRay.direction = normalize(wi); //w_i
        shadowRay.origin = addVectors(pointOnSurface, multiplyVector(normal, epsilon)); // x + n*epsilon

        HitPoint hitPoint;
        if(closestHit(0,shadowRay,hitPoint, scene))
        {
            if(hitPoint.time < length) // t<|wi|
            {
                isShadow = true;
            }
        }

        return isShadow;
    }

    Vec3f computePixelColor(Scene scene, Camera camera, Ray ray, int maxRecursion)
    {
        std::cout<<"computeColor 119"<<std::endl;
        Vec3f pixelColor;
        HitPoint hitPoint;
        std::vector<PointLight> point_lights = scene.point_lights;
        bool isHit = closestHit(0, ray, hitPoint, scene);
        if(isHit)
        {
            std::cout<<"hit"<<std::endl;
            int objectID = hitPoint.objectID;
            Vec3f ambientLight = scene.ambient_light;
            switch (objectID)
            {
                case 1: // 1->sphere 2-> triangle 3-> mesh
                {
                    Sphere sphere = scene.spheres[objectID];
                    Vec3f ambientCoeff = scene.materials[sphere.material_id].ambient;
                    Vec3f diffuseCoeff = scene.materials[sphere.material_id].diffuse;
                    Vec3f specularCoeff = scene.materials[sphere.material_id].specular;
                    float phongExponent = scene.materials[sphere.material_id].phong_exponent;
                    pixelColor = computeAmbient(ambientCoeff,ambientLight);
                    for(const auto &light : point_lights)
                    {
                        Vec3f normal = computeSphereNormal(hitPoint.point,
                                                           scene.vertex_data[sphere.center_vertex_id-1], // is it starting from 1?
                                                           sphere.radius);
                        bool isShadow = checkShadow(light, hitPoint.point, scene, normal);
                        if(!isShadow)
                        {
                            Vec3f diffuse = computeDiffuse(diffuseCoeff,light,normal,hitPoint.point);
                            Vec3f specular = computeSpecular(specularCoeff,phongExponent,light,ray,normal,hitPoint.point);
                            pixelColor.x = pixelColor.x + diffuse.x + specular.x;
                            pixelColor.y = pixelColor.y + diffuse.y + specular.y;
                            pixelColor.z = pixelColor.z + diffuse.z + specular.z;
                        }

                    }
                    break;
                }
                case 2: // triangle
                {
                    break;
                }
                case 3: // mesh
                {
                    break;
                }
                default:
                {
                    break;
                }
            }
        }
        else // pixelColor = background color;
        {
            pixelColor.x = scene.background_color.x;
            pixelColor.y = scene.background_color.y;
            pixelColor.z = scene.background_color.z;
        }
        std::cout<<"computeColor 178"<<std::endl;
        return pixelColor;
    }
}