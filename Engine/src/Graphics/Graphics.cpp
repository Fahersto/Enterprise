
#include "Enterprise/Graphics.h"
#include "Enterprise/Graphics/OpenGLHelpers.h"
#include "Enterprise/Window.h"
#include "Enterprise/Events.h"

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


void Graphics::Init()
{
	fbWidths[0] = 0;
	fbHeights[0] = 0;

	// Get maximum number of Uniform Buffer binding points
	EP_GL(glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &maxUniformBufferSize));
	EP_INFO("[OpenGL] Max uniform buffer size: {}", maxUniformBufferSize);
	EP_GL(glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxUniformBindingPoints));
	EP_INFO("[OpenGL] Uniform buffer binding points: {}", maxUniformBindingPoints);

	// Set up global VAO (OpenGL)
	GLuint vao;
	EP_GL(glGenVertexArrays(1, &vao));
	EP_GL(glBindVertexArray(vao));

#ifndef EP_CONFIG_RELEASE
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
		glm::mat4(1.0f),
		glm::mat4(1.0f),
		glm::mat4(1.0f),
		glm::vec3()
	);
	perCameraGlobalUB = CreateUniformBuffer(HN("EP_PERCAMERA"), sizeof(perCameraGlobalUBStruct), &perCameraGlobalUBDataStack.top(), true);

	perDrawGlobalUBData.ep_matrix_m = glm::mat4(1.0f);
	perDrawGlobalUBData.ep_matrix_mv = glm::mat4(1.0f);
	perDrawGlobalUBData.ep_matrix_mvp = glm::mat4(1.0f);
	perDrawGlobalUB = CreateUniformBuffer(HN("EP_PERDRAW"), sizeof(perDrawGlobalUBStruct), &perDrawGlobalUBData, true);

	// Load default fallback shader
	CompileShaderSrc
	(
R"GLSL(
#epshader EPNULLSHADER
#include <Enterprise.glsl>
#vertex
void main()
{
    gl_Position = ep_matrix_mvp * vec4(ep_position, 1.0f);
}
#fragment
out vec4 out_color;
void main()
{
	out_color = vec4(1.0, 0.0, 1.0, 1.0);
}
)GLSL"
	);
	BindShader(HN("EPNULLSHADER"));
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
}
