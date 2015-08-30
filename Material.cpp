//  Copyright (c) 2013 Christopher Pugh. All rights reserved.

#include "Material.h"

Material::operator bool()const
{
    return grainTexture && paperTexture && pressureTexture && crayons.size();
}


void Material::bind(){

    glActiveTexture(GL_TEXTURE0 + PaperTexture);
    glBindTexture(GL_TEXTURE_2D, paperTexture.tex);//paper

    glActiveTexture(GL_TEXTURE0 + PressureGrainTexture);
    glBindTexture(GL_TEXTURE_2D, pressureTexture.tex);

    glActiveTexture(GL_TEXTURE0 + GrainTexture);
    glBindTexture(GL_TEXTURE_2D, grainTexture.tex);
    
    if(lineTexture){
        glActiveTexture(GL_TEXTURE0 + LineTexture);
        glBindTexture(GL_TEXTURE_2D, lineTexture.tex);
    }

}






