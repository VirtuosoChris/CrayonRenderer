//Copyright (c) 2013 Christopher Pugh. All rights reserved.

precision highp float;

uniform sampler2D iconTexture;
varying vec2   tc;
uniform vec4 color; //rgb is crayon color, alpha channel is overall opacity

void main(){
    
    //read alpha from icon
	vec4 iconLookup = texture2D(iconTexture, tc);
	
    gl_FragColor = iconLookup *color;
    
}