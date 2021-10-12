#include "EP_PCH.h"

#include "Graphics.h"
#include "Window.h"

#include "Enterprise/Events/Events.h"

#include "OpenGLHelpers.h"

using Enterprise::Graphics;
using Enterprise::Events;

GLint Graphics::maxUniformBufferSize = 0;
GLint Graphics::maxUniformBindingPoints = 0;

int Graphics::maxTextureSlots = 0;
Graphics::TextureHandle* Graphics::textureInSlot = nullptr;
bool* Graphics::isTextureSlotUsedThisDraw = nullptr;
int Graphics::lastBoundTextureSlot = 0;

Graphics::UniformBufferHandle Graphics::perCameraGlobalUB;
Graphics::UniformBufferHandle Graphics::perDrawGlobalUB;
Graphics::perDrawGlobalUBStruct Graphics::perDrawGlobalUBData;

static bool OnWindowClose(Events::Event& e)
{
	EP_ASSERT(e.Type() == HN("WindowClose"));

    // By default, closing the window is equivalent to quitting from the OS.
    Enterprise::Events::Dispatch(HN("QuitRequested"));
    return true;
}

void Graphics::Init()
{
	Window::CreatePrimaryWindow();
	Events::Subscribe(HN("WindowClose"), OnWindowClose);

	// Get maximum number of Uniform Buffer binding points
	EP_GL(glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &maxUniformBufferSize));
	EP_INFO("[OpenGL] Max uniform buffer size: {}", maxUniformBufferSize);
	EP_GL(glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxUniformBindingPoints));
	EP_INFO("[OpenGL] Uniform buffer binding points: {}", maxUniformBindingPoints);

	// Set up global VAO (OpenGL)
	GLuint vao;
	EP_GL(glGenVertexArrays(1, &vao));
	EP_GL(glBindVertexArray(vao));

#ifndef EP_CONFIG_DIST
	GLint maxVertexAttributes;
	EP_GL(glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttributes));
	if (maxVertexAttributes >= 32)
	{
		// Limitation is due to the use of 32-bit bit fields to track attribute enable status in VertexArrays.cpp
		EP_WARN("[OpenGL] Implementation supports more vertex attributes than Enterprise!");
	}
#endif

	// Check for anisotropic filtering support
	int NumberOfExtensions;
	EP_GL(glGetIntegerv(GL_NUM_EXTENSIONS, &NumberOfExtensions));
	for(int i = 0; i < NumberOfExtensions; i++)
	{
		const char* extensionName = (const char *)EP_GL(glGetStringi(GL_EXTENSIONS, i));
		if (std::strcmp(extensionName, "GL_EXT_texture_filter_anisotropic") == 0)
		{
			EP_INFO("[OpenGL] Anisotropic filtering supported!");
			// TODO: Implement anisotropic filtering
		}
	}

	// Initialize texture slot tracking variables
	EP_GL(glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureSlots));
	EP_INFO("[OpenGL] Available texture slots: {}", maxTextureSlots);
	textureInSlot = new TextureHandle[maxTextureSlots];
	memset(textureInSlot, 0, maxTextureSlots * sizeof(TextureHandle));
	isTextureSlotUsedThisDraw = new bool[maxTextureSlots];
	memset(isTextureSlotUsedThisDraw, 0, maxTextureSlots * sizeof(bool));

	// Universal uniform buffers
	perCameraGlobalUBDataStack.emplace
	(
		Math::Mat4::Identity(),
		Math::Mat4::Identity(),
		Math::Mat4::Identity(),
		0
	);
	perCameraGlobalUB = CreateUniformBuffer(HN("EP_PERCAMERA"), sizeof(perCameraGlobalUBStruct), &perCameraGlobalUBDataStack.top(), true);

	perDrawGlobalUBData.ep_matrix_m = Math::Mat4::Identity();
	perDrawGlobalUBData.ep_matrix_mv = Math::Mat4::Identity();
	perDrawGlobalUBData.ep_matrix_mvp = Math::Mat4::Identity();
	perDrawGlobalUB = CreateUniformBuffer(HN("EP_PERDRAW"), sizeof(perDrawGlobalUBStruct), &perDrawGlobalUBData, true);
}

void Graphics::PreDraw()
{
	memset(isTextureSlotUsedThisDraw, 0, maxTextureSlots * sizeof(bool));
}

void Graphics::Cleanup()
{
	DeleteUniformBuffer(perDrawGlobalUB);
	DeleteUniformBuffer(perCameraGlobalUB);

	delete[] textureInSlot;
	delete[] isTextureSlotUsedThisDraw;

    Window::DestroyPrimaryWindow();
}
