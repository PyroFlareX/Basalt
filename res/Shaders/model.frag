#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : require

const int textureBindingSlot = 7;

struct vertexOutputData
{
	vec3 fragPos;
	vec3 normal;
	vec2 textureCoordinates;
};

//Fragment output
layout (location = 0) out vec4 FragColor;

//Input from vertex shader
layout (location = 0) in vertexOutputData outVertShader;

//Descriptor set buffers
layout ( set = 0, binding = 0 ) uniform MVP
{
	mat4 proj;
	mat4 view;
	mat4 model;
} CameraData;

layout ( set = 0, binding = textureBindingSlot ) uniform sampler2D textures[];

layout ( push_constant ) uniform constants
{
	vec4 textureid;
} PushConstants;

void main()
{
	vec3 lightColor = vec3(1.0, 1.0, 1.0);
	float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

	vec3 normal = texture(textures[int(PushConstants.textureid.y)], outVertShader.textureCoordinates).xyz;
	vec3 normal2 = 2 * normalize(outVertShader.normal);

	normal = normalize(normal + normal2);

	vec3 lightsrc = vec3(10.0, 10.0, 10.0);
	vec3 lightDir = normalize(lightsrc - outVertShader.fragPos);


	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;
	
	vec4 result = vec4(ambient + diffuse, 1.0);
	
	FragColor = texture(textures[int(PushConstants.textureid.x)], outVertShader.textureCoordinates) * result;
} 