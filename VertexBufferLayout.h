#pragma once

#include <vector>
#include "Renderer.h"

struct VertexBufferElement
{
	unsigned int size;
	unsigned int type;
	bool normalized;

	static unsigned int GetSizeOfType(int type)
	{
		switch (type)
		{
		case GL_FLOAT:			return 4;
		case GL_UNSIGNED_INT:	return 4;
		case GL_UNSIGNED_BYTE:	return 1;
		}
		ASSERT(false);  // debug break
		return 0;
	}
};


class VertexBufferLayout
{
private:
	std::vector<VertexBufferElement> m_Elements;
	unsigned int m_Stride;

public:
	VertexBufferLayout()
		: m_Stride(0) {}

	template<typename T>
	void Push(unsigned int size)
	{
		static_assert(0, "ERROR: You can use only types that are listed below. VertexBufferLayout.h");
	}

	template<>
	void Push<float>(unsigned int size)
	{
		m_Elements.push_back({ size, GL_FLOAT, GL_FALSE });
		m_Stride += size * VertexBufferElement::GetSizeOfType(GL_FLOAT);
	}

	template<>
	void Push<unsigned int>(unsigned int size)
	{
		m_Elements.push_back({ size, GL_UNSIGNED_INT, GL_FALSE });
		m_Stride += size * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
	}

	template<>
	void Push<unsigned char>(unsigned int size)
	{
		m_Elements.push_back({ size, GL_UNSIGNED_BYTE, GL_TRUE });
		m_Stride += size * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
	}

	inline const std::vector<VertexBufferElement> GetElements() const { return m_Elements; }
	inline const unsigned int GetStride() const { return m_Stride; }
};