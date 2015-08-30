//#version 120

//  Copyright (c) 2013 Christopher Pugh. All rights reserved.

precision highp float;

uniform sampler2D crayonBuffer;//color from last frame

uniform vec2 offsetNorm;
uniform vec2 lastPenLocationClip;

uniform float invPenRadiusClip;

uniform float zoomLevel;
uniform vec2 boxLocation;

varying vec2   tc;
varying vec2 screenCoords;


void main(){
    

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
    
    float gauss =horizontalLocation;
    
    float newAlpha = gauss;
	
    float alphaSum =  newAlpha  * crayonLookup.a;
    
	vec4 drawColor = vec4(crayonLookup.rgb,alphaSum);
    
	gl_FragColor = drawColor;
	
    
}