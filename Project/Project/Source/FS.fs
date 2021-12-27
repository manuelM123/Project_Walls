#version 330 core
out vec4 FragColor;

in vec2 TextCoords;
  
uniform sampler2D texture2;

void main()
{
    FragColor = texture(texture2,TextCoords);
} 

