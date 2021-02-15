#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

//vec3 smoothnoise()
//{
//	vec3 output = vec3(0.0f);
//	output.x = sin(1.0f);
//	output.z = output.x * cos(output.x);
//	output.y = cos(output.z) * sin(output.z * output.x + 5 * cos(output.x * output.z));
//	return output;
//}

void main() {
	outColor = vec4(fragColor, 1.0);
}