#version 130
varying vec4 fragment_position;
varying vec3 fragment_normal;
varying vec2 fragment_texcoord;

//uniform sampler2D env_texture;
uniform sampler2D tex_c;
uniform sampler2D tex_s;

uniform vec4 ambient = vec4(0.5, 0.5, 0.5, 1.0);
uniform vec4 diffuse = vec4(1.0, 1.0, 1.0, 1.0);
uniform vec4 specular = vec4(1.0, 1.0, 1.0, 1.0);
uniform float shininess = 50.0;

uniform vec4 lightPos = vec4(1.0, -1.0, 1.0, 1.0);

void main()
{	
	vec4 diffuseMap = texture(tex_c, fragment_texcoord);
	vec4 specularMap = texture(tex_s, fragment_texcoord);

	vec4 norm = vec4(fragment_normal, 0.0);

	vec4 s_eye = normalize(vec4(lightPos - fragment_position));
	vec4 v_eye = normalize(-fragment_position);
	vec4 r_eye = reflect(-s_eye, norm);
	vec4 Ia = vec4(0.1, 0.1, 0.1, 1.0) * ambient;
	vec4 Id = diffuse * diffuseMap * max(dot(s_eye, norm), 0.0);
	vec4 Is = specular * specularMap * pow(max(dot(r_eye, v_eye), 0.0), shininess);

	gl_FragColor = (Ia + Id + Is);
} 
