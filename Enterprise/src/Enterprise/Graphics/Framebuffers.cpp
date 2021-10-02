#include "EP_PCH.h"
#include "Graphics.h"
#include "OpenGLHelpers.h"

using Enterprise::Graphics;

// TODO: Implement framebuffers

Graphics::FramebufferHandle Graphics::CreateFramebuffer(int width, int height, std::initializer_list<FramebufferAttachmentSpec> attachments)
{
	return 0;
}

void Graphics::DeleteFramebuffer(FramebufferHandle fb)
{}

void Graphics::PushFramebuffer(FramebufferHandle fb)
{}

void Graphics::PopFramebuffer(FramebufferHandle fb)
{}


Graphics::TextureHandle Graphics::GetColorTexture(FramebufferHandle fb, int number)
{
	return 0;
}

Graphics::TextureHandle Graphics::GetDepthTexture(FramebufferHandle fb)
{
	return 0;
}


int Graphics::GetFramebufferWidth(FramebufferHandle fb)
{
	return 0;
}

int Graphics::GetFramebufferHeight(FramebufferHandle fb)
{
	return 0;
}

void Graphics::ResizeFramebuffer(FramebufferHandle fb, int width, int height)
{}


void Graphics::SetViewport(int x, int y, int width, int height)
{}


void Graphics::SetBlendMode(BlendMode mode)
{
	static BlendMode currentMode = BlendMode::Opaque;

	switch(mode)
	{
		case BlendMode::Opaque:
			if (currentMode != BlendMode::Opaque)
			{
				EP_GL(glDisable(GL_BLEND));
				currentMode = BlendMode::Opaque;
			}
			break;
		case BlendMode::Translucent:
			if (currentMode == BlendMode::Opaque)
			{
				EP_GL(glEnable(GL_BLEND));
			}
			if (currentMode != BlendMode::Translucent)
			{
				EP_GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
				currentMode = BlendMode::Translucent;
			}
			break;
		case BlendMode::Additive:
			if (currentMode == BlendMode::Opaque)
			{
				EP_GL(glEnable(GL_BLEND));
			}
			if (currentMode != BlendMode::Additive)
			{
				EP_GL(glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA));
				currentMode = BlendMode::Additive;
			}
			break;
		case BlendMode::Multiply:
			if (currentMode == BlendMode::Opaque)
			{
				EP_GL(glEnable(GL_BLEND));
			}
			if (currentMode != BlendMode::Multiply)
			{
				EP_GL(glBlendFunc(GL_DST_COLOR, GL_ZERO));
				currentMode = BlendMode::Multiply;
			}
			break;
	}
}

void Graphics::SetDepthTest(bool enable)
{
	static bool isEnabled = false;

	if (enable && !isEnabled)
	{
		EP_GL(glEnable(GL_DEPTH_TEST));
		isEnabled = true;
	}
	else if (!enable && isEnabled)
	{
		EP_GL(glDisable(GL_DEPTH_TEST));
		isEnabled = false;
	}
}
