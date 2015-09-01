#version 150

uniform sampler2D texture;
uniform sampler2D texture1;

in vec4 f_color;
in vec2 f_uv;
flat in int  f_texture;

out vec4 fragColor;

void main()
{
  if (f_texture == 0)
    fragColor = f_color * texture2D(texture, f_uv);
  else
    fragColor = f_color * texture2D(texture1, f_uv);
}



