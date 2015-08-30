//#version 120

//  Copyright (c) 2013 Christopher Pugh. All rights reserved.

precision highp float;

attribute vec3 position;
attribute vec2 texcoord;

varying vec2 tc;

uniform float zoomLevel;

uniform vec2 boxLocation;

void main(){
    
    vec2 texcoord2 =   boxLocation + (zoomLevel* texcoord);
    
    tc =    texcoord2;
    
	gl_Position = vec4(position,1.0);
}