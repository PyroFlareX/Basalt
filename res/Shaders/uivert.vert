#version 450

#extension GL_ARB_separate_shader_objects : enable

struct vertexOutputData
{
	vec2 textureCoordinates;
    vec4 color;
	vec4 textureID;
};

//Per-Vertex Inputs
layout (location = 0) in vec2 inPos;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec4 inColor;

layout (location = 0) out vertexOutputData outVertShader;

layout ( push_constant ) uniform constants
{
	vec2 scale;
	vec2 translate;
	vec4 textureID;
} PushConstants;

out gl_PerVertex 
{
	vec4 gl_Position;   
};

void main() 
{
    outVertShader.textureCoordinates = inUV;
    outVertShader.color = inColor;
	outVertShader.textureID = PushConstants.textureID;
	gl_Position = vec4(inPos * PushConstants.scale + PushConstants.translate, 0.0, 1.0);
}