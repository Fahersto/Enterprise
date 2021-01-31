#include "EP_PCH.h"

#include "Graphics.h"
#include "Window.h"

#include "WindowEvents.h"
#include "Enterprise/Application/ApplicationEvents.h"

using Enterprise::Graphics;

static const unsigned int QuadBatch_MaxQuads = 25000;
int Graphics::_maxTextureSlots;
int* Graphics::_textureSlots = nullptr;

static bool OnWindowClose(Events::Event& e)
{
	EP_ASSERT(e.Type() == EventTypes::WindowClose);

    // By default, closing the window is equivalent to quitting from the OS.
    Enterprise::Events::Dispatch(EventTypes::QuitRequested);
    return true;
}

void Graphics::Init()
{
	Events::SubscribeToType(EventTypes::WindowClose, OnWindowClose);
	Window::CreatePrimaryWindow();

	// Set up global VAO (OpenGL)
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Initialize texture slot tracking
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &_maxTextureSlots);
	_textureSlots = new int[_maxTextureSlots];
	memset(_textureSlots, 0, _maxTextureSlots);

	// QuadBatch VBO
	glGenBuffers(1, &_quadbatch_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _quadbatch_vbo);
	glBufferData(GL_ARRAY_BUFFER,
				 QuadBatch_MaxQuads * sizeof(QuadBatchDefaultVertex) * 4,
				 nullptr, GL_DYNAMIC_DRAW);

	// QuadBatch IBO
	glGenBuffers(1, &_quadbatch_ibo);
	unsigned int quadbatchindices[QuadBatch_MaxQuads * 6];
	for (unsigned int i = 0; i < QuadBatch_MaxQuads; i++)
	{
		quadbatchindices[i * 6]     = 4 * i;
		quadbatchindices[i * 6 + 1] = 4 * i + 1;
		quadbatchindices[i * 6 + 2] = 4 * i + 2;
		quadbatchindices[i * 6 + 3] = 4 * i + 2;
		quadbatchindices[i * 6 + 4] = 4 * i + 3;
		quadbatchindices[i * 6 + 5] = 4 * i;
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _quadbatch_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				 QuadBatch_MaxQuads * sizeof(unsigned int) * 6,
				 quadbatchindices, GL_STATIC_DRAW);


	glEnable(GL_BLEND); // blend mode
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// glEnable(GL_DEPTH_TEST); // depth test
	// glEnable(GL_CULL_FACE); // backface culling
}

void Graphics::Update()
{
	ClearRenderTarget();

	// Drawing code here

    Window::SwapBuffers();	 
}

void Graphics::Cleanup()
{
	delete[] _textureSlots;

    Window::DestroyPrimaryWindow();
}
