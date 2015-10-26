#version 440 core
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec4 vertex_colour;

// Inputs from vertex shader
out vec4 colour;

void main () {

    gl_Position = vec4(vertex_position,1.0);
	
	colour = vertex_colour;
	//ec4(vertex_colour.x,vertex_colour.y,vertex_colour.z,1.0);
}