#include <iostream>
#include "parser.h"
#include "ppm.h"
#include "rayGenerator.h"


int main(int argc, char* argv[])
{
    // Sample usage for reading an XML scene file
    parser::Scene scene;
    scene.loadFromXml(argv[1]);



    // The code below creates a test pattern and writes
    // it to a PPM file to demonstrate the usage of the
    // ppm_write function.
    //
    // Normally, you would be running your ray tracing
    // code here to produce the desired image.


    //write_ppm("test.ppm", image, width, height);

}
