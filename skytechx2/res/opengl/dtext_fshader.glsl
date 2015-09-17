#version 150

uniform sampler2D texture;

in vec2 g_uv;

out vec4 fragColor;

void main()
{
  fragColor = texture2D(texture, g_uv);
}



