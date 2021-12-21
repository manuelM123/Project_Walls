#version 410 core

// interpolated values from the vertex shaders
in vec3 fragmentColor;

// output data
out vec3 color;

void main () {
	// output color = color specified in the vertex shader,
	// interpolated between all 3 surrounding vertices
	color = fragmentColor;
}