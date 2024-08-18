#include "GameCommon.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/MathUtils.hpp"


BitmapFont* g_simpleBitmapFont = nullptr;	// Created by App in LoadFonts()


void DebugDrawRing(const Vec2& center, float radius, float thickness, const Rgba8& color)
{
	static const int numSides = 128;
	float deltaThetaDegrees = 360.f / static_cast<float>(numSides);

	float thetaDegrees = 0.f;
	do
	{
		float x1 = radius * CosDegrees(thetaDegrees);
		float y1 = radius * SinDegrees(thetaDegrees);

		thetaDegrees += deltaThetaDegrees;

		float x2 = radius * CosDegrees(thetaDegrees);
		float y2 = radius * SinDegrees(thetaDegrees);

		x1 += center.x;
		y1 += center.y;
		x2 += center.x;
		y2 += center.y;

		DebugDrawLine(Vec2(x1, y1), Vec2(x2, y2), thickness, color);

	} while (thetaDegrees <= 360.f);
}

/*
*
* A------------------B
* s------------------e
* D------------------C
*
*/
void DebugDrawLine(const Vec2& startPoint, const Vec2& endPoint, float thickness, const Rgba8& color)
{
	static const int totalVertexas = 6;
	Vertex_PCU vertexs[totalVertexas] = {};

	float halfThickness = thickness * 0.5f;
	float ninteyDegrees = 90.0f;
	float deltaDegrees = 45.0f;


	float orientationDegrees = (Vec2(endPoint) - Vec2(startPoint)).GetOrientationDegrees();
	float R = GetDistance2D(startPoint, endPoint);

	float xA = R * CosDegrees(orientationDegrees + (ninteyDegrees + deltaDegrees));
	float yA = R * SinDegrees(orientationDegrees + (ninteyDegrees + deltaDegrees));

	float xD = R * CosDegrees(orientationDegrees - (ninteyDegrees + deltaDegrees));
	float yD = R * SinDegrees(orientationDegrees - (ninteyDegrees + deltaDegrees));

	float xB = R * CosDegrees(orientationDegrees + deltaDegrees);
	float yB = R * SinDegrees(orientationDegrees + deltaDegrees);

	float xC = R * CosDegrees(orientationDegrees - deltaDegrees);
	float yC = R * SinDegrees(orientationDegrees - deltaDegrees);

	Vec2 A(xA, yA);
	Vec2 B(xB, yB);
	Vec2 C(xC, yC);
	Vec2 D(xD, yD);

	A.SetLength(halfThickness);
	B.SetLength(halfThickness);
	C.SetLength(halfThickness);
	D.SetLength(halfThickness);

	A += startPoint;
	B += endPoint;
	C += endPoint;
	D += startPoint;

	vertexs[0].m_position = Vec3(A);
	vertexs[1].m_position = Vec3(B);
	vertexs[2].m_position = Vec3(C);

	vertexs[3].m_position = Vec3(A);
	vertexs[4].m_position = Vec3(C);
	vertexs[5].m_position = Vec3(D);

	// color all vertexas
	for (int numVert = 0; numVert < totalVertexas; numVert++)
	{
		// color
		vertexs[numVert].m_color = color;
	}

	//g_theRenderer->DrawVertexArray(totalVertexas, vertexs);
}
