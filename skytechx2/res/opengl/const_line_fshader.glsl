#version 150

uniform sampler2D texture;

in vec4  v_pos;
in float v_u;

out vec4 fragColor;

void main()
{
  float i = 1.0;//(1 - sqrt(v_pos.x * v_pos.x + v_pos.y * v_pos.y));
  //gl_FragColor = vec4(i, i, i, i) * vec4(1, 0, 0, 1);

  fragColor = vec4(1.0, 1.0, 0.5, 1.0) * vec4(i, i, i, i) * texture2D(texture, vec2(v_u, 0.5));
}

