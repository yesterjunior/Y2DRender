#include "Canvas2D.h"
#include "TextureManager.h"
#include "Renderer.h"

namespace Simple2D
{
	Canvas2D::Canvas2D(Renderer* renderer)
		: pRenderer(renderer)
		, pCustomPass(nullptr)
	{
		pBlockAllocator = BlockAllocator::getInstance();

		float perrad = PI_2 / 16;
		for ( int i = 0; i < 16; i++ ) {
			circlePos[i].set(sin(perrad * i), cos(perrad * i));
		}

		pLinePass = new Pass;

		Shader* shader = new Shader("Shader/defaultGeometryShader.vs", "Shader/defaultGeometryShader.frag", CVA_V3F_C4F);
		pLinePass->setShader(shader);
		pLinePass->setPrimType(PT_LINES);
		pLinePass->setLayout(1);

		pTrianglePass = new Pass;
		pTrianglePass->enableBlend(true);
		pTrianglePass->setBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA, BLEND_ONE, BLEND_ZERO);

		shader = new Shader("Shader/defaultGeometryShader.vs", "Shader/defaultGeometryShader.frag", CVA_V3F_C4F);
		pTrianglePass->setShader(shader);
		pTrianglePass->setPrimType(PT_TRIANGLES);
		pTrianglePass->setLayout(1);
	}

	Canvas2D::~Canvas2D()
	{
		delete pLinePass;
		delete pTrianglePass;
	}

	void Canvas2D::drawLine(Vec2& pos1, Vec2& pos2, Color& color, float depth)
	{
		this->drawLine(pos1.x, pos1.y, pos2.x, pos2.y, color, depth);
	}

	void Canvas2D::drawLine(int x1, int y1, int x2, int y2, Color& color, float depth)
	{
		GeometryVertexFormat* gvf = pBlockAllocator->allocateWithType<GeometryVertexFormat>(2);
		gvf[0].position.set(x1, y1, depth / 100);
		gvf[1].position.set(x2, y2, depth / 100);
		
		gvf[0].color = gvf[1].color = color;

		unsigned int* indices = pBlockAllocator->allocateWithType<unsigned int>(2);
		indices[0] = 0;
		indices[1] = 1;

		VertexIndexData* vid = pBlockAllocator->allocateWithType<VertexIndexData>();
		vid->vertexData = ( char* ) gvf;
		vid->vertexCount = 2;
		vid->indexData = indices;
		vid->indexCount = 2;
		vid->next = nullptr;

		pRenderer->setPass(pCustomPass ? pCustomPass : pLinePass, true);
		pRenderer->addVertexIndexData(vid, sizeof(GeometryVertexFormat));
	}

	void Canvas2D::drawTexture(int x, int y, int z, Texture* texture, Color& color)
	{
		int w = texture->size.w;
		int h = texture->size.h;

		TextureVertexFormat* tvf = pBlockAllocator->allocateWithType<TextureVertexFormat>(4);
		tvf[0].position.set(x + 0, y + 0, 0);
		tvf[1].position.set(x + 0, y + h, 0);
		tvf[2].position.set(x + w, y + h, 0);
		tvf[3].position.set(x + w, y + 0, 0);

		tvf[0].texcoord = texture->texcoords[0];
		tvf[1].texcoord = texture->texcoords[1];
		tvf[2].texcoord = texture->texcoords[2];
		tvf[3].texcoord = texture->texcoords[3];

		tvf[0].color = color;
		tvf[1].color = color;
		tvf[2].color = color;
		tvf[3].color = color;

		unsigned int* indices = pBlockAllocator->allocateWithType<unsigned int>(6);
		indices[0] = 0;
		indices[1] = 2;
		indices[2] = 1;
		indices[3] = 0;
		indices[4] = 3;
		indices[5] = 2;

		VertexIndexData* vid = pBlockAllocator->allocateWithType<VertexIndexData>();
		vid->vertexData = ( char* ) tvf;
		vid->vertexCount = 4;
		vid->indexData = indices;
		vid->indexCount = 6;
		vid->next = nullptr;
		
		pRenderer->setPass(pCustomPass ? pCustomPass : pTrianglePass, true);
		pRenderer->addVertexIndexData(vid, sizeof(TextureVertexFormat));
	}

	void Canvas2D::drawCircle(const Vec3& center, int radius, Color& color, int nSlice)
	{
		if ( nSlice == -1 ) {
			if ( radius <= 12 ) {
				nSlice = 9;
			}
			else if ( radius <= 200 ) {
				nSlice = 24;
			}
			else {
				nSlice = 36;
			}
		}

		GeometryVertexFormat* gvf = pBlockAllocator->allocateWithType<GeometryVertexFormat>(nSlice);
		unsigned int* indices = pBlockAllocator->allocateWithType<unsigned int>(nSlice * 2);

		for ( int i = 0; i < nSlice; i++ ) {
			float rad = i / GLfloat(nSlice) * PI_2;
			gvf[i].position.set(center.x + cos(rad) * radius, center.y + sin(rad) * radius, 0);
			gvf[i].color = color;

			indices[i * 2 + 0] = i;
			indices[i * 2 + 1] = i + 1;
		}
		indices[nSlice * 2 - 1] = 0;

		VertexIndexData* vid = pBlockAllocator->allocateWithType<VertexIndexData>();
		vid->vertexData = ( char* ) gvf;
		vid->vertexCount = nSlice;
		vid->indexData = indices;
		vid->indexCount = nSlice * 2;
		vid->next = nullptr;

		pRenderer->setPass(pCustomPass ? pCustomPass : pLinePass);
		pRenderer->addVertexIndexData(vid, sizeof(GeometryVertexFormat));
	}

	void Canvas2D::fillCircle(const Vec3& center, int radius, int degrees, Color& color, int slice)
	{
		this->fillCircle(center, 0, radius, 0, degrees, color, slice);
	}

	void Canvas2D::fillCircle(const Vec3& center, int in_radius, int out_radius, int beginAngle, int endAngle, Color& color, int slice)
	{
		if ( slice == -1 ) {
			if ( out_radius <= 12 ) {
				slice = 9;
			}
			else if ( out_radius <= 200 ) {
				slice = 24;
			}
			else {
				slice = 36;
			}
		}

		float beginRadian = toRadian(beginAngle);
		float endRadian = toRadian(endAngle);

		slice = (endRadian - beginRadian) / PI_2 * slice;
		float strrad = (endRadian - beginRadian) / slice;

		GeometryVertexFormat* gvf = nullptr;
		unsigned int* indices = nullptr;
		int vertexCount, indexCount;

		if ( in_radius == 0 ) {
			vertexCount = slice + 2;
			indexCount = slice * 3;

			gvf = pBlockAllocator->allocateWithType<GeometryVertexFormat>(vertexCount);
			indices = pBlockAllocator->allocateWithType<unsigned int>(indexCount);

			gvf[0].position = center;
			gvf[0].color = color;

			for ( int i = 0; i <= slice; i++ ) {
				float fTmp = beginRadian + strrad * i;

				gvf[i + 1].position.set(center.x + sin(fTmp) * out_radius, center.y + cos(fTmp) * out_radius, center.z);
				gvf[i + 1].color = color;

				if ( i == slice ) continue;
				indices[i * 3 + 0] = 0;
				indices[i * 3 + 1] = i + 1;
				indices[i * 3 + 2] = i + 2;
			}
		}
		else {
			vertexCount = slice * 2 + 2;
			indexCount = slice * 6;

			gvf = pBlockAllocator->allocateWithType<GeometryVertexFormat>(vertexCount);
			indices = pBlockAllocator->allocateWithType<unsigned int>(indexCount);

			for ( int i = 0; i <= slice; i++ ) {
				float fTmp = beginRadian + strrad * i;

				gvf[i * 2 + 0].position.set(center.x + sin(fTmp) * out_radius, center.y + cos(fTmp) * out_radius, center.z);
				gvf[i * 2 + 1].position.set(center.x + sin(fTmp) * in_radius, center.y + cos(fTmp) * in_radius, center.z);
				gvf[i * 2 + 0].color = color;
				gvf[i * 2 + 1].color = color;

				if ( i == slice ) continue;
				indices[i * 6 + 0] = i * 2;
				indices[i * 6 + 1] = i * 2 + 1;
				indices[i * 6 + 2] = i * 2 + 3;
				indices[i * 6 + 3] = i * 2;
				indices[i * 6 + 4] = i * 2 + 3;
				indices[i * 6 + 5] = i * 2 + 2;
			}
		}
		VertexIndexData* vid = pBlockAllocator->allocateWithType<VertexIndexData>();
		vid->vertexData = ( char* ) gvf;
		vid->vertexCount = vertexCount;
		vid->indexData = indices;
		vid->indexCount = indexCount;
		vid->next = nullptr;

		pRenderer->setPass(pCustomPass ? pCustomPass : pTrianglePass);
		pRenderer->addVertexIndexData(vid, sizeof(GeometryVertexFormat));
	}

	void Canvas2D::drawRect(float x1, float y1, float x2, float y2, Color color, int depth)
	{
		this->drawRect((x1 + x2) / 2, (y1 + y2) / 2, x2 - x1, y2 - y1, color, 0, depth);
	}

	void Canvas2D::drawRect(float cx, float cy, float w, float h, Color color, float angle, int depth)
	{
		GeometryVertexFormat* gvf = pBlockAllocator->allocateWithType<GeometryVertexFormat>(4);

		Matrix4 mat;
		if ( angle == 0 ) {
			mat.makeTrans(cx, cy, 0);
		}
		else {
			mat = Matrix4::makeTransform(Vec3(cx, cy, 0), Vec3::ONE, angle);
		}
		float w2 = w / 2, h2 = h / 2;

		gvf[0].position = mat * Vec3(-w2, -h2, depth / 100.0);
		gvf[1].position = mat * Vec3(-w2,  h2, depth / 100.0);
		gvf[2].position = mat * Vec3( w2,  h2, depth / 100.0);
		gvf[3].position = mat * Vec3( w2, -h2, depth / 100.0);

		gvf[0].color = gvf[1].color = gvf[2].color = gvf[3].color = color;

		unsigned int* indices = pBlockAllocator->allocateWithType<unsigned int>(8);
		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 1;
		indices[3] = 2;
		indices[4] = 2;
		indices[5] = 3;
		indices[6] = 3;
		indices[7] = 0;

		VertexIndexData* vid = pBlockAllocator->allocateWithType<VertexIndexData>();
		vid->vertexData = ( char* ) gvf;
		vid->vertexCount = 4;
		vid->indexData = indices;
		vid->indexCount = 8;
		vid->next = nullptr;

		pRenderer->setPass(pCustomPass ? pCustomPass : pLinePass, true);
		pRenderer->addVertexIndexData(vid, sizeof(GeometryVertexFormat));
	}

	void Canvas2D::fillRect(float x1, float y1, float x2, float y2, Color color, float depth)
	{
		static Vec2 vs[4];

		vs[0].set(x1, y1);
		vs[1].set(x1, y2);
		vs[2].set(x2, y2);
		vs[3].set(x2, y1);

		this->fillPath(vs, 4, color, depth);
	}

	void Canvas2D::fillRect(float x, float y, float w, float h, Color color, int angle, int depth)
	{
		GeometryVertexFormat* gvf = pBlockAllocator->allocateWithType<GeometryVertexFormat>(4);
		Matrix4 mat;
		if ( angle == 0 ) {
			mat.makeTrans(x, y, 0);
		}
		else {
			mat = Matrix4::makeTransform(Vec3(x, y, 0), Vec3::ONE, angle);
		}
		float w2 = w / 2, h2 = h / 2;

		gvf[0].position = mat * Vec3(-w2, -h2, 0);
		gvf[1].position = mat * Vec3(-w2,  h2, 0);
		gvf[2].position = mat * Vec3( w2,  h2, 0);
		gvf[3].position = mat * Vec3( w2, -h2, 0);

		gvf[0].color = gvf[1].color = gvf[2].color = gvf[3].color = color;

		unsigned int* indices = pBlockAllocator->allocateWithType<unsigned int>(6);
		indices[0] = 0;
		indices[1] = 2;
		indices[2] = 1;
		indices[3] = 0;
		indices[4] = 3;
		indices[5] = 2;

		VertexIndexData* vid = pBlockAllocator->allocateWithType<VertexIndexData>();
		vid->vertexData = ( char* ) gvf;
		vid->vertexCount = 4;
		vid->indexData = indices;
		vid->indexCount = 6;
		vid->next = nullptr;

		pRenderer->setPass(pCustomPass ? pCustomPass : pTrianglePass, true);
		pRenderer->addVertexIndexData(vid, sizeof(GeometryVertexFormat), depth);
	}

	void Canvas2D::fillRoundedRect(int x1, int y1, int x2, int y2, int r, Color color, int depth)
	{
		if ( r == 0 ) {
			this->fillRect(x1, y1, x2, y2, color, depth);
			return;
		}

		GeometryVertexFormat* gvf = pBlockAllocator->allocateWithType<GeometryVertexFormat>(20);

		/* ”“…œΩ« */
		int offseth = y2 - r;
		int offsetw = x2 - r;
		for ( int i = 0; i < 5; i++ ) {
			gvf[i].position.set(offsetw + r * circlePos[i].x, offseth + r * circlePos[i].y, 0);
		}

		/* ”“œ¬Ω« */
		offsetw = x2 - r;
		offseth = y1 + r;
		for ( int i = 0; i < 5; i++ ) {
			gvf[5 + i].position.set(offsetw + r * circlePos[4 + i].x, offseth + r * circlePos[4 + i].y, 0);
		}

		/* ◊Ûœ¬Ω« */
		offsetw = x1 + r;
		offseth = y1 + r;
		for ( int i = 0; i < 5; i++ ) {
			gvf[10 + i].position.set(offsetw + r * circlePos[8 + i].x, offseth + r * circlePos[8 + i].y, 0);
		}

		/* ◊Û…œΩ« */
		offsetw = x1 + r;
		offseth = y2 - r;
		for ( int i = 0; i < 4; i++ ) {
			gvf[15 + i].position.set(offsetw + r * circlePos[12 + i].x, offseth + r * circlePos[12 + i].y, 0);
		}
		gvf[19].position.set(offsetw + r * circlePos[0].x, offseth + r * circlePos[0].y, 0);

		for ( int i = 0; i < 20; i++ ) {
			gvf[i].color = color;
		}

		int count = 20 - 2;
		unsigned int* indices = pBlockAllocator->allocateWithType<unsigned int>(count * 3);
		for ( int i = 0; i < count; i++ ) {
			indices[i * 3 + 0] = 0;
			indices[i * 3 + 1] = i + 1;
			indices[i * 3 + 2] = i + 2;
		}

		VertexIndexData* vid = pBlockAllocator->allocateWithType<VertexIndexData>();
		vid->vertexData = ( char* ) gvf;
		vid->vertexCount = 20;
		vid->indexData = indices;
		vid->indexCount = count * 3;
		vid->next = nullptr;

		pRenderer->setPass(pCustomPass ? pCustomPass : pTrianglePass, true);
		pRenderer->addVertexIndexData(vid, sizeof(GeometryVertexFormat), depth);
	}

	void Canvas2D::fillPath(Vec2* posv, int count, Color& color, int depth)
	{
		int indexCount = (count - 2) * 3;

		GeometryVertexFormat* gvf = pBlockAllocator->allocateWithType<GeometryVertexFormat>(count);
		unsigned int* indices = pBlockAllocator->allocateWithType<unsigned int>(indexCount);

		for ( int i = 0; i < count; i++ ) {
			gvf[i].position = posv[i];
			gvf[i].color = color;
		}
		
		for ( int i = 0; i < count - 2; i++ ) {
			indices[i * 3 + 0] = 0;
			indices[i * 3 + 1] = i + 1;
			indices[i * 3 + 2] = i + 2;
		}

		VertexIndexData* vid = pBlockAllocator->allocateWithType<VertexIndexData>();
		vid->vertexData = ( char* ) gvf;
		vid->vertexCount = count;
		vid->indexData = indices;
		vid->indexCount = indexCount;
		vid->next = nullptr;

		pRenderer->setPass(pCustomPass ? pCustomPass : pTrianglePass, true);
		pRenderer->addVertexIndexData(vid, sizeof(GeometryVertexFormat), depth);
	}

	void Canvas2D::fillTriangle(Vec2& p1, Vec2& p2, Vec2& p3, Color& color)
	{
		GeometryVertexFormat* gvf = pBlockAllocator->allocateWithType<GeometryVertexFormat>(3);

		gvf[0].position.set(p1.x, p1.y, 0);
		gvf[1].position.set(p2.x, p2.y, 0);
		gvf[2].position.set(p3.x, p3.y, 0);

		gvf[0].color = gvf[1].color = gvf[2].color = color;

		unsigned int* indices = pBlockAllocator->allocateWithType<unsigned int>(3);
		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;

		VertexIndexData* vid = pBlockAllocator->allocateWithType<VertexIndexData>();
		vid->vertexData = ( char* ) gvf;
		vid->vertexCount = 3;
		vid->indexData = indices;
		vid->indexCount = 3;
		vid->next = nullptr;

		pRenderer->setPass(pCustomPass ? pCustomPass : pTrianglePass);
		pRenderer->addVertexIndexData(vid, sizeof(GeometryVertexFormat));
	}
}