//#version 120
//  Copyright (c) 2013 Christopher Pugh. All rights reserved.


precision highp float;

attribute vec3 position;

attribute vec2 texcoord;

varying vec2 tc;

varying vec2 screenCoords;

uniform float zoomLevel;

uniform vec2 boxLocation;


void main(){
    
    tc =    texcoord;

    screenCoords = .5 * (position.xy + vec2(1.0));
    
    screenCoords = (zoomLevel * screenCoords) + boxLocation;
    
  	gl_Position = vec4(position,1.0);
	
    
}