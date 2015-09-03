#version 150

in vec4  a_positionWithUV;

out vec2 v_uv;

void main()
{
  gl_Position = vec4(a_positionWithUV.xy, 1, 1);
  v_uv = a_positionWithUV.zw;
}

