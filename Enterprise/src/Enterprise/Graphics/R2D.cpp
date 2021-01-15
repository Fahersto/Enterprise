#include "EP_PCH.h"
#include "Graphics.h"

using Enterprise::Graphics;


// Batch quad drawing

void Graphics::R2D::BeginBatch()
{
}

void Graphics::R2D::BatchQuad(Math::Vec2 pos, Math::Vec2 size,
							  Math::Vec4 color, Graphics::TextureRef tex, Math::Vec2 UV_TopLeft, Math::Vec2 UV_BottomRight)
{
}

void Graphics::R2D::BatchRotatedQuad(Math::Vec2 pos, Math::Vec2 size,
									 Math::Vec2 origin, float rot,
									 Math::Vec4 color, Graphics::TextureRef tex, Math::Vec2 UV_TopLeft, Math::Vec2 UV_BottomRight)
{
}

void Graphics::R2D::EndBatch()
{
}
