#include "EP_PCH.h"
#include "../Renderer2D.h"

using namespace Enterprise;

constexpr double tabWidthInEms = 2.0;

void Renderer2D::DrawText(std::string text, Graphics::TextureHandle atlas,
	glm::vec3 position, glm::quat rotation, float size,
	TextAlignment alignment)
{
	Graphics::BindTexture(atlas, HN("in_tex"));
	Graphics::SetModelMatrix(glm::mat4(1.0f));

	size_t lineCount = 1;
	std::vector<double> lineWidths;

	if (alignment == TextAlignment::TopLeft ||
		alignment == TextAlignment::MidLeft ||
		alignment == TextAlignment::BottomLeft)
		// line widths aren't needed in left-aligned text
	{
		for (const char& c : text)
		{
			if (c == '\n' || c == '\f')
				lineCount++;
		}
	}
	else
	{
		lineWidths.emplace_back();

		for (const char& c : text)
		{
			switch (c)
			{
			case '\r':
				lineWidths.back() = 0.0;
				break;
			case '\n': case '\f':
				lineWidths.emplace_back();
				lineCount++;
				break;
			case '\t':
			{
				unsigned int cursorPosRoundedDown = unsigned int(lineWidths.back());
				lineWidths.back() = double(cursorPosRoundedDown) + tabWidthInEms - (fmod(cursorPosRoundedDown, tabWidthInEms));
				break;
			}
			default:
				lineWidths.back() += Graphics::GetFontCharAdvance(atlas, c);
				break;
			}
		}
	}

	double xoffset, yoffset;
	double fontLineHeight = 0.0, fontAscenderHeight = 0.0, fontDescenderHeight = 0.0;
	Graphics::GetFontVerticalMetrics(atlas, fontLineHeight, fontAscenderHeight, fontDescenderHeight);

	if (alignment == TextAlignment::TopLeft ||
		alignment == TextAlignment::TopCenter ||
		alignment == TextAlignment::TopRight)
	{
		yoffset = fontDescenderHeight - 1.0;
	}
	else if (alignment == TextAlignment::MidLeft ||
		alignment == TextAlignment::MidCenter ||
		alignment == TextAlignment::MidRight)
	{
		yoffset = -fontAscenderHeight + (fontAscenderHeight + fontDescenderHeight + double(lineCount - 1) * fontLineHeight) / 2.0;
	}
	else // BottomLeft, BottomCenter, or BottomRight
	{
		yoffset = double(lineCount - 1) * fontLineHeight;
	}

	size_t currentQuadIndex = 0;
	unsigned int currentLine = 0;
	char prevChar = 0;
	double cursorPos = 0.0;
	glm::mat4 rotMat(rotation);

	for (const char& c : text)
	{
		if (currentQuadIndex >= spriteComponents.size())
		{
			Graphics::SetVertexData(quadVAH, quadVertices.data(), 0, currentQuadIndex * 4);
			Graphics::DrawTriangles(quadVAH, currentQuadIndex * 6, 0);
			currentQuadIndex = 0;
		}

		switch (c)
		{
		case '\n': case '\f':
			currentLine++;
			cursorPos = 0.0;
			break;
		case '\r':
			cursorPos = 0.0;
			break;
		case '\t':
		{
			unsigned int cursorPosRoundedDown = unsigned int(cursorPos);
			cursorPos = double(cursorPosRoundedDown) + tabWidthInEms - fmod(cursorPosRoundedDown, tabWidthInEms);
			break;
		}
		default:
		{
			if (alignment == TextAlignment::TopLeft ||
				alignment == TextAlignment::MidLeft ||
				alignment == TextAlignment::BottomLeft)
			{
				xoffset = 0.0;
			}
			else if (alignment == TextAlignment::TopCenter ||
				alignment == TextAlignment::MidCenter ||
				alignment == TextAlignment::BottomCenter)
			{
				xoffset = -lineWidths[currentLine] / 2.0;
			}
			else // TopRight, MidRight, or BottomRight
			{
				xoffset = -lineWidths[currentLine];
			}

			float l, r, b, t;
			Graphics::GetFontCharUVBounds(atlas, c, l, r, b, t);
			quadVertices[currentQuadIndex * 4].in_uv = { l, b };
			quadVertices[currentQuadIndex * 4 + 1].in_uv = { r, b };
			quadVertices[currentQuadIndex * 4 + 2].in_uv = { r, t };
			quadVertices[currentQuadIndex * 4 + 3].in_uv = { l, t };

			Graphics::GetFontCharQuadBounds(atlas, c, l, r, b, t, prevChar);
			quadVertices[currentQuadIndex * 4].ep_position	   = position + glm::vec3(rotMat * glm::vec4(size * (l + cursorPos + xoffset), size * (b - currentLine * fontLineHeight + yoffset), 0, 1));
			quadVertices[currentQuadIndex * 4 + 1].ep_position = position + glm::vec3(rotMat * glm::vec4(size * (r + cursorPos + xoffset), size * (b - currentLine * fontLineHeight + yoffset), 0, 1));
			quadVertices[currentQuadIndex * 4 + 2].ep_position = position + glm::vec3(rotMat * glm::vec4(size * (r + cursorPos + xoffset), size * (t - currentLine * fontLineHeight + yoffset), 0, 1));
			quadVertices[currentQuadIndex * 4 + 3].ep_position = position + glm::vec3(rotMat * glm::vec4(size * (l + cursorPos + xoffset), size * (t - currentLine * fontLineHeight + yoffset), 0, 1));

			quadVertices[currentQuadIndex * 4].in_tex = 0;
			quadVertices[currentQuadIndex * 4 + 1].in_tex = 0;
			quadVertices[currentQuadIndex * 4 + 2].in_tex = 0;
			quadVertices[currentQuadIndex * 4 + 3].in_tex = 0;

			prevChar = c;
			currentQuadIndex++;
			cursorPos += Graphics::GetFontCharAdvance(atlas, c);
			break;
		}
		}
	}

	Graphics::SetVertexData(quadVAH, quadVertices.data(), 0, currentQuadIndex * 4);
	Graphics::DrawTriangles(quadVAH, currentQuadIndex * 6, 0);
}
