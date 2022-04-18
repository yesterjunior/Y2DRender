#pragma once
#include "Math.h"
#include "Pass.h"

#include <vector>

namespace Simple2D
{
	class Renderer;
	class BlockAllocator;
	class Texture;

	class DLL_export Canvas2D
	{
	public:
		Canvas2D(Renderer* renderer);
		~Canvas2D();

		void drawLine(int x1, int y1, int x2, int y2, Color& color, float depth = 0);
		void drawLine(Vec2& pos1, Vec2& pos2, Color& color, float depth = 0);

		void drawTexture(int x, int y, int z, Texture* texture, Color& color);

		void drawCircle(const Vec3& center, int radius, Color& color, int nSlice = -1);
		void fillCircle(const Vec3& center, int radius, int degrees, Color& color, int slice = -1);
		void fillCircle(const Vec3& center, int in_radius, int out_radius, int beginAngle, int endAngle, Color& color, int slice = -1);

		void drawRect(float cx, float cy, float w, float h, Color color, float angle, int depth);
		void drawRect(float x1, float y1, float x2, float y2, Color color, int depth);

		void fillRect(float cx, float cy, float w, float h, Color color, int angle, int depth);
		void fillRect(float x1, float y1, float x2, float y2, Color color, float depth);

		void fillPath(Vec2* posv, int count, Color& color, int depth = 0);

		void fillRoundedRect(int x1, int y1, int x2, int y2, int r, Color color, int depth);

		void fillTriangle(Vec2& p1, Vec2& p2, Vec2& p3, Color& color);

		void setCustomPass(Pass* pass) { pCustomPass = pass; }

	private:
		Renderer* pRenderer;
		BlockAllocator* pBlockAllocator;

		Pass* pLinePass;
		Pass* pTrianglePass;
		Pass* pCustomPass;

		Vec2 circlePos[16];
	};
}