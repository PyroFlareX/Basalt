#version 450
#extension GL_ARB_separate_shader_objects : enable


layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

layout (location = 2) out vec2 textureCoordinates;
layout (location = 1) out vec3 normal;
layout (location = 0) out vec3 fragPos;

layout ( push_constant ) uniform constants
{
    mat4 model;
    mat4 view;
    mat4 proj;
} PushConstants;

vec3 colors[3] = vec3[](
	vec3(1.0, 0.0, 0.0),
	vec3(0.0, 1.0, 0.0),
	vec3(0.0, 0.0, 1.0)
	);

//uniform mat3 normmat;
//uniform vec3 lightsrc;

void main()
{
	fragPos = aPos; //vec3(PushConstants.model * vec4(aPos, 1.0));
	textureCoordinates = aTexCoord;
	normal = aNormal; //vec3(PushConstants.model * vec4(aNormal, 0.0));
	gl_Position = PushConstants.proj * PushConstants.view * /*PushConstants.model */ vec4(aPos, 1.0);
}