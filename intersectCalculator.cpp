#include "parser.h"
#include "mathFunctions.h"
#include "rayGenerator.h"
#include "intersectCalculator.h"
#include <vector>

using namespace parser;
using namespace mathFunctions;
using namespace intersectCalculator;
using namespace rayGenerator;
using namespace std;

namespace intersectCalculator
{
    bool closestHit(float hitTime, Ray ray, HitPoint &hitFound , Scene scene){
        int sphereCount = scene.spheres.size() , meshCount = scene.meshes.size() , triangleCount = scene.triangles.size();
        bool isHit = false;
        float firstHit = 3.40282E38;
        HitPoint currentHit = {0,0,0,0.0F};

        /*
        1 sphere 2 tri 3 mesh
        Vec3f m = add(ray.o,multS(scene.vertex_data[sphere.center_vertex_id-1],-1));
        LEFT THIS HERE INTENTIONALLY CHEF
        */
        for(int i = 0; i < sphereCount; i++){
            //sphere case
            Vec3f originToCenter  = subtractVectors(scene.vertex_data[scene.spheres[i].center_vertex_id -1], ray.origin); //indexing starts from 1 in vertex data but this issue might be solved
            hitTime = quadraticDelta(scene.spheres[i],originToCenter,ray);
            if( (0 < hitTime) && (hitTime < firstHit ) ){
                firstHit = hitTime;
                currentHit = {i , 1 , 0 , hitTime};
                isHit = true;

            }
        }

        for(int i = 0; i < triangleCount;i++){
            //triangle case
            Face current = scene.triangles[i].indices;
            Vec3f vertex0 = scene.vertex_data[current.v0_id-1], vertex1 = scene.vertex_data[current.v1_id-1], vertex2 = scene.vertex_data[current.v2_id-1];
            Vec3f column0 = subtractVectors(vertex0,vertex1), column1 = subtractVectors(vertex0, vertex2), column2 = ray.direction;
            Vec3f columnBeta = subtractVectors(vertex0,ray.origin);

            float alpha = determinant3(column0,column1,column2);
            float beta = determinant3(columnBeta,column1,column2) / alpha;
            float gamma = determinant3(column0, columnBeta, column2) / alpha;
            float time = determinant3(column0, column1, columnBeta) / alpha;

            if((beta + gamma) <= 1 && beta >= 0 && gamma >= 0 ){
                if(time > 0 &&  time < hitTime){
                    currentHit = {i,2,0,hitTime};
                    isHit = true;
                }
            }
        }
        for(int i = 0; i < meshCount; i++){
            //mesh case
            int faceCount = scene.meshes[i].faces.size();
            for(int j =0 ; j < faceCount;j++){
                //for each face
                Face current = scene.meshes[i].faces[j];
                Vec3f vertex0 = scene.vertex_data[current.v0_id-1], vertex1 = scene.vertex_data[current.v1_id-1], vertex2 = scene.vertex_data[current.v2_id-1];
                Vec3f column0 = subtractVectors(vertex0,vertex1), column1 = subtractVectors(vertex0, vertex2), column2 = ray.direction;
                Vec3f columnBeta = subtractVectors(vertex0,ray.origin);

                float alpha = determinant3(column0,column1,column2);
                float beta = determinant3(columnBeta,column1,column2) / alpha;
                float gamma = determinant3(column0, columnBeta, column2) / alpha;
                float time = determinant3(column0, column1, columnBeta) / alpha;

                if((beta + gamma) <= 1 && beta >= 0 && gamma >= 0 ){
                    if(time > 0 &&  time < hitTime){
                        currentHit = {i,3,j,hitTime};
                        isHit = true;
                    }
                }
            }
        }
        hitFound = currentHit;
        return isHit;
    }
}


