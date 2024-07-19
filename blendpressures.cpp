#include <sstream>
#include <image.h>
#include <iostream>
#include <fstream>


///this program takes the high pressure and low pressure grains generated by the crayon preprocess as input and averages them
///to produce the new high pressure grain texture.
///this is because the rendering algorithm produces results that are too "thick" or muddy looking
///at runtime using the default generation

int main (void){


    std::string materials[] = {
                                 "materialsfinal/constructionpaper-small",
                                 "materialsfinal/cardboard-small",
                                 "materialsfinal/notebook-small",
                                 "materialsfinal/parchment-small",
                                 "materialsfinal/coloringpaper-small",
                                 "materialsfinal/acrylic-small",
                                 "materialsfinal/construction-pink-small",
                                 "materialsfinal/construction-white-small",
                                 "materialsfinal/neon-small",
                                 "\0"
                              };

        for(unsigned int mat = 0; materials[mat].length(); mat++){


                std::cout<<"PROCESSING MATERIAL "<<materials[mat]<<std::endl;

                std::string lowPressureStr  = materials[mat] + "/grain.png";
                std::string highPressureStr = materials[mat] + "/grain-pressure.png";

                std::string outputStr = materials[mat] + "/grain-highpressure.png";

                //convert to float for more accurate averaging
                FloatImage lowPressure = loadImage(lowPressureStr);
                FloatImage highPressure = loadImage(highPressureStr);

                if(!lowPressure || !highPressure){

                    throw std::runtime_error("missing file");

                }

                LDRImage output = .5f*(lowPressure + highPressure);

                writeImage(output,outputStr);


        }



}