#include <cstdlib>
#include <fstream>
#include <string>
#include <GLWindow.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLShader.h>
#include <iostream>
#include <image.h>
#include <GLTexture.h>
#include <GpuMesh.h>
#include <Quad.h>

const unsigned int screenWidth = 1024;
const unsigned int screenHeight = 768;

void screenQuad()
{
    static Virtuoso::Quad q;
    static Virtuoso::GPUMesh quad(q);

    quad.push();

}


int main(void)
{

    try
    {


        glPixelStorei(GL_UNPACK_ALIGNMENT,1);
        glPixelStorei(GL_PACK_ALIGNMENT,1);

        GLWindow::Settings setts;

        setts.width = screenWidth;
        setts.height = screenHeight;

        setts.match_resolution_exactly=true;
        setts.is_fullscreen=false;

        setts.glversion_major= 2;
        setts.glversion_minor= 1.0;

        GLWindow win(setts);

        glewInit();

        LDRImage noiseImg = loadImage("noise.png",1);

        flipVertical(noiseImg);

        glPixelStorei(GL_UNPACK_ALIGNMENT,1);
        glPixelStorei(GL_PACK_ALIGNMENT,1);

        GLTexture inputTex(noiseImg);

        int width = noiseImg.getDimensions()[1];
        int height = noiseImg.getDimensions()[2];

        std::cout<<"W H "<<width<<" "<<height<<std::endl;

        glBindTexture(GL_TEXTURE_2D, inputTex.tex);

        writeImage(noiseImg, "noiseOut_test.png");

        GL::GLShader median2("median.vert", "medianfilter2.frag");
        median2.bind();
        median2.setTexture("tex", 0);
        median2.setUniform("invTexDimensions", 1.0f / float(width), 1.0f / float(height));

        GL::GLShader median3("median.vert", "medianfilter3.frag");
        median3.bind();
        median3.setTexture("tex", 0);
        median3.setUniform("invTexDimensions", 1.0f / float(width), 1.0f / float(height));

        GL::GLShader median4("median.vert", "medianfilter4.frag");
        median4.bind();
        median4.setTexture("tex", 0);
        median4.setUniform("invTexDimensions", 1.0f / float(width), 1.0f / float(height));

        GL::GLShader median5("median.vert", "medianfilter5.frag");
        median5.bind();
        median5.setTexture("tex", 0);
        median5.setUniform("invTexDimensions", 1.0f / float(width), 1.0f / float(height));

        median4.bind();


std::cout<<"BEGIN "<<std::endl;
        {

            while(win.isValid())
            {

      /*
#ifndef GL_ES_BUILD
                const GLenum buffers[]= {GL_COLOR_ATTACHMENT0, GL_BACK_LEFT};

                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

                glDrawBuffers(1, &buffers[1]);

#else*/


                glBindFramebuffer(GL_FRAMEBUFFER, 0);
//#endif




            glClearColor(1,0,0,1);
            glClear(GL_COLOR_BUFFER_BIT);

            screenQuad();

            //    checkFBOErrors();


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
