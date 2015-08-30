//  Copyright (c) 2013 Christopher Pugh. All rights reserved.

#include "CrayonApp.h"
#include <image.h>
#include <SystemImage.h>

#include "iosHelpers.h"

#include "appInstance.h"

#include "GetPhoto.h"

///\todo declaration of external function in implementation file is a little unclean
std::string pathString();

std::string grainFilePressure(const std::string& materialPath)
{
    return materialPath + "/grain-highpressure.png";
}


std::string grainFile(const std::string& materialPath)
{
    return materialPath + "/grain.png";
}


std::string paperFile(const std::string& materialPath)
{
    return materialPath + "/material.jpg";
}


std::string grainFilePressure(const std::string& materialPath, const std::string& suffix)
{
    return materialPath + "/grain-highpressure-" + suffix + ".png";
}


std::string grainFile(const std::string& materialPath, const std::string& suffix)
{
    return materialPath + "/grain-" + suffix + ".png";
}


std::string paperFile(const std::string& materialPath, const std::string& suffix)
{
    return materialPath + "/material-" + suffix + ".jpg";
}


CrayonApp::CrayonApp(unsigned int w, unsigned int h, const Material& mat):
    resWidth(w), resHeight(h),penDown(false),
    currentCrayon(0),
    penSize(penSizeDefault),
    fastSpeed(fastSpeedDefault),
    slowSpeed(slowSpeedDefault),
    material(mat),
    shouldSave(false),
    showPenIcon(true),
    iconDelta(0.0),
    iconOpacityScale(1.0)
  
{
    initGLState();
    
    {
        LDRImage::index_type sizeDummy = {{4,1,1}};
        LDRImage img(sizeDummy); //=//(loadImage("./gaussian.png"));
        img[0] = img[1] = img[2] = img[3] = 255u;
        flipVertical(img);
        penIcon = GLTexture(img);
    }
    
    {
        LDRImage img = loadImage("./magnifier.png");
        flipVertical(img);
        magnifierIcon = GLTexture(img);
    }
    
    glActiveTexture(GL_TEXTURE0 + NumTextureBindings); //bind one past the end.
    glBindTexture(GL_TEXTURE_2D, penIcon.tex);
    
    if(!penIcon)
    {
        throw std::runtime_error("ICON TEXTURE MISSING");
    }
    
    initializeShaders();
    
    std::cout<<"Shaders initialized"<<std::endl;

    allocateRenderTargets(w,h);
}


const std::string& CrayonApp::currentCrayonName()const
{
    return material.crayons[currentCrayon].second;
}


const CrayonColor& CrayonApp::currentCrayonColor()const
{
    return material.crayons[currentCrayon].first;
}


void CrayonApp::setCurrentCrayon(unsigned int c)
{
    if(c>=material.crayons.size())
    {
        throw std::runtime_error("Crayon index out of bounds");
    }
    
    currentCrayon=c;
    setCrayonColorUniform();
}   


///\todo extract rectangle: does it respect bounds?  is it as efficient?  would affine tile and crop work better? 
///\todo this file was a cpp
Material loadMaterial(const std::string& materialName, 
                      const std::string& materialPath,
                      Crayon* beginCrayon,
                      Crayon* endCrayon,
                      std::pair<GLuint, GLuint> resolution
                      )
{
    Timer tim;
    tim.reset();

    std::string materialPathCached = materialPath + "/cached";
    
    NSString* directory =   [NSString stringWithFormat:@"%@/%s", appDocumentsPath(), materialPathCached.c_str()];
    
    materialPathCached = NSStringToString(directory);
    
    mutex.lock();
        if(!directoryExistsAtPath(directory))
        {
            createDirectory(directory,nil);
        }
    mutex.unlock();
    
    mutex.lock();
        bool grainCached = fileExistsAtPath( [NSString stringWithFormat:@"%s", grainFile(materialPathCached,materialName).c_str()] );
        bool paperCached = fileExistsAtPath( [NSString stringWithFormat:@"%s", paperFile(materialPathCached,materialName).c_str() ]);
        bool pressureCached = fileExistsAtPath( [NSString stringWithFormat:@"%s", grainFilePressure(materialPathCached,materialName).c_str() ]);
    mutex.unlock();
    
    SystemImage grain;
    SystemImage paper;
    SystemImage pressure;
    
    if(!grainCached)//create the cached image
    {
        SystemImage grainRaw(grainFile(materialPath));
        
        unsigned int middleX =  (grainRaw.getWidth()>>1) - (resolution.first>>1);
        unsigned int middleY =  (grainRaw.getHeight()>>1) - (resolution.second>>1);
        
        grain = crop(affineTile(grainRaw), middleX - .5 * frameDimensions.first, middleY - .5*frameDimensions.second, frameDimensions.first, frameDimensions.second);
        mutex.lock();
            grain.writeToFile(grainFile(materialPathCached,materialName));
        mutex.unlock();
    }
    else//load the cached image
    {
        std::cout<<"LOADING CACHED IMAGE"<<std::endl;
        grain=SystemImage(grainFile(materialPathCached,materialName));
    }
    
    if(!paperCached) //create the cached image
    {
        SystemImage paperRaw(paperFile(materialPath));
        
        unsigned int middleX =  (paperRaw.getWidth()>>1) - (resolution.first>>1);
        unsigned int middleY =  (paperRaw.getHeight()>>1) - (resolution.second>>1);
        
        paper = crop(affineTile(paperRaw), middleX - .5 * frameDimensions.first, middleY - .5*frameDimensions.second, frameDimensions.first, frameDimensions.second);
        mutex.lock();
            paper.writeToFile(paperFile(materialPathCached,materialName));
        mutex.unlock();
        
    }
    else //load the cached image
    {
        auto paperPath = paperFile(materialPathCached,materialName);
        paper=SystemImage(paperPath);
    }
    
    if(!pressureCached)//create the cached image
    {
        SystemImage pressureRaw(grainFilePressure(materialPath));
        
        unsigned int middleX =  (pressureRaw.getWidth()>>1) - (resolution.first>>1);
        unsigned int middleY =  (pressureRaw.getHeight()>>1) - (resolution.second>>1);
        
        pressure = crop(affineTile(pressureRaw), middleX - .5 * frameDimensions.first, middleY - .5*frameDimensions.second, frameDimensions.first, frameDimensions.second);

        mutex.lock();
            pressure.writeToFile(grainFilePressure(materialPathCached,materialName));
        mutex.unlock();
    }
    else//load the cached image
    {
        pressure=SystemImage(grainFilePressure(materialPathCached,materialName));
    }
    
    LDRImage grainImg = toLDRImage(grain);
    LDRImage pressureImg = toLDRImage(pressure);
    LDRImage paperImg = toLDRImage(paper);
    
    double  delta = tim.getDelta();
    
    std::cout<<"TIME TO LOAD MATERIAL INPUT DATA"<<delta<<std::endl;
    
    return Material(
                    grainImg,
                    paperImg,
                    pressureImg,
                    beginCrayon, endCrayon
                    );
}


void CrayonApp::initGLState()
{
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glClearColor(0.0, 0.0, 0.0, 0.0);
}


bool CrayonApp::drewALine()
{
    return  penDown && penLocation != lastPenLocation;
}


float CrayonApp::pressureFromSpeed(float speedIn)
{
    return std::min<float>(1.0f, (std::max<float>(0.0f,(speedIn - slowSpeed)) / (fastSpeed - slowSpeed)));
}


void bindLocs(GL::GLShader& shader)
{
    glBindAttribLocation(shader.prog, 0, "position");
    glBindAttribLocation(shader.prog, 1, "texcoord");
    glLinkProgram(shader.prog);
}


void CrayonApp::initializeShaders()
{
    std::string path = pathString();
    std::cout<<path<<" is path "<<std::endl;
    
    eraserProg.initializeShader(path + "eraser.vert", path + "eraser.frag");
    bindLocs(eraserProg);

    eraserProg.bind();
    eraserProg.setTexture("crayonBuffer", Buffer0);
    eraserProg.setUniform("zoomLevel",1.0f);
    eraserProg.setUniform("boxLocation",0.0f,0.0f);
   
    alphaProg.initializeShader(path + "passthrough3.vert", path + "drawpass.frag");
    bindLocs(alphaProg);
    
    alphaProg.bind();
    alphaProg.setTexture("grainTexture", GrainTexture);
    alphaProg.setTexture("crayonBuffer", Buffer0);
    alphaProg.setTexture("grainTexturePressure", PressureGrainTexture);
    alphaProg.setUniform("crayonPressure", 1.0f);
    alphaProg.setUniform("screenDims",(float)resWidth, (float)resHeight);
    alphaProg.setUniform("scale", 1.0f);
    alphaProg.setUniform("zoomLevel",1.0f);
    alphaProg.setUniform("boxLocation",0.0f,0.0f);
    
    iconProg.initializeShader(path + "passthrough.vert",path + "icon.frag");
    bindLocs(iconProg);

    iconProg.bind();
    iconProg.setTexture("iconTexture", NumTextureBindings);
    iconProg.setUniform("zoomLevel",1.0f);
    iconProg.setUniform("boxLocation",0.0f,0.0f);
    
    backgroundProg.initializeShader(path+"passthrough2.vert", path + "backgroundComposite.frag");
    bindLocs(backgroundProg);
    
    Eigen::Matrix3f texMat = Eigen::Matrix3f::Identity();
    backgroundProg.bind();
    backgroundProg.setUniform("texMat", texMat);
    backgroundProg.setUniform("zoomLevel",1.0f);
    backgroundProg.setUniform("boxLocation",0.0f,0.0f);
    backgroundProg.setTexture("crayonBuffer", Buffer1);
    backgroundProg.setTexture("paper", PaperTexture);
    backgroundProg.setTexture("lineTexture", LineTexture);
    
    quadProg.initializeShader(path+"passthrough.vert", path+"quad.frag");
    bindLocs(quadProg);
    quadProg.bind();
    quadProg.setTexture("tex",PaperTexture );
    quadProg.setUniform("zoomLevel",1.0f);
    quadProg.setUniform("boxLocation",0.0f,0.0f);
    
    setCrayonColorUniform();
}


void CrayonApp::setCrayonColorUniform()
{
    const CrayonColor& c =  material.crayons[currentCrayon].first;

    float opacity = iconOpacityDefault * iconOpacityScale;
    
    iconProg.bind();
    iconProg.setUniform("color", c[0], c[1], c[2], opacity);
    
    alphaProg.bind();
    alphaProg.setUniform("color", 1.0-c[0], 1.0-c[1], 1.0-c[2]);
}


void CrayonApp::onPenDown(const PenLocation& pin)
{
    penDown = true;
    penLocation = (pin*2.0) - PenLocation(1.0,1.0);
    lastPenLocation = penLocation;
    lastOffset = Eigen::Vector2f(0.0,0.0);
}


void CrayonApp::onPenUp()
{
    penDown=false;
    shouldSave=true;
    lastOffset = Eigen::Vector2f(0.0,0.0);
}


///input is a vec2 with screen coordinates normalized from (0,1) on each axis
void CrayonApp::handlePenMove(const PenLocation& pin)
{
    penLocation = (pin*2.0) - PenLocation(1.0,1.0);

    if(!penDown)//for mouse purposes... can move while not clicked
    {
        lastPenLocation = penLocation;
    }
}



void CrayonApp::renderCrayonBuffers()
{
    glViewport(0,0,frameWidth, frameHeight);
    material.bind();
    bindCrayonBuffers();

    double deltaT = timer.getDelta();

    if(drewALine())
    {
        float speed = (penLocation - lastPenLocation).norm() / deltaT;
        float pressure =  pressureFromSpeed(speed);
        drawLine(lastPenLocation, penLocation, pressure, penSize);
    }
    
    //non-scaled viewport
    glViewport(0,0,frameWidth, frameHeight);
}

void CrayonApp::renderCompositedFrame()
{
    bindCrayonBuffers();///\todo wasnt here
    const GLuint currentLast[2][2] = {{Buffer0, Buffer1}, {Buffer1, Buffer0}};

    if(drewALine())
    {
        swapCrayonBuffers();
    }
    
    backgroundProg.bind();
    backgroundProg.setTexture("crayonBuffer", currentLast[unitPair][BUFFER_LAST_RENDERING]);
    backgroundProg.setUniform("linesPresent", (bool)material.lineTexture);
    backgroundProg.setUniform("zoomLevel", zoomLevel);
    backgroundProg.setUniform("boxLocation", boxLocationX, boxLocationY);
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    screenQuad();
    
    lastPenLocation = penLocation;

    timer.reset();
}


void CrayonApp::erase()
{
    shouldSave = true; 
    
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
    
    glFramebufferTexture2D(
                           GL_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D,
                           colorBuffers[ currentLast[unitPair][BUFFER_LAST_RENDERING]].tex,
                           0
                           );
    
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
}


void CrayonApp::pushBackground()
{
    material.bind();
    bindCrayonBuffers();
    
    const GLuint currentLast[2][2] = {{Buffer0, Buffer1}, {Buffer1, Buffer0}};
    
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferObject);
    
    glViewport(0,0,frameWidth, frameHeight);
    
    glFramebufferTexture2D(
                           GL_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D,
                           colorBuffers[ currentLast[unitPair][BUFFER_RENDER_TARGET]].tex,
                           0
                           );
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    quadProg.bind();
    
    glActiveTexture(GL_TEXTURE0 + currentLast[unitPair][BUFFER_LAST_RENDERING]);
    glBindTexture(GL_TEXTURE_2D, colorBuffers[currentLast[unitPair][BUFFER_LAST_RENDERING]].tex);
    quadProg.setTexture("tex",currentLast[unitPair][BUFFER_LAST_RENDERING]);
    
    screenQuad();
    glFinish();
}

