//#version 120


//  Copyright (c) 2013 Christopher Pugh. All rights reserved.
precision highp float;

uniform sampler2D crayonBuffer;
uniform sampler2D paper;

varying vec2   tc;
varying vec2 tcLines;

uniform bool linesPresent;
uniform sampler2D lineTexture;

uniform float zoomLevel;
 

void main(){
  
    vec2 tc2 = tc;
    
	const float alphaCutoff = 0.0;//.15;
	 
	vec4 paperLookup = texture2D(paper, tc2);
	
	vec4 crayonLookup =  texture2D(crayonBuffer, tc2);
	
	float opacityLookup =crayonLookup.a;
	
	opacityLookup = opacityLookup < alphaCutoff ? 0.0 :opacityLookup;
    
    bool coordsOK = ( tcLines.x >0.0 && tcLines.x < 1.0) && ( tcLines.y >0.0 && tcLines.y < 1.0);
    
    float lineMask = linesPresent  && coordsOK? texture2D(lineTexture, tcLines).r : 1.0;

    gl_FragColor= vec4(mix(  paperLookup.rgb,  vec3(1.0,1.0,1.0) - crayonLookup.rgb, opacityLookup), 1.0);

    gl_FragColor.rgb *= lineMask;
}