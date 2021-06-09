#pragma once


class IndexBuffer
{
private:
	unsigned int m_RendererID;

public:
	IndexBuffer(const void* data, int count);
	~IndexBuffer();

	void Bind() const;
	void Unbind() const;
};
