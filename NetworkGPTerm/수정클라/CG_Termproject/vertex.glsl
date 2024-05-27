#version 330 core

layout(location = 0) in vec3 aPos;   // 위치 변수는 attribute position 0을 가집니다.
layout(location = 1)in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
uniform mat4 modeltrans;
uniform mat4 viewTransform;
uniform mat4 projectionTransform;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;


void main()
{
	gl_Position = projectionTransform * viewTransform * modeltrans * vec4(aPos, 1.0);
	FragPos = vec3(modeltrans * vec4(aPos, 1.0));
	Normal = aNormal;
	TexCoord =aTexCoord;
}