//#version 120

//  Copyright (c) 2013 Christopher Pugh. All rights reserved.

precision highp float;

attribute vec3 position;
attribute vec2 texcoord;


uniform highp float zoomLevel;

varying vec2 tc;

varying vec2 tcLines;


uniform mat3 texMat;


uniform vec2 boxLocation;



void main(){
   
    
    vec2 texcoord2 =    boxLocation+( zoomLevel* texcoord);
    
    
    tc =    texcoord2;

    tcLines =   (texMat * vec3( texcoord2,1.0)).st;
	
    
	gl_Position = vec4(position,1.0);
    
}