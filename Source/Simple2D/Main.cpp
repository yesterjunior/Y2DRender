#define _CRT_SECURE_NO_WARNINGS
#pragma once
//#include <vld.h>
#include <Windows.h>
#include "RenderWindow.h"
#include "GraphicsContext.h"
#include "Canvas2D.h"
#include "Renderer.h"
#include "TextureManager.h"
#include "TextRender.h"
#include "Particle\ParticleSystem.h"
#include "Event.h"
#include "Sprite.h"
#include <cstdarg>

using namespace Simple2D;

class ParticleEvent : public MouseEventListener
{
public:
	void mouseMove(const MouseEvent& event)
	{
		ps->getEmitter()->setEmitPos(Vec2(event.nX, DEFAULT_WIN_H - event.nY));
	}

	ParticleSystem* ps;
};

void log(const char* format, ...)
{
	va_list ap;
	va_start(ap, format);

	static char formatBuffer[512];
	vsprintf(formatBuffer, format, ap);
	va_end(ap);

	static char outputBuffer[512];
	sprintf(outputBuffer, "%s\n", formatBuffer);
	OutputDebugStringA(outputBuffer);
}

void displayInfo(float deltaTime, int drawCall, TextRender* textRender)
{
	static char  buffer[256] = { "" };
	static float duration = 0;
	static int   frameCount = 0;

	duration += deltaTime;
	if ( duration >= 1000 ) {
		sprintf_s(buffer, "基于 OpenGL 动态CUE - FPS: %d - 平均帧时间：%d 毫秒 - DrawCall: %d", 
			frameCount, ( int ) deltaTime, drawCall);

		duration = 0;
		frameCount = 0;
	}
	frameCount++;

	textRender->drawText(20, 570, 0.3, buffer, Color(1, 1, 1, 1));
	textRender->render();
}

void init(Pass*& p1, Pass*& p2)
{
	p1 = new Pass;
	p2 = new Pass;

	Shader* s1 = new Shader("Shader/defaultGeometryShader.vs", "Shader/defaultGeometryShader.frag", CVA_V3F_C4F);
	Shader* s2 = new Shader("Shader/defaultGeometryShader.vs", "Shader/defaultGeometryShader.frag", CVA_V3F_C4F);

	p1->enableBlend(true);
	p2->enableBlend(true);

	p1->setBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA, BLEND_ONE, BLEND_ZERO);
	p2->setBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA, BLEND_ONE, BLEND_ZERO);

	p1->setShader(s1);
	p2->setShader(s2);

	p1->setPrimType(PT_TRIANGLES);
	p2->setPrimType(PT_TRIANGLES);

	p1->enableStencilTest(true);
	p1->setStencilMask(0xFF);
	p1->setStencilCompareFunc(COMPARE_ALWAYS);
	p1->setStencilRef(1);
	p1->setStencilOp(STENCIL_OP_KEEP, STENCIL_OP_KEEP, STENCIL_OP_REPLACE);

	p2->enableStencilTest(true);
	p2->setStencilMask(0xFF);
	p2->setStencilCompareFunc(COMPARE_EQUAL);
	p2->setStencilRef(1);
	p2->setStencilOp(STENCIL_OP_KEEP, STENCIL_OP_KEEP, STENCIL_OP_REPLACE);
}

/* 模板测试 */
void stencilTest(GraphicsContext* gc, Canvas2D* canvas, Pass* p1, Pass* p2)
{
	static float d = 0;
	static float d1 = 0;
	static float h = 0;

	Vec2 vs1[23];
	Vec2 vs2[23];

	gc->render();
	canvas->setCustomPass(p1);
	canvas->fillCircle(Vec3(400, 300, 0), 100, 360, Color(1, 1, 1, 0));
	gc->render();

	canvas->setCustomPass(p2);

	float hz = 20;
	vs1[0].set(500, 200);
	vs1[1].set(300, 200);
	for ( int i = 0; i <= hz; i++ ) {
		float y = sinf(i / hz * PI_2 + d + i / hz * 2);

		vs1[i + 2].set(300 + i / hz * 200, y * 15 + 200 + h);
	}
	canvas->fillPath(vs1, 23, Color(0, 1, 0, 0.5));

	
	vs2[0].set(500, 200);
	vs2[1].set(300, 200);
	for ( int i = 0; i <= hz; i++ ) {
		float y = sinf(i / hz * PI_2 + d1 + i / hz * 3);

		vs2[i + 2].set(300 + i / hz * 200, y * (10 + i / hz * 10) + 200 + h);
	}
	canvas->fillPath(vs2, 23, Color(0, 1, 0, 0.7));

	gc->render();
	canvas->setCustomPass(nullptr);

	h += 0.08;
	if ( h > 200 ) {
		h = 0;
	}

	d += 0.01;
	if ( d >= PI_2 ) d = 0;
	d1 += 0.02;
	if ( d1 >= PI_2 ) d1 = 0;
}

/* 裁剪测试 */
void scissorTest(ParticleSystem* ps, Canvas2D* canvas)
{
	static float x = 400, y = 300;
	static int xdir = 1, ydir = 1;
	static float clipx1 = 0, clipy1 = 0, clipx2 = 800, clipy2 = 600;
	static int clipdx = 1, clipdy = 1;

	ps->getEmitter()->setEmitPos(Vec2(x, y));
	int speed = 2.5;
	x -= xdir * speed;
	y -= ydir * speed;
	if ( x < 0 ) {
		xdir = -1;
	}
	else if ( x > DEFAULT_WIN_W ) {
		xdir = 1;
	}
	if ( y < 0 ) {
		ydir = -1;
	}
	else if ( y > DEFAULT_WIN_H ) {
		ydir = 1;
	}

	clipx1 += clipdx * 0.5f;
	clipx2 -= clipdx * 0.5f;
	clipy1 += clipdy * 0.5f;
	clipy2 -= clipdy * 0.5f;
	if ( clipx1 >= 150 ) {
		clipdx = -1;
	}
	else if ( clipx1 <= 0 ) {
		clipdx = 1;
	}
	if ( clipy1 >= 150 ) {
		clipdy = -1;
	}
	else if ( clipy1 <= 0 ) {
		clipdy = 1;
	}
	canvas->drawLine(0, clipy1, DEFAULT_WIN_W, clipy1, Color(0, 1, 1, 1));
	canvas->drawLine(clipx1, 0, clipx1, DEFAULT_WIN_H, Color(0, 1, 1, 1));

	canvas->drawLine(0, clipy2, DEFAULT_WIN_W, clipy2, Color(0, 1, 1, 1));
	canvas->drawLine(clipx2, 0, clipx2, DEFAULT_WIN_H, Color(0, 1, 1, 1));
	ps->getPass()->setScissorRect(clipx1, clipy1, clipx2 - clipx1, clipy2 - clipy1);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//RenderWindow window(1000, 600);
	RenderWindow window(DEFAULT_WIN_W, DEFAULT_WIN_H);
	GraphicsContext graphicsContext(&window);

	Canvas2D canvas(graphicsContext.getRenderer());
	TextRender textRender(graphicsContext.getRenderer());

	//Texture* texture = TextureManager::instance()->getTexture("bilibili.jpg");

	ParticleSystem* fire = new ParticleSystem();
	fire->initWithPlist("Particle/fire1.plist");
	fire->setTexture("Particle/fire.png");
	fire->getEmitter()->setEmitPos(Vec2(400, 400));

	ParticleEvent pe;
	pe.ps = fire;

	ParticleSystem* ball = new ParticleSystem();
	ball->initWithPlist("Particle/motion.plist");
	ball->setTexture("Particle/fire.png");
	ball->getEmitter()->setEmitPos(Vec2(400, 300));

	Pass* pass = ball->getPass();
	pass->enableScissor(true);
	pass->setScissorRect(100, 100, 600, 400);

	ParticleSystemManager manager;
	manager.appendParticleSystem(fire);
	manager.appendParticleSystem(ball);

	LARGE_INTEGER lastFrameTime, nowFrameTime;

	/* 获取机器内部计时器的时钟频率 */
	LARGE_INTEGER perf_frequency_int64;
	QueryPerformanceFrequency(&perf_frequency_int64);
	double perf_frequency = ( double ) perf_frequency_int64.QuadPart;

	/* 第一次计数  */
	QueryPerformanceCounter(&lastFrameTime);
	double fps = -1;
	double fixed_frame_time = 1000.0 / fps;	

	//graphicsContext.enableMass(true);
	Pass* p1, *p2;
	init(p1, p2);

	MSG msg = { 0 };
	while ( msg.message != WM_QUIT ) {
		QueryPerformanceCounter(&nowFrameTime);
		float frame_time = ((nowFrameTime.QuadPart - lastFrameTime.QuadPart) * 1000.0f) / perf_frequency;

		/* -1 表示不限 FPS */
		if ( fps != -1 && frame_time < fixed_frame_time ) {
			continue;
		}
		lastFrameTime = nowFrameTime;

		if ( PeekMessage(&msg, 0, 0, 0, PM_REMOVE) ) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			EventDispatcher::getInstance()->flushEvent();
		}
		graphicsContext.beginRender();

		// 模板测试
		textRender.drawText(370, 150, 0.3, "模板测试", Color(1, 1, 1, 1));
		stencilTest(&graphicsContext, &canvas, p1, p2);

		// 裁剪测试
		scissorTest(ball, &canvas);

		manager.update(frame_time / 1000.0f);
		manager.render(graphicsContext.getRenderer());

		for ( int i = 0; i < 10; i++ ) {
			int x = 20 + i * 20;
			int y = 20 + i * 20;
			if ( i < 5 ) {
				canvas.fillRect(x, y, x + 100, y + 100, Color(0.2 * i, 0, 0, 0.1 + 0.1 * i), i);
			}
			else {
				canvas.fillRect(x, y, x + 100, y + 100, Color(0, 0, 0.2 * i, 0.1 * i), i);
			}
		}

		/* 渲染，获取 drawCall */
		graphicsContext.render();
		displayInfo(frame_time, graphicsContext.getRenderer()->getDrawcall(), &textRender);
		
		/* 最后渲染文本 */
		graphicsContext.render();
		graphicsContext.endRender();

	}
	return 0;
}