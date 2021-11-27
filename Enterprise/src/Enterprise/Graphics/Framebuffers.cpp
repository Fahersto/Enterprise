#include "EP_PCH.h"
#include "Graphics.h"
#include "Window.h"
#include "OpenGLHelpers.h"

using Enterprise::Graphics;
using Enterprise::Window;

static std::deque<Graphics::FramebufferHandle> fbStack = { 0 };
std::deque<glm::vec4> Graphics::fbViewports;
std::map<Graphics::FramebufferHandle, int> Graphics::fbWidths;
std::map<Graphics::FramebufferHandle, int> Graphics::fbHeights;

// First: isTexture Second: ogl texture or rbo name
static std::map<Graphics::FramebufferHandle, std::vector<std::pair<bool, GLuint>>> colorAttachmentHandles;
static std::map<Graphics::FramebufferHandle, std::pair<bool, GLuint>> depthAttachmentHandles;

extern std::map<Graphics::TextureHandle, GLint> samplerTypeNeededForTexture;

Graphics::FramebufferHandle Graphics::CreateFramebuffer(std::initializer_list<FramebufferAttachmentSpec> attachments, int width, int height)
{
	if (width == 0 && height == 0)
	{
		// TODO: Link resolution to window width and height changes
		width = Window::GetWidth();
		height = Window::GetHeight();
	}

	EP_ASSERT(attachments.size() > 0);

	GLuint framebufferHandle;
	EP_GL(glGenFramebuffers(1, &framebufferHandle));
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferHandle);

	int colorAttachmentCount = 0;
	bool handledDepthAttachment = false;

	for (const FramebufferAttachmentSpec& a : attachments)
	{
		if (a.format == ImageFormat::Depth16 ||
			a.format == ImageFormat::Depth24 ||
			a.format == ImageFormat::Depth32 ||
			a.format == ImageFormat::Depth32F ||
			a.format == ImageFormat::Depth24Stencil8)
		{
			if (handledDepthAttachment)
			{
				EP_WARN("Graphics::CreateFramebuffer(): Attempted to create framebuffer with more than one depth attachment!");
				EP_DEBUGBREAK();
				continue;
			}
			else
			{
				handledDepthAttachment = true;
			}
		}

		if (a.makeTexture)
		{
			GLuint tex;
			glGenTextures(1, &tex);
			glBindTexture(GL_TEXTURE_2D, tex);
			glTexImage2D(GL_TEXTURE_2D, 0,
				imageFmtToOGLInternalFmt(a.format),
				width, height, 0,
				imageFmtToOGLImageFmt(a.format),
				imageFmtToOGLPixelDataType(a.format),
				nullptr);

			EP_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texFilterToOGLFilter(a.minFilter)));
			EP_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texFilterToOGLFilter(a.magFilter)));
			EP_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
			EP_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

			switch(a.format)
			{
			case ImageFormat::Depth16: case ImageFormat::Depth24: case ImageFormat::Depth32: case ImageFormat::Depth32F:
				// Depth-only formats
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex, 0);
				samplerTypeNeededForTexture[tex] = GL_SAMPLER_2D;
				depthAttachmentHandles[framebufferHandle] = { true, tex };
				break;
			case ImageFormat::Depth24Stencil8:
				// Depth + Stencil format
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, tex, 0);
				samplerTypeNeededForTexture[tex] = GL_SAMPLER_2D;
				depthAttachmentHandles[framebufferHandle] = { true, tex };
				break;
			default:
				// Color formats
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + colorAttachmentCount, GL_TEXTURE_2D, tex, 0);
				samplerTypeNeededForTexture[tex] = GL_SAMPLER_2D;
				colorAttachmentHandles[framebufferHandle].push_back({ true, tex });
				break;
			}

			currentSlotOfTexture[tex] = -1;
		}
		else // Renderbuffer
		{
			GLuint rbo;
			EP_GL(glGenRenderbuffers(1, &rbo));
			EP_GL(glBindRenderbuffer(GL_RENDERBUFFER, rbo));
			EP_GL(glRenderbufferStorage(GL_RENDERBUFFER, imageFmtToOGLInternalFmt(a.format), width, height));

			switch (a.format)
			{
			case ImageFormat::Depth16: case ImageFormat::Depth24: case ImageFormat::Depth32: case ImageFormat::Depth32F:
				// Depth-only formats
				EP_GL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo));
				break;
			case ImageFormat::Depth24Stencil8:
				// Depth + Stencil format
				EP_GL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo));
				break;
			default:
				// Color formats
				EP_GL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + colorAttachmentCount, GL_RENDERBUFFER, rbo));
				break;
			}
		}

		colorAttachmentCount++;
	}

	GLenum check = EP_GL(glCheckFramebufferStatus(GL_FRAMEBUFFER));
	if (check != GL_FRAMEBUFFER_COMPLETE)
	{
		EP_ERROR("Graphics::CreateFramebuffer(): Framebuffer is not complete!");
		EP_DEBUGBREAK();

		// Delete textures and rbos
		for (const auto& [isTexture, oglHandle] : colorAttachmentHandles[framebufferHandle])
		{
			if (isTexture)
			{
				EP_GL(glDeleteTextures(1, &oglHandle));
			}
			else
			{
				EP_GL(glDeleteRenderbuffers(1, &oglHandle));
			}
		}
		colorAttachmentHandles.erase(framebufferHandle);

		if (depthAttachmentHandles[framebufferHandle].first)
		{
			EP_GL(glDeleteTextures(1, &depthAttachmentHandles[framebufferHandle].second));
		}
		else
		{
			EP_GL(glDeleteTextures(1, &depthAttachmentHandles[framebufferHandle].second));
		}
		depthAttachmentHandles.erase(framebufferHandle);

		return 0;
	}

	EP_GL(glBindTexture(GL_TEXTURE_2D, textureInSlot[lastBoundTextureSlot]));
	EP_GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	fbWidths[framebufferHandle] = width;
	fbHeights[framebufferHandle] = height;
	return framebufferHandle;
}

void Graphics::DeleteFramebuffer(FramebufferHandle fb)
{
	if (colorAttachmentHandles.count(fb) == 0 && depthAttachmentHandles.count(fb) == 0)
	{
		EP_ERROR("Graphics::DeleteFramebuffer(): FramebufferHandle does not exist!");
		return;
	}
	else
	{
		for (const FramebufferHandle& stackFb : fbStack)
		{
			if (fb == stackFb)
			{
				EP_ERROR("Graphics::DeleteFramebuffer(): Attempted to delete a framebuffer that is currently in the stack!");
				return;
			}
		}
	}

	fbWidths.erase(fb);
	fbHeights.erase(fb);

	for (const auto& [isTexture, oglHandle] : colorAttachmentHandles[fb])
	{
		if (isTexture)
		{
			EP_GL(glDeleteTextures(1, &oglHandle));
		}
		else
		{
			EP_GL(glDeleteRenderbuffers(1, &oglHandle));
		}
	}
	colorAttachmentHandles.erase(fb);

	if (depthAttachmentHandles[fb].first)
	{
		EP_GL(glDeleteTextures(1, &depthAttachmentHandles[fb].second));
	}
	else
	{
		EP_GL(glDeleteTextures(1, &depthAttachmentHandles[fb].second));
	}
	depthAttachmentHandles.erase(fb);

	EP_GL(glDeleteFramebuffers(1, &fb));
}

void Graphics::PushFramebuffer(FramebufferHandle fb)
{
	EP_GL(glBindFramebuffer(GL_FRAMEBUFFER, fb));
	fbStack.emplace_back(fb);
	fbViewports.emplace_back();
}

void Graphics::PushFramebuffer(FramebufferHandle fb, float l, float r, float b, float t)
{
	EP_GL(glBindFramebuffer(GL_FRAMEBUFFER, fb));
	fbStack.emplace_back(fb);
	SetViewport(l, r, b, t);
	fbViewports.emplace_back(glm::vec4(l, r, b, t));
}

void Graphics::PopFramebuffer()
{
	if (fbStack.size() > 1) // Index 0 is default framebuffer '0'
	{
		fbStack.pop_back();
		fbViewports.pop_back();
		EP_GL(glBindFramebuffer(GL_FRAMEBUFFER, fbStack.back()));
		SetViewport(fbViewports.back().x,
					fbViewports.back().y,
					fbViewports.back().z,
					fbViewports.back().w);
	}
	else
	{
		EP_WARN("Graphics::PopFramebuffer(): There are no framebuffers on the stack!");
	}
}


Graphics::TextureHandle Graphics::GetColorTexture(FramebufferHandle fb, int number)
{
	if (colorAttachmentHandles.count(fb))
	{
		if (number < colorAttachmentHandles[fb].size())
		{
			if (colorAttachmentHandles[fb][number].first)
			{
				return colorAttachmentHandles[fb][number].second;
			}
			else
			{
				EP_WARN("Graphics::GetColorTexture(): Color attachment {} is not a texture!", number);
				EP_DEBUGBREAK();
				return 0;
			}
		}
		else
		{
			EP_WARN("Graphics::GetColorTexture(): Framebuffer does not contain color attachment with index '{}'.", number);
			EP_DEBUGBREAK();
			return 0;
		}
	}
	else
	{
		EP_WARN("Graphics::GetColorTexture(): FramebufferHandle does not exist!");
		EP_DEBUGBREAK();
		return 0;
	}
}

Graphics::TextureHandle Graphics::GetDepthTexture(FramebufferHandle fb)
{
	if (depthAttachmentHandles.count(fb))
	{
		if (depthAttachmentHandles[fb].first)
		{
			return depthAttachmentHandles[fb].second;
		}
		else
		{
			EP_WARN("Graphics::GetDepthTexture(): Framebuffer depth attachment is not a texture.");
			EP_DEBUGBREAK();
			return 0;
		}
	}
	else
	{
		EP_WARN("Graphics::GetDepthTexture(): Framebuffer handle does not exist or does not have a depth attachment!");
		EP_DEBUGBREAK();
		return 0;
	}
}


int Graphics::GetFramebufferWidth(FramebufferHandle fb)
{
	if (fbWidths.count(fb))
	{
		return fbWidths[fb];
	}
	else
	{
		EP_WARN("Graphics::GetFramebufferWidth(): Framebuffer handle does not exist!");
		return 0;
	}
}

int Graphics::GetFramebufferHeight(FramebufferHandle fb)
{
	if (fbHeights.count(fb))
	{
		return fbHeights[fb];
	}
	else
	{
		EP_WARN("Graphics::GetFramebufferHeights(): Framebuffer handle does not exist!");
		return 0;
	}
}

void Graphics::ResizeFramebuffer(FramebufferHandle fb, int width, int height)
{
	// TODO: Implement
}


void Graphics::SetViewport(float l, float r, float b, float t)
{
	int& width = fbWidths[fbStack.back()];
	int& height = fbHeights[fbStack.back()];

	EP_GL(glViewport(l * width, b * height, r * width, t * height));
	fbViewports.back() = { l, r, b, t };
}


void Graphics::ClearCurrentViewport(glm::vec4 color)
{
	// TODO: Implement scissor test

	static glm::vec4 lastSetColor = glm::vec4(0.0f);
	if (color != lastSetColor)
	{
		EP_GL(glClearColor(color.r, color.g, color.b, color.a));
		lastSetColor = color;
	}

	EP_GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

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

void Graphics::SetBackfaceCulling(bool enable)
{
	static bool isBackfaceCullEnabled = false;
	if (isBackfaceCullEnabled != enable)
	{
		if (enable)
		{
			EP_GL(glEnable(GL_CULL_FACE));
		}
		else
		{
			EP_GL(glDisable(GL_CULL_FACE));
		}

		isBackfaceCullEnabled = enable;
	}
}
