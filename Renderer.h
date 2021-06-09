#pragma once

#include "glad/glad.h"

#include <iostream>

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"


#define ASSERT(x) if (!(x)) __debugbreak();
// find errors
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLCallLog(#x, __FILE__, __LINE__))  // # to convert into a string
	

void GLClearError();
bool GLCallLog(const char* function, const char* file, int line);

class Renderer
{
public:
	void Draw(const VertexArray& va, const Shader& shader, int count);
	void DrawElements(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, int count);
	void DrawCircle(const VertexArray& va, const Shader& shader, int count);
	void Clear();
};
