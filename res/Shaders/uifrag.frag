#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : require

const int textureBindingSlot = 7;

struct vertexOutputData
{
	vec2 textureCoordinates;
    vec4 color;
	vec4 textureID;
};

layout ( location = 0 ) out vec4 FragColor;

layout ( location = 0 ) in vertexOutputData outVertShader;

layout ( set = 0, binding = textureBindingSlot ) uniform sampler2D textures[];

void main()
{
	vec4 color = outVertShader.color;

	if(outVertShader.textureID.x != 0)
	{
		//color = texture(textures[int(outVertShader.textureID.x)], outVertShader.textureCoordinates);
		FragColor = texture(textures[int(outVertShader.textureID.x)], outVertShader.textureCoordinates) * color;
	}
	else
	{
		//Hardcoded to the index for the dedicated font texture | 0
		FragColor = texture(textures[int(0)], outVertShader.textureCoordinates) * color;
	}
} 