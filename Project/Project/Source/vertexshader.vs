#version 330 core

// input vertex data, different for all executions of this shader
layout (location = 0) in vec3 vertexPosition_modelspace;
layout (location = 1) in vec3 vertexColor;

// output data, will be interpolated for each fragment
out vec3 fragmentColor;

// values that stay constant for the whole mesh
uniform mat4 mvp;

void main () {
	// output position of the vertex, in clip space: mvp * position
	gl_Position = mvp * vec4 (vertexPosition_modelspace, 1);

	// the color of each vertex will be interpolated
	// to produce the color of each fragment
	fragmentColor = vertexColor;
}