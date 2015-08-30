#include <cstdlib>
#include <fstream>
#include <string>
#include <GLWindow.h>
#include <GL/glew.h>
#include <GL/gl.h>

#include "CrayonApp.h"
#include <iostream>


const unsigned int screenWidth = 1024;
const unsigned int screenHeight = 768;


std::shared_ptr<CrayonApp> appPtr;

int currentMaterial = 0;

int countMaterials()
{
    for(int i =0; ; i++)
    {
        if(!materialPaths[i].length())return i;
    }

}


void nextMaterial()
{

    static int numMaterials = countMaterials();

    currentMaterial++;

    currentMaterial%= numMaterials;

    appPtr->material = loadMaterial(materialPaths[currentMaterial],
                                    &crayonsForMaterial[currentMaterial][0],
                                    &crayonsForMaterial[currentMaterial][NUM_CRAYONS]
                                   );

}




void nextColor()
{

    auto val = appPtr->getCurrentCrayon();
    val++;

    val%= appPtr->material.numCrayons();


    appPtr->setCurrentCrayon(val);

    std::cerr<<"Changing to color "<<appPtr->currentCrayonName()<<std::endl;

}



void increasePenSize()
{
    appPtr->penSize *= 1.1;
    std::cerr<<"Pen size is now "<<appPtr->penSize<<std::endl;


}

void decreasePenSize()
{


    appPtr->penSize *= (1.0 / 1.1);
    std::cerr<<"Pen size is now "<<appPtr->penSize<<std::endl;


}


int main(void)
{

    try
    {

        GLWindow::Settings setts;

        setts.width = screenWidth;
        setts.height = screenHeight;

        setts.match_resolution_exactly=true;
        setts.is_fullscreen=false;

        setts.glversion_major= 2;
        setts.glversion_minor= 1.0;

        GLWindow win(setts);

        glewInit();


        appPtr.reset(new CrayonApp(screenWidth, screenHeight,
                                   loadMaterial(materialPaths[0],
                                                crayonsForMaterial[0],
                                                &crayonsForMaterial[0][NUM_CRAYONS]
                                               )
                                  )); //init the app object


        PCInputBinding binding;
        binding.bindMouseMove(std::bind(&CrayonApp::handlePenMove,appPtr.get(), std::placeholders::_1));
        binding.bindButtonDown(PCInputBinding::MOUSE_LEFT, std::bind(&CrayonApp::onPenDown, appPtr.get()));
        binding.bindButtonUp(PCInputBinding::MOUSE_LEFT, std::bind(&CrayonApp::onPenUp, appPtr.get()));

        binding.bindButtonUp('M', nextMaterial);
        binding.bindButtonUp('C', nextColor);
        binding.bindButtonUp('-', decreasePenSize);
        binding.bindButtonUp('+', increasePenSize);

        win.use_binding(binding);

        {


            while(win.isValid())
            {

                appPtr->renderCrayonBuffers();


#ifndef GL_ES_BUILD
                const GLenum buffers[]= {GL_COLOR_ATTACHMENT0, GL_BACK_LEFT};

                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

                glDrawBuffers(1, &buffers[1]);

#else
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif

                checkFBOErrors();


                appPtr->renderCompositedFrame();
                win.update();
                win.flush();
            }

        }//scope

    }
    catch(const std::exception& e)
    {

        std::cout<<e.what()<<std::endl;
    }
    catch(const std::string& str)
    {
        std::cout<<str<<std::endl;
    }
    return 0;






}
