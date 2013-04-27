#version 130
varying vec4 fragment_position;
varying vec3 fragment_normal;
varying vec2 fragment_texcoord;

uniform sampler2D tex;

void main() {
	gl_FragColor = texture(tex, fragment_texcoord);
}
