#include <Histogram.h>
#include <Image.h>
#include <ImageProcessing.h>
#include <Gaussian.h>

#include <fstream>




#define WEIGHTED_HISTOGRAMS

///For each paper texture
///--convert to grayscale
///--equalize the local intensity to the global intensity
///--given some number of input samples of crayon, make a histogram.
///--match the histogram of the grain texture to the crayon histogram.


float targetSTDEVFalloff(unsigned int pixelsAcross)
{

    //want to fade to 0 at 3 stdev.

    return  float(pixelsAcross>>1) / 3.0;

}



float gaussianUnNormalized(float incoord, float stdev, float relevantDimensionLength)
{


    float distanceX =   (incoord -( .5 * relevantDimensionLength));

    //float a = 1.0 / sqrt(2.0 * 3.1415 * stdev * stdev);

    float exponent = (-distanceX * distanceX )/ (2.0 * stdev * stdev);

    return exp(exponent) ;

}

template <class DATATYPE>
void histogramToCSV(const Histogram<DATATYPE>& hist, const std::string& filename)
{

    std::ofstream fout(filename.c_str());

    for(std::size_t i =0; i < hist.numBins(); i++)
    {

        fout<<hist[i]<<'\n';
    }
    fout.close();
}


double crayonSigma(const LDRImage& input)
{

    double ratePerHorizontalResolution = 16.0 / 4629.0;

    return ratePerHorizontalResolution * input.getDimensions()[1];

}


///returns a scalar value that brings input vector maximally close to
double bestFitVectorScale(double xIn, double yIn, double zIn,
                          double xOut, double yOut, double zOut)
{

    double num = xIn * xOut + yIn * yOut + zIn * zOut;
    double den = xIn * xIn + yIn * yIn + zIn + zIn ;

    return num / den;
}



///assumes an interpolation between the black crayon at (0,0,0) color and the paper color, taken from the global mean color of the blank
///paper image.  returns the best fit alpha value that gives the color taken from the sample image.
double colorMapToAlpha(double paperColR, double paperColG, double paperColB,
                       double finalColR, double finalColG, double finalColB)

{

    const double crayonR = 0.0;
    const double crayonG = 0.0;
    const double crayonB = 0.0;

    double rval =  bestFitVectorScale(paperColR - crayonR, paperColG - crayonG, paperColB - crayonB ,
                                      finalColR - crayonR, finalColG - crayonG, finalColB - crayonB
                                     );

    ///\todo see if this matters

    rval = std::min(1.0,rval);

    rval = std::max(0.0,rval);

    return rval;

}


///finds the mean rgb color of an entire image
void meanColorRGB(const LDRImage& img, double& r, double& g, double& b)
{

    std::cout<<"ADLKGJLSDGJ"<<std::endl;
    const std::size_t channels = img.getDimensions()[0];
    const std::size_t pixels = img.numElements() / channels ;

    std::cout<<"PIXELS CHANNELS "<<pixels<< "  "<<channels<<std::endl;

    double den = 1.0 / pixels;

    if(channels < 3)
    {

        throw std::runtime_error("Image must have at least 3 channels for the meanColorRGB method to be valid");
    }

    for(std::size_t i = 0; i < pixels; i++)
    {

        std::size_t base = channels * i;

        r += img[base] * den;
        g += img[base+1] * den;
        b += img[base+2] * den;

    }
    std::cerr<<"Mean color is "<<r<<" "<<g<<" "<<b<<std::endl;
}



LDRImage localToLuminance(const LDRImage& in)
{
    if(in.getDimensions()[0] >1)
    {

        return toLuminance(in);
    }
    else
    {
        return in;
    }
}



LDRImage highPassFilter(const LDRImage& img, double sigma =  16)
{

    HDRImage gray = localToLuminance(img); ///\todo what if the input is already in grayscale?

    //double globalMean = mean(gray);

    HDRImage lowpass = gaussianBlur2D(gray, sigma);///\todo does this refactor use color on gauss blurs and such? i forget

    double lpMean = mean(lowpass);

    for(std::size_t i = 0; i < gray.numElements(); i++)
        gray[i] = (gray[i] - lowpass[i])+lpMean;

    gray.clamp(0,255);

    return gray;

}



void createGrainTexture(const std::string& path,   Histogram<unsigned char>& histoTarget, LDRImage& grain,const std::string& grainFile)
{


    generateCDF(histoTarget,histoTarget); ///we have a normalized pdf of each alpha value; make it a cdf.

    Histogram<unsigned char> cdfIn(0,255,256);

    generateCDF(grain,cdfIn);

    matchCDF(grain,histoTarget, cdfIn);

    writeImage(grain, path + "/"+ grainFile);

#ifdef HIGHPASS_RESULT

    LDRImage equalizedGrain = highPassFilter(grain,crayonSigma(grain));

    writeImage(equalizedGrain,path + "/grain-highpass.png");
#endif // HIGHPASS_RESULT
}




///\todo grain copy assignment: indicates bug in image lib?  grain was pass by value for some reason.  this is the input texture for this process.
///no need. but for some reason having it be that way caused crashes.  maybe because of RAM?
void processSamples(const std::string& outputPath, const std::string& samplesFolder, const LDRImage& paperTexture,  LDRImage& grain, const std::string& grainFile)
{
    std::cout<<"PROCESSING SAMPLES"<<std::endl;
    double meanR=0,meanG=0,meanB=0;
    meanColorRGB(paperTexture,meanR,meanG,meanB);

    std::cout<<"MEAN COLOR IS "<<meanR<<" "<<meanG<<" "<<meanB<<std::endl;
    // std::string samplesFolder = path + "/samples/";


    Histogram<unsigned char> histo(0,255,256);

    std::size_t totalPixels = 0;

    try
    {

        for(int sampleI = 0; ; sampleI++)  ///for some arbitrary number of samples that exist with filenames given as consecutively numbered png files
        {

            std::cerr<<"\tProcessing wax sample "<<sampleI<<std::endl;

            std::stringstream sstr;

            sstr<<samplesFolder<<sampleI<<".png";

            LDRImage sample = loadImage(sstr.str());

            if(!sample)break;

            auto outDims = sample.getDimensions();
            outDims[0] = 1;
            LDRImage sampleOut(outDims);


            const std::size_t channels = sample.getDimensions()[0];
            const std::size_t pixels = sample.numElements() / channels ;



            if(channels < 3)
            {

                throw std::runtime_error("Image must have at least 3 channels for the meanColorRGB method to be valid");
            }

            totalPixels += pixels;

            for(std::size_t pix = 0; pix < pixels; pix++)
            {

                std::size_t base = channels * pix;

                unsigned char alphaOut = std::round(255 * colorMapToAlpha(meanR, meanG, meanB, sample[base], sample[base+1], sample[base+2]));
                alphaOut = ~alphaOut;
//                std::cerr<<"alpha out is "<<(int)alphaOut<<" neg is "<<(unsigned int)(~alphaOut)<<std::endl;
                sampleOut[pix] = alphaOut;


                LDRImage::index_type pixelIndex = unlinearizeMultidimensionalIndex(base, sample.getDimensions());

#ifdef WEIGHTED_HISTOGRAMS
                double gaussianWeightSample = 1.0;
                //x axis is longer, then falloff with the y axis coordinate
                if(sample.getDimensions()[1] > sample.getDimensions()[2])
                {
                    gaussianWeightSample = gaussianUnNormalized(pixelIndex[2],targetSTDEVFalloff(sample.getDimensions()[2]), sample.getDimensions()[2]);
                }
                else   //y axis is longer, then falloff with the x coordinate
                {
                    gaussianWeightSample  = gaussianUnNormalized(pixelIndex[1],targetSTDEVFalloff(sample.getDimensions()[1]), sample.getDimensions()[1]);
                }

#else
                const double gaussianWeightSample = 1.0; //if histograms are not weighted, then we
#endif

                //std::cerr<<"Gaussian sample weight is "<<gaussianWeightSample<<" for "<<pixelIndex<<" with image dims "<<sample.getDimensions()<<std::endl;

                histo[alphaOut] += gaussianWeightSample; ///we invert the alpha out here because we want low alpha to mean the paper is visible.

            }


            {
                ///write debug alpha maps out

                std::stringstream sstr2;

                sstr2<<  samplesFolder+ "debug/"<<sampleI<<".png";
                std::cerr<<"writing "<<sstr2.str()<<std::endl;
                writeImage(sampleOut, sstr2.str());

            }
        }


    }
    catch(std::exception& e)
    {
        std::cerr<<"end: "<<e.what()<<std::endl;
    }

    std::size_t pixAc =0;
    for(std::size_t i = 0; i < histo.numBins(); i++)
    {
        pixAc += histo[i];

    }

    histo.normalize();

    histogramToCSV(histo,outputPath + "/samples/debug/histo.csv");

    std::cerr<<"Creating grain texture"<<std::endl;
    createGrainTexture(outputPath, histo, grain, grainFile);

    ///we'll go through each pixel of each sample, and convert it to an estimated alpha value.
    ///we'll then

    ///\todo backproject the estimated alpha map to recreate the original image estimate

    ///final step is to take the histogram of alpha values and match the grain texture distribution to the alpha map

}

void makeMaterial(const std::string& path)
{

    std::string pathToInputImg = path + "/material.png";
    std::string pathToOutputImg = path + "/highpassed.png";

    LDRImage input = loadImage(pathToInputImg);

    if(!input){
        throw std::runtime_error("Unable to load input image");
    }

    if( !std::ifstream(pathToOutputImg))
    {
        std::cerr<<"Generating high passed version of image "<<std::endl;

        double sigmaToUse = crayonSigma(input);

        std::cerr<<"Using sigma of "<<sigmaToUse<<std::endl;

        LDRImage output = highPassFilter(input, sigmaToUse);

        writeImage(output, pathToOutputImg);

        processSamples(path, path + "/samples/" ,input, output, "grain.png");
        processSamples(path, path + "/samples/highpressure/" ,input, output, "grain-pressure.png");


    }
    else
    {
        std::cerr<<"Loading high passed version of image "<<std::endl;


        LDRImage output = loadImage(pathToOutputImg);

        std::cout<<"LOADED IMG with "<<output.getDimensions()<<std::endl;

        if(!input)std::cerr<<"ERROR LOADING INPUT"<<std::endl;

        if(!output)std::cerr<<"ERROR LOADING OUTPUT"<<std::endl;
        //processSamples(path, input, output);


        std::cout<<"\n\nPROCESSING SAMPLES 1"<<std::endl;
        std::string str1 = path + "/samples/";
        std::string str2 = "grain.png";
        std::cout<<"GOT PAST STRINGS"<<std::endl;

        processSamples(path, str1 ,input, output, str2);

        std::cout<<"PROCESSING SAMPLES 2"<<std::endl;
        processSamples(path, path + "/samples/highpressure/" ,input, output, "grain-pressure.png");


    }

}



