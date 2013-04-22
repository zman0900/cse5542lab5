#version 130
attribute vec4 vpos;     //In the local space
attribute vec3 vnorm;    //In the local space
attribute vec2 vtex;     //In the local space

varying vec4 fragment_position;
varying vec3 fragment_normal;
varying vec2 fragment_texcoord;

void main()
{ 
	fragment_position = (gl_ModelViewMatrix * vpos); //In the eye space
	fragment_normal   = gl_NormalMatrix * vnorm;   //In the eye space
	fragment_texcoord = vtex;
	gl_Position = gl_ModelViewProjectionMatrix * vpos;   //In the clip space
}
