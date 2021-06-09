#include "Renderer.h"


// iterate through all errors to clear it
void GLClearError()
{
	// GL_NO_ERROR is guaranteed to be 0
	while (glGetError() != GL_NO_ERROR);
}

bool GLCallLog(const char* function, const char* file, int line)
{
	// look at line 7 ⬆
	while (GLenum error = glGetError())
	{
		std::cout << "[ERROR]:" << error << " | " << function << ", " << file << " | " << line;
		// error has been occured
		return false;
	}
	// no errors has been spoted
	return true;
}

void Renderer::Draw(const VertexArray& va, const Shader& shader, int count)
{
	va.Bind();
	shader.Bind();
	GLCall(glDrawArrays(GL_LINES, 0, count));
}

void Renderer::DrawElements(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, int count)
{
	va.Bind();
	ib.Bind();
	shader.Bind();
	GLCall(glDrawElements(GL_LINES, count, GL_UNSIGNED_INT, 0));
}

void Renderer::DrawCircle(const VertexArray& va, const Shader& shader, int count)
{
	va.Bind();
	shader.Bind();
	GLCall(glDrawArrays(GL_LINE_STRIP, 0, count));
}

void Renderer::Clear()
{
	GLCall(glEnable(GL_DEPTH_TEST));
	GLCall(glClearColor(0.1f, 0.65f, 0.55f, 1.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}
