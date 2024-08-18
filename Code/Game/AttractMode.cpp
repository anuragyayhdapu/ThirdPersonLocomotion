#include "Game/App.hpp"
#include "Game/AttractMode.hpp"
#include "Game/GameCommon.hpp"

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Vertex_PCU.hpp"

extern App* g_theApp;

constexpr int MAX_VERTEXAS = 3;
constexpr float MIN_CIRCLE_RADIUS = 2.0f;
constexpr float MAX_CIRCLE_RADIUS = 20.0f;
constexpr float CHANGE_SPEED = 15.0f;

void AttractMode::Startup()
{
	m_screenCamera.SetOrthographicView(Vec2(0.f, 0.f), Vec2(200.f, 100.f));

	m_attractModeClock = new Clock();
}

void AttractMode::Update()
{
	float deltaseconds = m_attractModeClock->GetDeltaSeconds();

	// change circle radius based on time
	
	// Pause
	if (g_theInput->WasKeyJustPressed('P'))
	{
		if (!m_attractModeClock->IsPaused())
		{
			m_attractModeClock->Pause();
		}
		else
		{
			m_attractModeClock->Unpause();
		}
	}

	// Start Slow Motion
	if (g_theInput->IsKeyDown('T'))
	{
		m_attractModeClock->SetTimeScale(0.1f);
	}
	else if (g_theInput->IsKeyDown('Y'))
	{
		m_attractModeClock->SetTimeScale(2.f);
	}
	else
	{
		m_attractModeClock->SetTimeScale(1.f);
	}
	

	// Step Through Single Frame
	if (g_theInput->WasKeyJustPressed('O'))
	{
		m_attractModeClock->StepSingleFrame();
	}

	

	if (isIncreasing)
	{
		m_circleRadius += (deltaseconds * CHANGE_SPEED);
		if (m_circleRadius > MAX_CIRCLE_RADIUS)
		{
			m_circleRadius = MAX_CIRCLE_RADIUS;
			isIncreasing = false;

			return;
		}
	}
	else
	{
		m_circleRadius -= (deltaseconds * CHANGE_SPEED);
		if (m_circleRadius < MIN_CIRCLE_RADIUS)
		{
			m_circleRadius = MIN_CIRCLE_RADIUS;
			isIncreasing = true;

			return;
		}
	}
}

void AttractMode::Render() const
{
	g_theRenderer->ClearScreen(m_backgroundColor);

	g_theRenderer->BeginCamera(m_screenCamera);
	RenderTestTriangle();
	RenderRingAndTexture();

	g_theRenderer->EndCamera(m_screenCamera);
}

void AttractMode::RenderRingAndTexture() const
{
	// box
	std::vector<Vertex_PCU> boxVerts;
	AABB2 bounds(Vec2(10.f, 10.f), Vec2(50.f, 50.f));
	AddVertsForAABB2(boxVerts, bounds, Rgba8(255, 255, 255));

	Texture* texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Test_StbiFlippedAndOpenGL.png");
	g_theRenderer->BindTexture(texture);
	g_theRenderer->DrawVertexArray((int)boxVerts.size(), boxVerts.data());

	// ring
	Vec2 center = (m_screenCamera.GetOrthographicBottomLeft() + m_screenCamera.GetOrthographicTopRight()) / 2.f;
	float thickness = 3.0f;
	Rgba8 color(255, 0, 0);

	std::vector<Vertex_PCU> ringVerts;
	AddVertsForRing2D(ringVerts, center, m_circleRadius, thickness, color);
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray((int)ringVerts.size(), ringVerts.data());
}

float RangeMapX(float value)
{
	return RangeMap(value, -1.f, 1.f, 0.f, WORLD_TOP_RIGHT_ORTHO.x);
}

float RangeMapY(float value)
{
	return RangeMap(value, -1.f, 1.f, 0.f, WORLD_TOP_RIGHT_ORTHO.y);
}

void AttractMode::RenderTestTriangle() const
{
	const int NUM_VERTICES = 3;

	Vertex_PCU vertices[NUM_VERTICES] = {
		Vertex_PCU(Vec3(RangeMapX(-0.5f), RangeMapY(-0.5f),  0.f),  Rgba8::WHITE, Vec2::ZERO),
		Vertex_PCU(Vec3(RangeMapX(0.f),   RangeMapY(0.5f), 0.f),  Rgba8::WHITE, Vec2::ZERO),
		Vertex_PCU(Vec3(RangeMapX(0.5f),  RangeMapY(-0.5f), 0.f) , Rgba8::WHITE, Vec2::ZERO)
	};

	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray(NUM_VERTICES, vertices);
}