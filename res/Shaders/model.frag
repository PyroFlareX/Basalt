#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : require

struct outVert
{
	vec3 fragPos;
	vec3 normal;
	vec2 textureCoordinates;
	vec4 textureids;
};

layout (location = 0) out vec4 FragColor;

layout (location = 0) in outVert outVertShader;

layout (set = 0, binding = 0) uniform testbuffer
{
	mat4 proj;
	mat4 view;
	mat4 model;
} testbufferdata;

layout (set = 0, binding = 1) uniform sampler2D textures[];


void main()
{
	vec3 lightColor = vec3(1.0, 1.0, 1.0);
	float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

	vec3 normal = texture(textures[int(outVertShader.textureids.y)], outVertShader.textureCoordinates).xyz;
	normal = (normal + 1) / 2;
	vec3 normal2 = normalize(outVertShader.normal);

	normal = normalize(normal + normal2);

	vec3 lightsrc = vec3(10.0, 10.0, 10.0);
	vec3 lightDir = normalize(lightsrc - outVertShader.fragPos);

	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;
	
	vec4 result = vec4(ambient + diffuse, 1.0);
	
	FragColor = texture(textures[int(outVertShader.textureids.x)], outVertShader.textureCoordinates) * result;
} 