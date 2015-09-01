#version 150

uniform sampler2D texture;

varying vec4      v_color;

out vec4 fragColor;

void main()
{
  fragColor = vec4(1, 0, 0, 1);//v_color;// * texture2D(texture, gl_PointCoord);
}


