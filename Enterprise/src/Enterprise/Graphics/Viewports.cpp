#include "EP_PCH.h"
#include "Graphics.h"

using Enterprise::Graphics;


void Graphics::TargetViewportForRender(Math::Vec2 pos, Math::Vec2 size)
{
};

void Graphics::TargetTextureForRender(TextureRef texture)
{
};

void Graphics::ClearRenderTarget()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
};
