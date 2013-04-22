#version 130
varying vec4 fragment_position;
varying vec3 fragment_normal;
varying vec2 fragment_texcoord;

uniform sampler2D env_texture;
uniform sampler2D tex_c, tex_n, tex_s;


void main()
{   
    //Calculate illumination in the eye space
    vec3 light_source=vec3(0, 0, 0);
    vec3 V = -vec3(fragment_position);
	vec3 L = light_source - vec3(fragment_position);
	L = normalize(L);
	vec3 N = normalize(fragment_normal);
    float brightness=max(abs(dot(L,N)), 0.0);

	vec4 c = texture(tex_c, fragment_texcoord);
	vec4 s = texture(tex_s, fragment_texcoord);

    gl_FragColor = brightness * c + s;
} 
