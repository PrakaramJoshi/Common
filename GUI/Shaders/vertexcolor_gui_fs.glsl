#version 440 core
in vec4 colour;
out vec4 frag_colour;

void main () {
  frag_colour = vec4 (colour);
}