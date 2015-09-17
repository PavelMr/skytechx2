#version 150

uniform sampler2D f_texture;
uniform vec4 f_color;

in vec2 f_uv;
in vec4 f1_color;

out vec4 fragColor;

void main()
{
  fragColor = texture2D(f_texture, f_uv);// * f1_color;
  //gl_FragColor = vec4(1,1,1,0.5);
}


