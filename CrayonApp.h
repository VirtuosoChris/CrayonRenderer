//  Copyright (c) 2013 Christopher Pugh. All rights reserved.

#ifndef CRAYON_APP_H_INCLUDED
#define CRAYON_APP_H_INCLUDED

#include "CrayonRenderer.h"

#include "DefaultMaterials.h"

#include <Timer.h>


const double fastSpeedDefault=10.0;
const double slowSpeedDefault=1.0;
const double penSizeDefault=20.0;
const double iconOpacityDefault = .75;

const double iconIncrementDefault =.5;

class CrayonApp : public CrayonRenderer
{

private:

    Timer timer;


    ///\todo what do these get initialized to?
    PenLocation penLocation;
    PenLocation lastPenLocation;

    bool penDown;


    bool drewALine();

    unsigned int currentCrayon;

    
protected:
   
    
   
  
    
public:
    
    inline bool isPenDown(){return penDown;}
    
    double iconDelta; //rate per second
    
    double iconOpacityScale;
   
    
    GLTexture magnifierIcon;
    GLTexture penIcon; //texture to render pen icon
    bool showPenIcon;
    GL::GLShader iconProg;

    ///defaults to false.  indicates if there is data to prompt the user to save.
    ///if there is a line image and not a new blank drawing, gets initialized to true.  gets set to true additionally
    /// whenever the user draws a line or clears the screen.
    ///gets reset to false when the user saves and gets reset on a new drawing to either true or false depending on if
    ///it's a blank drawing or not.
    ///also defaults to false when loading an image because there is no unsaved data yet.
    bool shouldSave;
    
    
    unsigned int resWidth;
    unsigned int resHeight;

    double penSize;

    double fastSpeed;
    double slowSpeed;
    

    Material material;

    static void initGLState();

    void initializeShaders();

    void setCrayonColorUniform();

    CrayonApp(unsigned int w, unsigned int h, const Material& initialMaterial);

    float pressureFromSpeed(float speedIn);

    void onPenDown(const PenLocation& loc);
    void onPenUp();

    void handlePenMove(const PenLocation& pin);

    void renderCrayonBuffers();

    void renderCompositedFrame();
    //void renderFrame();

    unsigned int getCurrentCrayon()const{return currentCrayon;}

    void setCurrentCrayon(unsigned int c);

    const std::string& currentCrayonName()const;
    const CrayonColor& currentCrayonColor()const;
    
    void erase();
    
    void pushBackground();



};


Material loadMaterial(const std::string& materialName, const std::string& materialPath, Crayon* beginCrayon, Crayon* endCrayon, std::pair<GLuint, GLuint> dimensions);

std::string grainFilePressure(const std::string& materialPath);

std::string grainFile(const std::string& materialPath);

std::string paperFile(const std::string& materialPath);

std::string grainFile(const std::string& materialPath, const std::string& suffix);

std::string paperFile(const std::string& materialPath, const std::string& suffix);

std::string grainFilePressure(const std::string& materialPath, const std::string& suffix);

void bindLocs(GL::GLShader& shader);

#endif
