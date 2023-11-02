#include <iostream>
#include "parser.h"
#include "ppm.h"
#include "rayGenerator.h"
#include <algorithm>
#include "computeColor.h"

using namespace rayGenerator;
using namespace parser;
using namespace computeColor;

int max_recursion_depth;
std::vector<Camera> cameras;

float clamp(float value)
{
    float bounded = value > 255 ? 255 : round(value);
    return bounded;
}

int main(int argc, char* argv[])
{
    // Sample usage for reading an XML scene file
    parser::Scene scene;
    scene.loadFromXml(argv[1]);
    max_recursion_depth = scene.max_recursion_depth;
    cameras = scene.cameras;

    //for every camera angle, we must have a distinct ppm.
    for(int i = 0; i<cameras.size(); i++)
    {
        Camera camera = cameras[i];
        int width = camera.image_width;
        int height = camera.image_height;
        unsigned char* image = new unsigned char [width * height * 3];

        int pixel = 0;
        //for each pixel, we must calculate pixel color
        for(int ny= 0; ny<height; ny++)
        {
            for(int nx = 0; nx<width; nx++ )
            {
                Ray ray = rayGenerator::generateRay(camera, ny, nx);
                ray.depth = 0;
                Vec3f color = computePixelColor(scene, camera, ray, max_recursion_depth);
                image[pixel] = clamp(color.x);
                image[pixel+1] = clamp(color.y);
                image[pixel+2] = clamp(color.z);
                pixel += 3;
            }
        }

        write_ppm(camera.image_name.c_str(), image, width, height);
    }
}
