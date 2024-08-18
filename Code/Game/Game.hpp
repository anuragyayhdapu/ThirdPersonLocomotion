#pragma once

#include "Game/GameCommon.hpp"
#include "Engine/Math/Vec2.hpp"


class App;
class Clock;
class Entity;
class Player;
class Prop;

class Game
{
private:
	App* m_app;

	bool m_showDebugView = false;
	bool m_isTestColorIncreasing = false;
	unsigned char m_testColorValue = 255;

public:
	Game(App* g_app, bool showDebugView = false);
	void Startup();
	void Shutdown();
	
	void BeginFrame();
	void Update();
	void Render() const;
	void EndFrame();

	bool IsDubugViewOn();

	Camera m_screenCamera;

	//Rgba8 m_backGroundColor = Rgba8(139, 191, 124);
	Rgba8 m_backGroundColor = Rgba8(38, 38, 38);

protected:
	void RenderColorChangingTriangle() const;
	void RenderTestTriangle() const;

	float RangeMapX(float value) const;
	float RangeMapY(float value) const;

	Clock* m_GameClock = nullptr;

	void CreateScene();
	Player* m_player = nullptr;
	Prop* m_cubeProp = nullptr;
	Prop* m_cubeProp2 = nullptr;
	void AddVertsForCubeProp(Prop& prop);

	Prop* m_sphereProp = nullptr;
	void AddVertsForSphereProp(Prop& prop);

	/*Prop* m_cylinderProp = nullptr;
	void AddVertsForCylinderProp(Prop& prop);*/

	void RenderGridLines() const;

	std::vector<Entity*> m_entities;

	void UpdateGameState();
	void UpdateCubePropColor();
	void UpdateAllEnteties();
	void AddDebugRenderObjects();

	void AddBasisAtOrigin();

	//----------------------------------------------------------------------------------------------------------
	Vec3 m_movingPoint = Vec3::ZERO;
	float m_parametricT = 0.f;
	std::vector<Vertex_PCU> m_pointVerts;
	void InitMovingPoint();
	void UpdateParametricT();
	void RenderMovingPoint() const;
};

