#version 150

in  vec4 f_color;

out vec4 fragColor;

void main()
{
  fragColor = vec4(f_color.rgb, 1);
}


