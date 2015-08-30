//#version 120

//  Copyright (c) 2013 Christopher Pugh. All rights reserved.

precision highp float;

uniform sampler2D grainTexture; 
uniform sampler2D grainTexturePressure;
uniform sampler2D crayonBuffer;//color from last frame

uniform vec2 screenDims;
uniform vec3 color;

uniform float crayonPressure;

varying vec2   tc;
varying vec2 screenCoords;

uniform float scale;

uniform vec2 offsetNorm;
uniform vec2 lastPenLocationClip;

uniform float invPenRadiusClip;

uniform float zoomLevel;

uniform vec2 boxLocation;


void main(){
    
   
    
    float pressureLookup = texture2D(grainTexturePressure,screenCoords).r;
    float grainLookup = texture2D(grainTexture,screenCoords).r;
	vec4 crayonLookup = texture2D(crayonBuffer, screenCoords);
	 
    vec2 screenCoordClip = 2.0*(screenCoords - vec2(.5));
    
    //go to normalized coords
    vec2 lastPenLocationClip2 = .5 * (lastPenLocationClip + vec2(1.0));
    
    //scale, bias for zoom
    lastPenLocationClip2 = (zoomLevel * lastPenLocationClip2) + boxLocation;
    
    //back to clip
    lastPenLocationClip2 = 2.0*(lastPenLocationClip2 - vec2(.5));
    
    
    vec2 vectorToProject = screenCoordClip - lastPenLocationClip2;
    
    float magnitude = abs(dot(offsetNorm, vectorToProject));
    
    float horizontalLocation = magnitude * invPenRadiusClip / zoomLevel;
    
    float gauss = 1.0 - horizontalLocation;//=1.0;
    
	//read the alpha we want from the grain texture
	float lum =  scale* mix(pressureLookup,grainLookup,	crayonPressure);
    
    vec3 alphaMult = (crayonLookup.a *  crayonLookup.rgb);

    
	float newAlpha =  gauss*lum*(1.0-crayonLookup.a);
	
    float alphaSum =  newAlpha+crayonLookup.a;
	float scale = 1.0 / (alphaSum );
	
	vec4 drawColor = vec4(
    scale*(
     (newAlpha * color)   +
     alphaMult
     )
	, alphaSum
	
	);
	
	gl_FragColor = drawColor;
}