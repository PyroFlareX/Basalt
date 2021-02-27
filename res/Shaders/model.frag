#version 450
#extension GL_ARB_separate_shader_objects : enable


layout (location = 0) out vec4 FragColor;

layout (location = 0) in vec2 textureCoordinates;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 fragPos;

//uniform sampler2D texturec;
//uniform vec3 lightsrc;

void main()
{
	vec3 lightColor = vec3(1.0, 1.0, 1.0);
	float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
	vec3 norm = normalize(normal);

	//vec3 lightDir = normalize(lightsrc - fragPos);

    vec3 lightDir = normalize(vec3(0.0, 0.0, 0.0) - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;
	
	vec4 result = vec4(ambient + diffuse, 1.0);
	
	//FragColor = texture(texturec, textureCoordinates) * result;

	FragColor = vec4(fragPos, 1.0);// * result;
} 