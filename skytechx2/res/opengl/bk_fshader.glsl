#version 150
#extension GL_NV_shadow_samplers_cube : enable

uniform samplerCube texture;

in vec3 g_texCoord0;

out vec4 fragColor;

void main()
{
  fragColor = vec4(textureCube(texture, g_texCoord0).bgr, 1);
}



