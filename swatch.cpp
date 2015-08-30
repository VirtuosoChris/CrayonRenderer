#include <iostream>
#include <fstream>
#include <sstream>
#include <image.h>
#include <cstdlib>

const unsigned int numColors = 64;


void processDirectory(const std::string& swatchPrefix, std::ofstream& outFile){

    double color[3];

    color[0] = color[1] = color[2] = 0.0;

    double count;///\todo good to be double and single summation?
    std::cout<<"\n\n"<<std::endl;


    for(unsigned int i =0; ;i++){

        std::stringstream sstr;
        sstr<<swatchPrefix<<i<<".jpg";
        std::string filePath = sstr.str();
        std::cout<<"Processing "<<filePath<<std::endl;

        //std::ofstream(swatchPrefix+"test.txt");

        std::ifstream test(filePath);

        if(test){
        std::cout<<"OPEN"<<std::endl;}
        else{
        std::cout<<"NOT OPEN"<<std::endl;
        }



        LDRImage sample = loadImage(filePath,3);
        if(!sample){
            std::cout<<"\tfile not found"<<std::endl;
            if(!count)throw std::runtime_error("no samples available for swatch");
            break;}

        count += sample.getDimensions()[1] * sample.getDimensions()[2];

        for(unsigned int pix =0; pix < sample.numElements(); pix+=3){
            color[0]+= sample[pix];
            color[1]+= sample[pix+1];
            color[2]+= sample[pix+2];
        }
    }

    color[0] /= count;
    color[1] /= count;
    color[2] /= count;

    outFile<<(int)color[0]<<' '<<(int)color[1]<<' '<<(int)color[2]<<'\n';

}

void processSwatches(const std::string& materialPath){

    std::ofstream outFile(materialPath + "/colors.txt");

    for(unsigned int i =1; i <= numColors; i++){

        std::stringstream sstr;
        sstr<<materialPath<<"/"<<i<<'/';
        std::string swatchPrefix = sstr.str();

        processDirectory(swatchPrefix, outFile);


    }

    outFile.close();

}




int main(void){

    std::string materials[] = { "swatches/acrylic",
                                "swatches/cardboard",
                                "swatches/coloringpaper",
                                "swatches/neon",
                                "swatches/notebook",
                                "swatches/parchment",
                                "swatches/pinkcon",
                                "swatches/whitecon",
                                "swatches/yellowcon",///\todo common bug list
                                "\0"
                                };


    for(int i=0; materials[i].length();i++){

        std::cout<<"Str length is "<<materials[i].length()<<std::endl;

        try{

          processSwatches(materials[i]);

        }catch(const std::exception& ex){


            std::cerr<<"Error making material:: "<<ex.what()<<std::endl;

            system("pause");

        }

    }

    return 0;
}
