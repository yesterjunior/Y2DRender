#pragma once
#define GLEW_STATIC

#include "Common.h"
#include "Renderer.h"

#include "GL\glew.h"

#include <Windows.h>
#include <vector>

namespace Simple2D
{
	class RenderWindow;
	class Renderer;
	class TextRender;

	struct MSAA
	{
		GLuint frameBufferObject;
		GLuint dpethRenderBuffer;
		GLuint colorRenderBuffer;
	};

	class DLL_export GraphicsContext
	{
	public:
		GraphicsContext(RenderWindow* renderWindow);
		~GraphicsContext();

		void beginRender();
		void render();
		void endRender();

		void flip();

		void enableMass(bool enabled);

		void renderToTexture(bool b);

		Renderer* getRenderer() { return pRenderer; }

	private:
		void createOpenGLContext();

		/* 创建多重采样 */
		void createMSAA();

		/* 创建纹理帧缓冲 */
		void createTextureFrameBuffer();
		
	private:
		RenderWindow* pRenderWindow;
		Renderer* pRenderer;
		TextRender* pTextRender;

		GLuint frameBufferObject;
		GLuint frameBufferTexture;

		bool bRenderToTexture;
		bool bCreateTextureFrameBuffer;

		MSAA msaa;
		bool bEnableMsaa;
		bool bCreateMsaa;

		HGLRC openglRenderContext;
		HDC deviceContext;
	};
}