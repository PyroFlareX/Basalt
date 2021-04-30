#version 450
#extension GL_ARB_separate_shader_objects : enable

struct outVert
{
	vec3 fragPos;
	vec3 normal;
	vec2 textureCoordinates;
};

layout (location = 0) out vec4 FragColor;

layout (location = 0) in outVert outVertShader;

layout (set = 0, binding = 0) uniform testbuffer
{
	float x;
	float y;
	float z;
	float w;
} testbufferdata;

layout (set = 0, binding = 1) uniform sampler2D texturec;


void main()
{
	vec3 lightColor = vec3(1.0, 1.0, 1.0);
	float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
	vec3 norm = normalize(outVertShader.normal);

	//vec3 lightDir = normalize(lightsrc - outVertShader.fragPos);

    vec3 lightDir = normalize(vec3(10.0, 10.0, 10.0) - outVertShader.fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;
	
	vec4 result = vec4(ambient + diffuse, 1.0);
	
	FragColor = texture(texturec, outVertShader.textureCoordinates) * result;

	//FragColor = vec4(outVertShader.fragPos, 1.0) * result;
} 