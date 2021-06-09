#include "Shader.h"

#include "Renderer.h"

#include <fstream>
#include <string>
#include <sstream>

Shader::Shader(const std::string& filepath)
{
	ShaderProgramSource source = ParseShader(filepath);
	std::cout << "VERTEX SHADER" << std::endl;
	std::cout << source.VertexSource << std::endl;
	std::cout << "FRAGMENT SHADER" << std::endl;
	std::cout << source.FragmentSource << std::endl;

	m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
	glDeleteProgram(m_RendererID);
}


ShaderProgramSource Shader::ParseShader(const std::string& file)
{
	std::ifstream stream(file);

	enum class ShaderType
	{
		NONE = -1, VERTEX, FRAGMENT
	};
	ShaderType type = ShaderType::NONE;

	std::stringstream string_stream[2];
	std::string line;
	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
		}
		else
		{
			if (type != ShaderType::NONE)
				string_stream[static_cast<int>(type)] << line << '\n';
		}
	}
	return { string_stream[0].str(), string_stream[1].str() };
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& shader)
{
	GLCall(unsigned int shader_object = glCreateShader(type));
	const char* source = shader.c_str();
	GLCall(glShaderSource(shader_object, 1, &source, nullptr));
	GLCall(glCompileShader(shader_object));

	int success;
	// allows the developer to query a shader for information given an object parameter
	GLCall(glGetShaderiv(shader_object, GL_COMPILE_STATUS, &success));
	if (!success)
	{
		int length;
		GLCall(glGetShaderiv(shader_object, GL_INFO_LOG_LENGTH, &length));
		char* message = reinterpret_cast<char*>(malloc(sizeof(char) * length));
		GLCall(glGetShaderInfoLog(shader_object, length, &length, message));
		std::cout << "Failed to compile " <<
			(type == GL_VERTEX_SHADER ? "vertex " : "fragment ") << "shader!";
		std::cout << message << std::endl;
	}
	return shader_object;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	GLCall(unsigned int vbo = CompileShader(GL_VERTEX_SHADER, vertexShader));
	GLCall(unsigned int fbo = CompileShader(GL_FRAGMENT_SHADER, fragmentShader));

	GLCall(unsigned int program = glCreateProgram());
	GLCall(glAttachShader(program, vbo));
	GLCall(glAttachShader(program, fbo));
	GLCall(glLinkProgram(program));

	GLCall(glDeleteShader(vbo));
	GLCall(glDeleteShader(fbo));

	return program;
}


void Shader::Bind() const
{
	GLCall(glUseProgram(m_RendererID));
}

void Shader::Unbind() const
{
	GLCall(glUseProgram(0));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

void Shader::SetUniformMat4f(const std::string& name, glm::mat4& matrix)
{
	// The second parameter specify how many matrices we're passing, which is one in this case. The third parameter specify transpose (switches the row and column)
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}

unsigned int Shader::GetUniformLocation(const std::string& name)
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];
	GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
	if (location == -1)
		std::cout << "Warning: uniform " << name << " doesn't exist!" << std::endl;
	
	m_UniformLocationCache[name] = location;
	return m_UniformLocationCache[name];
}
