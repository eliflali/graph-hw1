#include <iostream>
#include "parser.h"
#include "ppm.h"
#include "rayGenerator.h"

using namespace rayGenerator;
using namespace parser;

Vec3i background_color;
float shadow_ray_epsilon;
int max_recursion_depth;
std::vector<Camera> cameras;
Vec3f ambient_light;
std::vector<PointLight> point_lights;
std::vector<Material> materials;
std::vector<Vec3f> vertex_data;
std::vector<Mesh> meshes;
std::vector<Triangle> triangles;
std::vector<Sphere> spheres;

int main(int argc, char* argv[])
{

    // Sample usage for reading an XML scene file
    parser::Scene scene;
    scene.loadFromXml(argv[1]);

    background_color = scene.background_color;
    shadow_ray_epsilon = scene.shadow_ray_epsilon;
    max_recursion_depth = scene.max_recursion_depth;
    cameras = scene.cameras;
    ambient_light = scene.ambient_light;
    point_lights = scene.point_lights;
    materials = scene.materials;
    vertex_data = scene.vertex_data;
    meshes = scene.meshes;
    triangles = scene.triangles;
    spheres = scene.spheres;

    //for every camera angle, we must have a distinct ppm.
    for(int i = 0; i<cameras.size(); i++)
    {
        Camera camera = cameras[i];
        int width = camera.image_width;
        int height = camera.image_height;
        unsigned char* image = new unsigned char [width * height * 3];
        //for each pixel, we must calculate pixel color
        for(int ny= 0; ny<height; ny++)
        {
            for(int nx = 0; nx<width; nx++ )
            {
                Ray ray = rayGenerator::generateRay(camera, width, height);

            }
        }

        write_ppm(camera.image_name.c_str(), image, width, height);
    }
}
