#include "crayonPreprocess.h"
#include <iostream>

int main(void)
{

    std::string materials[] = { "materials/tissuepaper-small",
                                "materials/constructionpaper-small",
                                 "materials/cardboard",
                                 "materials/cardboard-small",
                                 "materials/constructionpaper",
                                 "materials/notebook",
                                 "materials/parchment",
                                 "materials/coloringpaper-small",


                                 "materials/acrylic",
                                 "materials/construction-pink",
                                 "materials/construction-white",
                                 "materials/neon",
                                 "materials/tissuepaper",
                                 "materials/coloringpaper",
                                 "\0"
                              };

    int i =0;

    while(materials[i].length())
    {

        try
        {

            std::cerr<<"\nMaking material "<<materials[i]<<std::endl;

            makeMaterial(materials[i]);

        }
        catch(const std::exception& ex)
        {


            std::cerr<<"Error making material:: "<<ex.what()<<std::endl;

            system("pause");

        }
        i++;
    }

    return 0;
}
