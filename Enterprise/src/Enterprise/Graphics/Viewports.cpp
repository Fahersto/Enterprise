#include "EP_PCH.h"
#include "Graphics.h"

#include "OpenGLHelpers.h"

using Enterprise::Graphics;


void Graphics::TargetViewportForRender(Math::Vec2 pos, Math::Vec2 size)
{
};

void Graphics::TargetTextureForRender(TextureRef texture)
{
};

void Graphics::ClearRenderTarget()
{
	EP_GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
};
