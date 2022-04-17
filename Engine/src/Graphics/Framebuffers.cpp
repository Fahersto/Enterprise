#include "Enterprise/Graphics.h"
#include "Enterprise/Window.h"
#include "Enterprise/Graphics/OpenGLHelpers.h"

using Enterprise::Graphics;
using Enterprise::Window;

static Graphics::FramebufferHandle nextFbHandle = 1;
static std::list<GLuint> returnedFbHandles;

static std::map<Graphics::FramebufferHandle, std::vector<Graphics::FramebufferAttachmentSpec>> fbAttachmentSpecs;
std::map<Graphics::FramebufferHandle, int> Graphics::fbWidths;
std::map<Graphics::FramebufferHandle, int> Graphics::fbHeights;

static std::map<Graphics::FramebufferHandle, GLuint> fbos {{0, 0}};
static std::map<Graphics::FramebufferHandle, std::vector<std::pair<bool, GLuint>>> colorAttachments;
static std::map<Graphics::FramebufferHandle, std::pair<bool, GLuint>> depthAttachments;

static std::deque<Graphics::FramebufferHandle> fbStack = { 0 };
static std::deque<glm::vec4> viewportStack = {{0, 1, 0, 1}};

extern std::map<Graphics::TextureHandle, GLint> samplerTypeNeededForTexture;

Graphics::FramebufferHandle Graphics::CreateFramebuffer(std::vector<FramebufferAttachmentSpec> attachments, int width, int height, Graphics::FramebufferHandle fb)
{
	FramebufferHandle returnVal = 0;

	if (fb != 0)
	{
		if (fb >= nextFbHandle)
		{
			EP_ERROR("Graphics::CreateFramebuffer(): Specified FramebufferHandle \"{}\" has not been previously assigned!", fb);
			return 0;
		}
		else
		{
			for (auto rit = returnedFbHandles.rbegin(); rit != returnedFbHandles.rend(); ++rit)
			{
				if ((*rit) == fb)
				{
					returnVal = fb;
					break;
				}
				else
				{
					EP_ERROR("Graphics::CreateFramebuffer(): Specified FramebufferHandle \"{}\" is already in use!", fb);
					return 0;
				}
			}
		}
	}

	// TODO: Link resolution to window width and height changes

	EP_ASSERT(attachments.size() > 0);

	GLuint fboHandle;
	EP_GL(glGenFramebuffers(1, &fboHandle));
	glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);

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
				depthAttachments[fboHandle] = { true, tex };
				break;
			case ImageFormat::Depth24Stencil8:
				// Depth + Stencil format
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, tex, 0);
				samplerTypeNeededForTexture[tex] = GL_SAMPLER_2D;
				depthAttachments[fboHandle] = { true, tex };
				break;
			default:
				// Color formats
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + colorAttachmentCount, GL_TEXTURE_2D, tex, 0);
				samplerTypeNeededForTexture[tex] = GL_SAMPLER_2D;
				colorAttachments[fboHandle].push_back({ true, tex });
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
		for (const auto& [isTexture, oglHandle] : colorAttachments[fboHandle])
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
		colorAttachments.erase(fboHandle);

		if (depthAttachments[fboHandle].first)
		{
			EP_GL(glDeleteTextures(1, &depthAttachments[fboHandle].second));
		}
		else
		{
			EP_GL(glDeleteTextures(1, &depthAttachments[fboHandle].second));
		}
		depthAttachments.erase(fboHandle);

		return 0;
	}

	if (returnVal == 0)
	{
		if (returnedFbHandles.size() > 0)
		{
			returnVal = returnedFbHandles.back();
			returnedFbHandles.pop_back();
		}
		else
		{
			returnVal = nextFbHandle;
			nextFbHandle++;
		}
	}
	else
	{
		for (auto rit = returnedFbHandles.rbegin(); rit != returnedFbHandles.rend(); ++rit)
		{
			if ((*rit) == returnVal)
			{
				returnedFbHandles.erase(std::next(rit).base());
				break;
			}
		}
		returnVal = fboHandle;
	}

	fbos[returnVal] = fboHandle;

	EP_GL(glBindTexture(GL_TEXTURE_2D, textureInSlot[lastBoundTextureSlot]));
	EP_GL(glBindFramebuffer(GL_FRAMEBUFFER, fbos[fbStack.back()]));

	fbWidths[returnVal] = width;
	fbHeights[returnVal] = height;
	fbAttachmentSpecs[returnVal] = attachments;
	return returnVal;
}

void Graphics::DeleteFramebuffer(FramebufferHandle fb)
{
	if (colorAttachments.count(fb) == 0 && depthAttachments.count(fb) == 0)
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

	fbos.erase(fb);
	fbWidths.erase(fb);
	fbHeights.erase(fb);
	fbAttachmentSpecs.erase(fb);

	for (const auto& [isTexture, oglHandle] : colorAttachments[fb])
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
	colorAttachments.erase(fb);

	if (depthAttachments[fb].first)
	{
		EP_GL(glDeleteTextures(1, &depthAttachments[fb].second));
	}
	else
	{
		EP_GL(glDeleteTextures(1, &depthAttachments[fb].second));
	}
	depthAttachments.erase(fb);

	returnedFbHandles.push_back(fb);

	EP_GL(glDeleteFramebuffers(1, &fb));
}

void Graphics::PushFramebuffer(FramebufferHandle fb)
{
	EP_ASSERTF_SLOW(fbos.count(fb), "Graphics::PushFramebuffer(): Framebuffer does not exist!");
	EP_GL(glBindFramebuffer(GL_FRAMEBUFFER, fbos[fb]));
	fbStack.emplace_back(fb);
	viewportStack.emplace_back();
}

void Graphics::PushFramebuffer(FramebufferHandle fb, float l, float r, float b, float t)
{
	EP_ASSERTF_SLOW(fbos.count(fb), "Graphics::PushFramebuffer(): Framebuffer does not exist!");
	EP_GL(glBindFramebuffer(GL_FRAMEBUFFER, fbos[fb]));
	fbStack.emplace_back(fb);
	SetViewport(l, r, b, t);
	viewportStack.emplace_back(glm::vec4(l, r, b, t));
}

void Graphics::PopFramebuffer()
{
	if (fbStack.size() > 1) // Index 0 is default framebuffer '0'
	{
		fbStack.pop_back();
		viewportStack.pop_back();
		EP_GL(glBindFramebuffer(GL_FRAMEBUFFER, fbos[fbStack.back()]));
		SetViewport(viewportStack.back().x,
					viewportStack.back().y,
					viewportStack.back().z,
					viewportStack.back().w);
	}
	else
	{
		EP_WARN("Graphics::PopFramebuffer(): There are no framebuffers on the stack!");
	}
}


Graphics::TextureHandle Graphics::GetColorTexture(FramebufferHandle fb, int number)
{
	if (colorAttachments.count(fb))
	{
		if (number < colorAttachments[fb].size())
		{
			if (colorAttachments[fb][number].first)
			{
				return colorAttachments[fb][number].second;
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
	if (depthAttachments.count(fb))
	{
		if (depthAttachments[fb].first)
		{
			return depthAttachments[fb].second;
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
		if (fbWidths[fb] == 0) // Default framebuffer
			return Window::GetWidth();
		else
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
	if (fbWidths.count(fb))
	{
		if (fbWidths[fb] != width || fbHeights[fb] != height)
		{
			std::vector<FramebufferAttachmentSpec> attachments = fbAttachmentSpecs[fb];
			DeleteFramebuffer(fb);
			CreateFramebuffer(attachments, width, height, fb);
		}
	}
	else
	{
		EP_ERROR("Graphics::ResizeFramebuffer(): Framebuffer \"{}\" does not exist!", fb);
	}
}


void Graphics::SetViewport(float l, float r, float b, float t)
{
	int& width = fbWidths[fbStack.back()];
	int& height = fbHeights[fbStack.back()];

	EP_GL(glViewport(l * width, b * height, r * width, t * height));
	viewportStack.back() = { l, r, b, t };
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
