#include "computeColor.h"
#include "parser.h"
#include "mathFunctions.h"
#include "intersectCalculator.h"
#include <iostream>
#include <math.h>

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
        Vec3f wi = normalize(subtractVectors(lightPosition,pointOnSurface));
        return wi;
    }

    Vec3f computeWo(const Vec3f &x, const Vec3f &pointOnSurface)
    {
        Vec3f wo= normalize(subtractVectors(x,pointOnSurface));
        return wo;
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
        halfVector = normalize(addVectors(wi,normalize(wo)));
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
        Vec3f wo= computeWo(ray.origin,pointOnSurface);
        Vec3f halfVector = computeHalfVector(wi, wo);
        Vec3f irradiance = computeIrradiance(light.intensity, pointOnSurface, light.position);
        float cosalpha = std::max(0.0f, dotProduct(normal, halfVector)) ;
        if(cosalpha>1)
        {
            cosalpha = 1;
        }
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
        if(closestHit(shadowRay,hitPoint, scene))
        {
            if(hitPoint.time  < length) // t<|wi|
            {
                isShadow = true;
            }
        }

        return isShadow;
    }

    /*
    
x:
-8.05607224
y:
-4.52776432
z:
-2.87845039


x:
-8.03443527
y:
-4.50485754
z:
-2.86534524
*/

    bool isReflective(Material material)
    {
        bool reflective;

        reflective = material.is_mirror ? true : false;

        return reflective;
    }

    Ray computeReflectionRay(const Ray &ray, const Vec3f &n, const Vec3f &pointOnSurface, const float &shadowEpsilon)
    {
        //w_r = -w_o + 2*n*costheta = -w_o + 2*n(dotproduct(n.w_o))
        //k_m*L_i(x,w_r)
        Vec3f wo= computeWo(ray.origin,pointOnSurface);
        Ray reflectionRay;
        float dotpNWo = dotProduct(wo,n);
        Vec3f multiplied = multiplyVector(n, 2*dotpNWo);
        reflectionRay.direction = normalize(subtractVectors(multiplied, wo));
        reflectionRay.origin = addVectors(pointOnSurface, multiplyVector(n, 2*shadowEpsilon));
        return reflectionRay;
    }

    Vec3f computePixelColor(Scene scene, Camera camera, Ray ray, int maxRecursion, int ignoreObjectID, int ignoreObjectType)
    {
        Vec3f pixelColor;
        HitPoint hitPoint;
        std::vector<PointLight> point_lights = scene.point_lights;
        bool isHit = closestHit(ray, hitPoint, scene, ignoreObjectID);
        if(isHit && hitPoint.objectID != ignoreObjectID)
        {
            int objectType = hitPoint.objectType;
            int objectID = hitPoint.objectID;
            Vec3f ambientLight = scene.ambient_light;
            switch (objectType)
            {
                case 1: // 1->sphere 2-> triangle 3-> mesh
                {
                    Sphere sphere = scene.spheres[objectID];
                    Material material = scene.materials[sphere.material_id-1];
                    Vec3f ambientCoeff = material.ambient;
                    Vec3f diffuseCoeff = material.diffuse;
                    Vec3f specularCoeff = material.specular;
                    Vec3f mirror = material.mirror;
                    float phongExponent = material.phong_exponent;

                    Vec3f normal = computeSphereNormal(hitPoint.point,
                                                       scene.vertex_data[sphere.center_vertex_id-1], // it is starting from 1
                                                       sphere.radius);
                    pixelColor = computeAmbient(ambientCoeff,ambientLight);

                    for(const auto &light : point_lights)
                    {

                        bool isShadow = checkShadow(light, hitPoint.point, scene, normal);
                        if(!isShadow)
                        {
                            Vec3f diffuse = computeDiffuse(diffuseCoeff,light,normal,hitPoint.point);
                            Vec3f specular = computeSpecular(specularCoeff,phongExponent,light,ray,normal,hitPoint.point);
                            pixelColor = addVectors(pixelColor, diffuse);
                            pixelColor = addVectors(pixelColor, specular);
                        }

                    }
                    if(maxRecursion>0 && isReflective(scene.materials[sphere.material_id-1]))
                    {
                        Ray reflectionRay;
                        reflectionRay = computeReflectionRay(ray, normal, hitPoint.point, scene.shadow_ray_epsilon);
                        Vec3f reflectedColor = computePixelColor(scene, camera,reflectionRay ,--maxRecursion, objectID, objectType);
                        pixelColor.x += reflectedColor.x * mirror.x;
                        pixelColor.y += reflectedColor.y * mirror.y;
                        pixelColor.z += reflectedColor.z * mirror.z;
                    }

                    break;
                }
                case 2: // triangle
                {
                    Triangle triangle = scene.triangles[objectID];
                    Material material  = scene.materials[triangle.material_id-1];
                    Vec3f ambientCoeff = material.ambient;
                    Vec3f diffuseCoeff = material.diffuse;
                    Vec3f specularCoeff = material.specular;
                    float phongExponent = material.phong_exponent;
                    Vec3f mirror = material.mirror;
                    pixelColor = computeAmbient(ambientCoeff,ambientLight);
                    Vec3f normal = computeTriangleNormal(scene.vertex_data[triangle.indices.v0_id-1],
                                                         scene.vertex_data[triangle.indices.v1_id-1], // it is starting from 1
                                                         scene.vertex_data[triangle.indices.v2_id-1]);
                    for(const auto &light : point_lights)
                    {

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
                    if(maxRecursion>0 && isReflective(scene.materials[triangle.material_id-1]))
                    {
                        Ray reflectionRay;
                        reflectionRay = computeReflectionRay(ray, normal, hitPoint.point, scene.shadow_ray_epsilon);
                        Vec3f reflectedColor = computePixelColor(scene, camera,reflectionRay ,--maxRecursion, objectID, objectType);
                        pixelColor.x += reflectedColor.x * mirror.x;
                        pixelColor.y += reflectedColor.y * mirror.y;
                        pixelColor.z += reflectedColor.z * mirror.z;
                    }
                    break;
                }
                case 3: // mesh
                {
                    Mesh mesh = scene.meshes[objectID];
                    Face face = mesh.faces[hitPoint.faceID];
                    Material material = scene.materials[mesh.material_id-1];
                    Vec3f ambientCoeff = material.ambient;
                    Vec3f diffuseCoeff = material.diffuse;
                    Vec3f specularCoeff = material.specular;
                    float phongExponent = material.phong_exponent;
                    Vec3f mirror = material.mirror;
                    pixelColor = computeAmbient(ambientCoeff,ambientLight);
                    Vec3f normal = computeTriangleNormal(scene.vertex_data[face.v0_id-1],
                                                         scene.vertex_data[face.v1_id-1], // it is starting from 1
                                                         scene.vertex_data[face.v2_id-1]);

                    for(const auto &light : point_lights)
                    {
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
                    if(maxRecursion>0 && isReflective(material))
                    {
                        Ray reflectionRay;
                        reflectionRay = computeReflectionRay(ray, normal, hitPoint.point, scene.shadow_ray_epsilon);
                        Vec3f reflectedColor = computePixelColor(scene, camera,reflectionRay ,--maxRecursion, objectID,objectType);
                        pixelColor.x += reflectedColor.x * mirror.x;
                        pixelColor.y += reflectedColor.y * mirror.y;
                        pixelColor.z += reflectedColor.z * mirror.z;
                    }
                    /*if(pixelColor.x == 0 && pixelColor.y == 0 && pixelColor.z ==0){
                        pixelColor = {35,145,89};
                    }*/
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
            /*if(pixelColor.x == 0 && pixelColor.y == 0 && pixelColor.z ==0){
                pixelColor = {35,145,89};
            }*/
        }
        
        return pixelColor;
    }
}