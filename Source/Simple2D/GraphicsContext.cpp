#include "GraphicsContext.h"
#include "RenderWindow.h"
#include "Math.h"
#include "Renderer.h"

#include <cassert>

namespace Simple2D
{
	GraphicsContext::GraphicsContext(RenderWindow* renderWindow)
		: pRenderWindow(renderWindow)
		, pRenderer(nullptr)
		, openglRenderContext(0)
		, deviceContext(0)
		, bEnableMsaa(false)
		, bCreateMsaa(false)
		, bRenderToTexture(false)
	{
		this->createOpenGLContext();

		pRenderer = new Renderer();
	}

	GraphicsContext::~GraphicsContext()
	{
		wglDeleteContext(openglRenderContext);
		ReleaseDC(pRenderWindow->getHwnd(), deviceContext);

		if ( bCreateMsaa ) {
			glDeleteRenderbuffers(1, &msaa.colorRenderBuffer);
			glDeleteRenderbuffers(1, &msaa.dpethRenderBuffer);
			glDeleteFramebuffers(1, &msaa.frameBufferObject);
		}

		if ( bCreateTextureFrameBuffer ) {
			glDeleteFramebuffers(1, &frameBufferObject);
			glDeleteTextures(1, &frameBufferTexture);
		}

		delete pRenderer;
	}

	void GraphicsContext::createOpenGLContext()
	{
		if ( openglRenderContext == 0 ) {
			deviceContext = GetDC(pRenderWindow->getHwnd());

			PIXELFORMATDESCRIPTOR pfd = { 0 };
			int color_deep = GetDeviceCaps(deviceContext, BITSPIXEL);

			pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
			pfd.nVersion = 1;
			pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
			pfd.iPixelType = PFD_TYPE_RGBA;
			pfd.cColorBits = color_deep;
			pfd.cDepthBits = 0;
			pfd.cStencilBits = 0;
			pfd.iLayerType = PFD_MAIN_PLANE;

			int pixle_format = ChoosePixelFormat(deviceContext, &pfd);
			SetPixelFormat(deviceContext, pixle_format, &pfd);

			/* ���� OpenGL ��Ⱦ������ */
			openglRenderContext = wglCreateContext(deviceContext);
			if ( openglRenderContext == 0 ) exit(0);

			/* ѡ�� openglRenderContext ��Ϊ��ǰ�̵߳� openglRenderContext */
			if ( wglMakeCurrent(deviceContext, openglRenderContext) == 0 ) exit(1);

			/* GLEW ���������� OpenGL �ĺ���ָ��ģ������ڵ����κ� OpenGL �ĺ���֮ǰ������Ҫ��ʼ��GLEW */
			if ( glewInit() != GLEW_OK ) exit(1);

			/* �����ӿڣ���СΪ�ͻ�����С */
			SIZE size = pRenderWindow->getClientSize();
			glViewport(0, 0, size.cx, size.cy);
		}
	}

	void GraphicsContext::enableMass(bool enabled)
	{
		if ( bCreateMsaa == false ) {
			this->createMSAA();
		}
		bEnableMsaa = enabled;
	}

	void GraphicsContext::createMSAA()
	{
		/* ����֡������ */
		glGenFramebuffers(1, &msaa.frameBufferObject);
		glBindFramebuffer(GL_FRAMEBUFFER, msaa.frameBufferObject);

		/* ��ɫ��Ⱦ������ */
		glGenRenderbuffers(1, &msaa.colorRenderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, msaa.colorRenderBuffer);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGBA8, DEFAULT_WIN_W, DEFAULT_WIN_H);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		/* ����ɫ��Ⱦ������󸽼ӵ�֡�������ɫ������ */
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, msaa.colorRenderBuffer);
		assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

		/* �����Ⱦ������ */
		glGenRenderbuffers(1, &msaa.dpethRenderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, msaa.dpethRenderBuffer);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, DEFAULT_WIN_W, DEFAULT_WIN_H);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		/* �������Ⱦ������󸽼ӵ�֡�������ȸ����� */
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, msaa.dpethRenderBuffer);
		assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

		/* ��� */
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void GraphicsContext::renderToTexture(bool b)
	{
		if ( bCreateTextureFrameBuffer == false ) {
			bCreateTextureFrameBuffer = true;
			this->createTextureFrameBuffer();
		}
		bRenderToTexture = b;
	}

	void GraphicsContext::createTextureFrameBuffer()
	{
		/* ����֡������� */
		glGenFramebuffers(1, &frameBufferObject);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObject);

		/* ����֡�������� */
		glGenTextures(1, &frameBufferTexture);
		glBindTexture(GL_TEXTURE_2D, frameBufferTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, DEFAULT_WIN_W, DEFAULT_WIN_H, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		/* ��������֡���� */
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBufferTexture, 0);
		if ( glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE ) {
			return;
		}

		/* ��Ⱦ������� */
		GLuint renderBufferObject;
		glGenRenderbuffers(1, &renderBufferObject);
		glBindRenderbuffer(GL_RENDERBUFFER, renderBufferObject);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, DEFAULT_WIN_W, DEFAULT_WIN_H);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		/* ����Ⱦ������󸽼ӵ�֡�������Ⱥ�ģ�帽���� */
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBufferObject);
		if ( glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE ) {
			return;
		}

		/* ��� */
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void GraphicsContext::beginRender()
	{
		/* ��������ݣ���֡������� */
		if ( bEnableMsaa ) {
			glBindFramebuffer(GL_FRAMEBUFFER, msaa.frameBufferObject);
		}

		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		pRenderer->reset();
	}

	void GraphicsContext::render()
	{
		pRenderer->render();
	}

	void GraphicsContext::endRender()
	{
		glDisable(GL_SCISSOR_TEST);

		if ( bEnableMsaa ) {
			glBindFramebuffer(GL_READ_FRAMEBUFFER, msaa.frameBufferObject);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			glBlitFramebuffer(0, 0, DEFAULT_WIN_W, DEFAULT_WIN_H, 0, 0, DEFAULT_WIN_W, DEFAULT_WIN_H, GL_COLOR_BUFFER_BIT, GL_NEAREST);
			//glBlitFramebuffer(0, 0, DEFAULT_WIN_W, DEFAULT_WIN_H, 0, 0, DEFAULT_WIN_W, DEFAULT_WIN_H, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
			glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		}
		SwapBuffers(deviceContext);
	}
}