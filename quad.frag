//#version 120

//  Copyright (c) 2013 Christopher Pugh. All rights reserved.

precision highp float;
uniform sampler2D tex;
varying   vec2 tc;

void main(){
 
	gl_FragColor = texture2D(tex, tc.st);
}