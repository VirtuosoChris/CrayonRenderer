//  Copyright (c) 2013 Christopher Pugh. All rights reserved.

#ifndef CRAYON_RENDERER_H_INCLUDED
#define CRAYON_RENDERER_H_INCLUDED

#include <SharedCounter.h>

#include "Material.h"

#include <GLTexture.h>
#include <GLShader.h>

#include <Eigen/core>


///\todo all of these functions set their glstate based on the usage patterns in the CrayonApp...
///either document or revise

void screenQuad();

typedef Eigen::Vector2d PenLocation;


const unsigned int BUFFER_LAST_RENDERING=1u;
const unsigned int BUFFER_RENDER_TARGET=0u;

class CrayonRenderer
{
protected:

    GLuint framebufferObject;
    
    Utilities::SharedCounter copyCount;

    unsigned int frameWidth;
    unsigned int frameHeight;

    

public:
    
    CrayonRenderer& operator= (const CrayonRenderer &rendererIn);
    
    int unitPair ;//= 0; //which pair of texture units constitute the ping pong buffers
    
    GLTexture colorBuffers[2];
    

    bool eraserMode; //are we in crayon drawing mode or eraser mode?

    Eigen::Vector2f lastOffset;
    
    GL::GLShader alphaProg;
    GL::GLShader quadProg;
    GL::GLShader backgroundProg;
    GL::GLShader eraserProg;

    CrayonRenderer():
        framebufferObject(0), frameWidth(0), frameHeight(0),unitPair(0), eraserMode(false) {}

    void bindCrayonBuffers();

    unsigned int getFrameWidth()const{return frameWidth;}

    unsigned int getFrameHeight()const{return frameHeight;}

    GLuint getFBO()const{return framebufferObject;}

   // GLTexture& getCrayonBuffer();

    ///allocates render target textures and an internal FBO
    void allocateRenderTargets(unsigned int w,unsigned int h);

    void drawLine(const PenLocation& lastPenLocation, const PenLocation& penLocation,float pressure, double penSize);

    void drawLineGeometry(const PenLocation& lastPenLocation, const PenLocation& penLocation, double penSize);

    void freeGLResources();
    virtual ~CrayonRenderer();

    void swapCrayonBuffers();
    
    void eraser(const PenLocation& penLocation, const PenLocation& lastPenLocation, double penSize);


};


void checkFBOErrors();

#endif
