#shader vertex
#version 330 core
layout(location = 0) in vec3 aPos;
uniform mat4 translation_matrix;
void main()
{
	gl_Position = translation_matrix * vec4(aPos, 1.0f);
}

#shader fragment
#version 330 core
out vec4 FragColor;
uniform vec4 u_color;
void main()
{
	FragColor = u_color;
}