#version 410 core

out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D texture1;

void main()
{
    FragColor = texture(texture1, TexCoord);
    //FragColor = texture(texture1, TexCoord) * vec4(ourColor, 1.0);
    //FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.4);
}
