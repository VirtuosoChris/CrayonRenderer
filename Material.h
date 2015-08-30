//  Copyright (c) 2013 Christopher Pugh. All rights reserved.

#ifndef CRAYON_MATERIAL_H_INCLUDED
#define CRAYON_MATERIAL_H_INCLUDED

#include <array>
#include <GLTexture.h>

typedef std::array<float, 3> CrayonColor;

typedef std::pair<CrayonColor, std::string> Crayon;

//print name of crayon
inline std::ostream& operator<<(std::ostream& str, const Crayon& crayon){
    return str<<crayon.second;
}


enum TextureBindings
{
    Buffer0,
    Buffer1,
    PaperTexture,
    GrainTexture,
    PressureGrainTexture,
    LineTexture,
    NumTextureBindings
};


struct Material
{
    GLTexture grainTexture;
    GLTexture paperTexture;
    GLTexture pressureTexture;
    GLTexture lineTexture;

    std::vector<Crayon> crayons;

    template <class ITERATOR>
    Material(const GLTexture& grainTex,
             const GLTexture& paperTex,
             const GLTexture& pressureTex,
             ITERATOR crayonBegin, ITERATOR crayonEnd
             )
             :grainTexture(grainTex),
             paperTexture(paperTex),
             pressureTexture(pressureTex),
             crayons(crayonBegin, crayonEnd)
             {
                
             }

    void bind();

    Material(){}///dummy

    operator bool()const;

    unsigned int numCrayons()const{return crayons.size();}
};




#endif
