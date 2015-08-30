//  Copyright (c) 2013 Christopher Pugh. All rights reserved.

#include "CrayonRenderer.h"
#include <Quad.h>
#include <GpuMesh.h>

#ifndef GL_ES_BUILD
#include <GL/glew.h>
#endif

#include <Mesh.h>

using namespace Virtuoso;
void screenQuad()
{
    static Virtuoso::Quad q;
    static Virtuoso::GPUMesh quad(q);

    quad.push();

}

void CrayonRenderer::bindCrayonBuffers(){//the crayon buffers go to fixed texture units... we just pass in differing uniforms based on the buffer swap
    glActiveTexture(GL_TEXTURE0 + Buffer0);
    glBindTexture(GL_TEXTURE_2D, colorBuffers[0].tex);
    
    glActiveTexture(Buffer1 + GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorBuffers[1].tex);
}

void CrayonRenderer::swapCrayonBuffers(){

    unitPair = (unitPair+1)%2;

}


void CrayonRenderer::freeGLResources()
{
    if(framebufferObject)
        glDeleteFramebuffers(1,&framebufferObject);

}


CrayonRenderer& CrayonRenderer::operator=(const CrayonRenderer &rendererIn)
{
    // Check for self assignment
    if(this != &rendererIn)
    {
        if(copyCount.isUnique())
        {
            freeGLResources();
        }
        
        framebufferObject = rendererIn.framebufferObject;
        copyCount = rendererIn.copyCount;
        frameWidth = rendererIn.frameWidth;
        frameHeight = rendererIn.frameHeight;
        unitPair = rendererIn.unitPair;
        colorBuffers[0] = rendererIn.colorBuffers[0];
        colorBuffers[1] = rendererIn.colorBuffers[1];
        eraserMode = rendererIn.eraserMode;
        lastOffset = rendererIn.lastOffset;
        alphaProg = rendererIn.alphaProg;
        quadProg = rendererIn.quadProg;
        backgroundProg = rendererIn.backgroundProg;
        eraserProg = rendererIn.eraserProg;
    }
    
    return *this;
}


CrayonRenderer::~CrayonRenderer()
{

    if(copyCount.isUnique())
    {
        freeGLResources();
    }
}


#ifndef GL_ES_BUILD

void handleFboError(GLenum err)
{
    if(glGetError() != GL_NO_ERROR)
    {
        std::cerr<<"A gl error was found"<<std::endl;
    }

    switch(err)
    {

    case GL_NONE:
        std::cerr<<"GL_NONE ERROR"<<std::endl;
        break;

    case GL_FRAMEBUFFER_UNDEFINED:
        std::cerr<<"Undefined default framebuffer bound!"<<std::endl;
        break;

    case GL_FRAMEBUFFER_COMPLETE://hooray!
        std::cout<<"FRAMEBUFFER COMPLETE!"<<std::endl;
        break;

    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        std::cerr<<"One of the render buffers is incomplete!"<<std::endl;
        break;

    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        std::cerr<<"At least one buffer must be attached to the fbo!"<<std::endl;
        break;

    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
        std::cerr<<"A buffer attachment point is enabled but has no attached buffer"<<std::endl;
        break;

    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
        std::cerr<<"read attachment point enabled but does not have buffer attached"<<std::endl;
        break;

    case GL_FRAMEBUFFER_UNSUPPORTED:
        std::cerr<<"Unsupported format combinations"<<std::endl;
        break;

    case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
        std::cerr<<"Multisampling mismatch"<<std::endl;
        break;

    case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
        std::cerr<<"layer targets mismatch"<<std::endl;
        break;

    default:
        std::cerr<<"Unhandled error case in FBO check!"<<std::endl;
        break;

    }

}



void checkFBOErrors(){
        GLenum err;
        if((err=glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER))!= GL_FRAMEBUFFER_COMPLETE)
        {
            handleFboError(err);
        }
}

#else


void handleFboError(GLenum err)
{
    if(glGetError() != GL_NO_ERROR)
    {
        std::cerr<<"A gl error was found"<<std::endl;
    }
    
    switch(err)
    {
        case GL_FRAMEBUFFER_COMPLETE://hooray!
            std::cout<<"FRAMEBUFFER COMPLETE!"<<std::endl;
            break;
            
        
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            std::cerr<<"One of the render buffers is incomplete!"<<std::endl;
            break;
        
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
            std::cerr<<"Not all attached images have the same dimension"<<std::endl;
            break;
            
            
            
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            std::cerr<<"At least one buffer must be attached to the fbo!"<<std::endl;
            break;
            
            
        case GL_FRAMEBUFFER_UNSUPPORTED:
            std::cerr<<"Unsupported format combinations"<<std::endl;
            break;
            
        
            
        default:
            std::cerr<<"Unhandled error case in FBO check!"<<std::endl;
            break;
            
    }
    
}



void checkFBOErrors(){
    GLenum err;
    if((err=glCheckFramebufferStatus(GL_FRAMEBUFFER))!= GL_FRAMEBUFFER_COMPLETE)
    {
        handleFboError(err);
    }
}


#endif


#ifndef GL_ES_BUILD
void CrayonRenderer::allocateRenderTargets(unsigned int w,unsigned int h)
{
    frameWidth=w;
    frameHeight=h;

    ImageFormat bufferFormat;
    bufferFormat.width = w;
    bufferFormat.height = h;
    bufferFormat.channels=4;

    //allocate the render textures
    colorBuffers[0] = GLTexture(bufferFormat);
    colorBuffers[1] = GLTexture(bufferFormat);

    glBindTexture(GL_TEXTURE_2D,colorBuffers[1].tex);

    glGenFramebuffers(1,&framebufferObject);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebufferObject);

    glFramebufferTexture2D(
        GL_DRAW_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,

        colorBuffers[1].tex,
        0
    );

    GLenum buffers[]= {GL_COLOR_ATTACHMENT0, GL_BACK_LEFT};

    glDrawBuffers(1,&buffers[0]);

    glClear( GL_COLOR_BUFFER_BIT);

    glFramebufferTexture2D(
        GL_DRAW_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        colorBuffers[0].tex,
        0
    );

    glDrawBuffers(1,&buffers[0]);

    glClear( GL_COLOR_BUFFER_BIT);


}
#else


void CrayonRenderer::allocateRenderTargets(unsigned int w,unsigned int h)
{
    frameWidth=w;
    frameHeight=h;
    
    ImageFormat bufferFormat;
    bufferFormat.width = w;
    bufferFormat.height = h;
    bufferFormat.channels=4;
    
    //allocate the render textures
    colorBuffers[0] = GLTexture(bufferFormat);
    colorBuffers[1] = GLTexture(bufferFormat);
    
    glBindTexture(GL_TEXTURE_2D,colorBuffers[1].tex);
    
    glGenFramebuffers(1,&framebufferObject);
    
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferObject);
    
    glFramebufferTexture2D(
                           GL_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D,
                           
                           colorBuffers[1].tex,
                           0
                           );
        
    glClear( GL_COLOR_BUFFER_BIT);
    
    glFramebufferTexture2D(
                           GL_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D,
                           colorBuffers[0].tex,
                           0
                           );
    
        
    glClear( GL_COLOR_BUFFER_BIT);
    
    
}


#endif




void CrayonRenderer::eraser(const PenLocation& penLocation, const PenLocation& lastPenLocation,  double penSize){
    
    penSize *= 2.0;
    const GLuint currentLast[2][2] = {{Buffer0, Buffer1}, {Buffer1, Buffer0}};
    
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferObject);
    
    glFramebufferTexture2D(
                           GL_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D,
                           
                           colorBuffers[ currentLast[unitPair][BUFFER_RENDER_TARGET]].tex,
                           
                           0
                           );
    
    
    checkFBOErrors();
    
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    
    quadProg.bind();
    quadProg.setTexture("tex",currentLast[unitPair][BUFFER_LAST_RENDERING]);
    
    screenQuad();
    
    
    extern float zoomLevel; ///start with fullscreen
    
    extern float boxLocationX;
    
    extern float boxLocationY;
    
    eraserProg.bind();
    
    eraserProg.setTexture("crayonBuffer", currentLast[unitPair][BUFFER_LAST_RENDERING]);
    eraserProg.setUniform("zoomLevel", zoomLevel);
    eraserProg.setUniform("boxLocation", boxLocationX, boxLocationY);
    
    glViewport(boxLocationX * frameWidth, boxLocationY* frameHeight, zoomLevel * frameWidth, zoomLevel* frameHeight);
    
    //get vector perpendicular to the line direction & normalize
    Eigen::Vector2f offset( (penLocation[1] - lastPenLocation[1]),(lastPenLocation[0] - penLocation[0]));
    
    offset.normalize();
    
    //pass in the normalized offset vector to the shader
    
    eraserProg.setUniform("offsetNorm", offset[0],offset[1]);
    eraserProg.setUniform("lastPenLocationClip", (float)lastPenLocation[0],(float)lastPenLocation[1]);
    
    //the size should be the size of the pen in clip coords.  penSize is in pixels, so do the appropriate conversions
    //note that this scaled vector is HALF the pen size since it goes from the center to the edge.
    offset[1] *=(penSize / frameHeight);
    offset[0] *= (penSize / frameWidth);
    
    float halfPenRadiusClip = offset.norm();//get the magnitude of the offset vector scaled to be the length of the pen radius
    eraserProg.setUniform("invPenRadiusClip", 1.0f / halfPenRadiusClip);
    
    PenLocation tempdiff = penLocation - lastPenLocation;
    
    //convert from clip vector to pixel vector
    tempdiff[1] += 1.0;
    tempdiff[1] *=.50;
    tempdiff[1] *= frameHeight;
    
    tempdiff[0] += 1.0;
    tempdiff[0] *=.50;
    tempdiff[0] *= frameWidth;
    
    
    //norm is length of line in pixels.
    //endTC is therefore portion of the way "into" the brush... but we're not using this anymore apparently
    double endTC = tempdiff.norm() / penSize;
    
    Mesh lineModel;
    
    lineModel.addAttribute("position",3);
    
    lineModel.addAttribute("texcoord",2);
    
    AttributeArray::Inserter<3> pos((lineModel)["position"]);
    
    AttributeArray::Inserter<2> tc((lineModel)["texcoord"]);
    
    lineModel.begin(7);
    
    tc.vertex({1,0});
    pos.vertex({lastPenLocation[0] + offset[0], lastPenLocation[1] + offset[1], 0.0});
    
    
    tc.vertex( {1,endTC} );
    pos.vertex({penLocation[0] + offset[0], penLocation[1] + offset[1],0});
    
    tc.vertex({0,endTC});
    pos.vertex({penLocation[0] - offset[0], penLocation[1] - offset[1],0});
    
    
    tc.vertex({0,0});
    pos.vertex({lastPenLocation[0] - offset[0], lastPenLocation[1] - offset[1], 0.0});
    
    //start connective triangle verts
    tc.vertex({.50f,0.0f}) ;
    pos.vertex({lastPenLocation[0], lastPenLocation[1], 0.0});
    
    
    tc.vertex({0.0f,0.0f}) ;
    pos.vertex({lastPenLocation[0] - lastOffset[0], lastPenLocation[1] - lastOffset[1], 0.0});
    
    tc.vertex({0.0,0.0f}) ;
    pos.vertex({lastPenLocation[0] + lastOffset[0], lastPenLocation[1] + lastOffset[1], 0.0});
    
    lastOffset = offset;
    
    //insert connective triangles to make the line segments less blocky, eliminate gaps at direction changes
    
    lineModel.end();
    
    lineModel.beginIndices(12);///\todo should take faces?
    
    //line quad
    lineModel.insertIndices(0,1,2);
    lineModel.insertIndices(2,3,0);
    
    lineModel.insertIndices(  4,3,5);
    lineModel.insertIndices(4,0,6);
    
    lineModel.endIndices();
    
    GPUMesh mesh(lineModel);
    
    mesh.push();
}


#ifndef GL_ES_BUILD

void CrayonRenderer::drawLine(const PenLocation& lastPenLocation, const PenLocation& penLocation, float pressure, double penSize)
{//return;
    
    
    if(eraserMode){
        
        eraser(const PenLocation& penLocation,   double penSize);
        
        return;
    }
    
    const GLuint currentLast[2][2] = {{Buffer0, Buffer1}, {Buffer1, Buffer0}};
    const GLenum buffers[]= {GL_COLOR_ATTACHMENT0, GL_BACK_LEFT};

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebufferObject);

    glFramebufferTexture2D(
        GL_DRAW_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,

        colorBuffers[ currentLast[unitPair][BUFFER_RENDER_TARGET]].tex,

        0
    );

    glDrawBuffers(1,&buffers[0]);

    checkFBOErrors();

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    quadProg.bind();
    quadProg.setTexture("tex",currentLast[unitPair][BUFFER_LAST_RENDERING]);

    screenQuad();

    alphaProg.bind();
    alphaProg.setTexture("grainTexture", GrainTexture);

    alphaProg.setTexture("crayonBuffer", currentLast[unitPair][BUFFER_LAST_RENDERING]);

    alphaProg.setUniform("crayonPressure", pressure);
    
    drawLineGeometry(lastPenLocation, penLocation,penSize);

}

#else


void CrayonRenderer::drawLine(const PenLocation& lastPenLocation, const PenLocation& penLocation, float pressure, double penSize)
{//return;
    
    if(eraserMode){
        
        eraser(  penLocation,  lastPenLocation,  penSize);
        
        return;
    }
    
    const GLuint currentLast[2][2] = {{Buffer0, Buffer1}, {Buffer1, Buffer0}};
    
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferObject);
    
    glFramebufferTexture2D(
                           GL_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D,
                           
                           colorBuffers[ currentLast[unitPair][BUFFER_RENDER_TARGET]].tex,
                           
                           0
                           );
    
    
    checkFBOErrors();
    
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    quadProg.bind();
    quadProg.setTexture("tex",currentLast[unitPair][BUFFER_LAST_RENDERING]);
    
    screenQuad();
    
    alphaProg.bind();
    alphaProg.setTexture("grainTexture", GrainTexture);
    
    alphaProg.setTexture("crayonBuffer", currentLast[unitPair][BUFFER_LAST_RENDERING]);
    
    alphaProg.setUniform("crayonPressure", pressure);
    
    
    extern float zoomLevel; ///start with fullscreen
    
    extern float boxLocationX;
    
    extern float boxLocationY;
    
    glViewport(boxLocationX * frameWidth, boxLocationY* frameHeight, zoomLevel * frameWidth, zoomLevel* frameHeight);
    
    alphaProg.setUniform("zoomLevel", zoomLevel);
    alphaProg.setUniform("boxLocation", boxLocationX, boxLocationY);
    
    
    
    drawLineGeometry(lastPenLocation, penLocation,penSize);
    
    
}

#endif

void CrayonRenderer::drawLineGeometry(const PenLocation& lastPenLocation, const PenLocation& penLocation, double penSize)
{
    //get vector perpendicular to the line direction & normalize
    Eigen::Vector2f offset( (penLocation[1] - lastPenLocation[1]),(lastPenLocation[0] - penLocation[0]));

    offset.normalize();
    
    //pass in the normalized offset vector to the shader
    
    alphaProg.setUniform("offsetNorm", offset[0],offset[1]);
    alphaProg.setUniform("lastPenLocationClip", (float)lastPenLocation[0],(float)lastPenLocation[1]);
    
    //the size should be the size of the pen in clip coords.  penSize is in pixels, so do the appropriate conversions
    //note that this scaled vector is HALF the pen size since it goes from the center to the edge.
    offset[1] *=(penSize / frameHeight);
    offset[0] *= (penSize / frameWidth);

    float halfPenRadiusClip = offset.norm();//get the magnitude of the offset vector scaled to be the length of the pen radius
    alphaProg.setUniform("invPenRadiusClip", 1.0f / halfPenRadiusClip);
    
    PenLocation tempdiff = penLocation - lastPenLocation;

    //convert from clip vector to pixel vector
    tempdiff[1] += 1.0;
    tempdiff[1] *=.50;
    tempdiff[1] *= frameHeight;

    tempdiff[0] += 1.0;
    tempdiff[0] *=.50;
    tempdiff[0] *= frameWidth;

    //norm is length of line in pixels.
    //endTC is therefore portion of the way "into" the brush... but we're not using this anymore apparently
    double endTC = tempdiff.norm() / penSize;

    Mesh lineModel;

    lineModel.addAttribute("position",3);

    lineModel.addAttribute("texcoord",2);

    AttributeArray::Inserter<3> pos((lineModel)["position"]);

    AttributeArray::Inserter<2> tc((lineModel)["texcoord"]);

    lineModel.begin(7);

        tc.vertex({1,0});
        pos.vertex({lastPenLocation[0] + offset[0], lastPenLocation[1] + offset[1], 0.0});


        tc.vertex( {1,endTC} );
        pos.vertex({penLocation[0] + offset[0], penLocation[1] + offset[1],0});
        

   
        tc.vertex({0,endTC});
        pos.vertex({penLocation[0] - offset[0], penLocation[1] - offset[1],0});
    
        tc.vertex({0,0});
        pos.vertex({lastPenLocation[0] - offset[0], lastPenLocation[1] - offset[1], 0.0});
    
        //start connective triangle verts
        tc.vertex({.50f,0.0f}) ;
        pos.vertex({lastPenLocation[0], lastPenLocation[1], 0.0});
    
    
        tc.vertex({0.0f,0.0f}) ;
        pos.vertex({lastPenLocation[0] - lastOffset[0], lastPenLocation[1] - lastOffset[1], 0.0});
    
        tc.vertex({0.0,0.0f}) ;
        pos.vertex({lastPenLocation[0] + lastOffset[0], lastPenLocation[1] + lastOffset[1], 0.0});
    
        lastOffset = offset;
    
    //insert connective triangles to make the line segments less blocky, eliminate gaps at direction changes

        lineModel.end();

        lineModel.beginIndices(12);///\todo should take faces?

        //line quad
        lineModel.insertIndices(0,1,2);
        lineModel.insertIndices(2,3,0);
    
        //connective tris
        lineModel.insertIndices(  4,3,5);
        lineModel.insertIndices(4,0,6);
    
        lineModel.endIndices();

        GPUMesh mesh(lineModel);

        mesh.push();
}


