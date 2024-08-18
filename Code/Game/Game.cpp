#include "Game/Player.hpp"
#include "Game/Prop.hpp"
#include "Game/Game.hpp"
#include "Game/App.hpp"
#include "Game/Entity.hpp"

#include "Engine/Renderer/DebugRenderSystem.hpp"
#include "Engine/Window/Window.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"


constexpr int MAX_VERTEXAS = 3;
constexpr unsigned char MIN_TEST_COLOR_VALUE = 5;
constexpr unsigned char MAX_TEST_COLOR_VALUE = 255;

extern App* g_theApp;


Game::Game(App* g_app, bool showDebugView) :
	m_app(g_app),
	m_showDebugView(showDebugView)
{
}

void Game::Startup()
{
	// set camera dimensions
	m_screenCamera.SetOrthographicView(SCREEN_BOTTOM_LEFT_ORTHO, SCREEN_TOP_RIGHT_ORTHO);

	// create clock
	m_GameClock = new Clock();

	CreateScene();
	AddBasisAtOrigin();
	InitMovingPoint();
}

void Game::Shutdown()
{
	for (int index = 0; index < m_entities.size(); index++)
	{
		Entity* entity = m_entities[index];
		if (entity != nullptr)
		{
			delete entity;
			entity = nullptr;
		}
	}
}

void Game::CreateScene()
{
	// 1. add a player to the scene
	m_player = new Player(this);
	m_player->Startup();
	m_player->m_position = Vec3(-3.f, 0.f, 1.f);

	// 2. add 1x1x1 cube prop
	m_cubeProp = new Prop(this);
	m_cubeProp->m_position = Vec3(2.f, 2.f, 0.f);
	// rotate cube 1 about x-axis
	m_cubeProp->m_angularVelocity.m_rollDegrees = 30.f;
	// rotate cube 1 about y-axis
	m_cubeProp->m_angularVelocity.m_pitchDegrees = 30.f;
	AddVertsForCubeProp(*m_cubeProp);

	m_cubeProp2 = new Prop(this);
	m_cubeProp2->m_position = Vec3(-2.f, -2.f, 0.f);
	AddVertsForCubeProp(*m_cubeProp2);

	m_sphereProp = new Prop(this);
	m_sphereProp->m_texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/TestUV.png");
	m_sphereProp->m_angularVelocity.m_yawDegrees = 45.f;
	m_sphereProp->m_position = Vec3(10.f, -5.f, 1.0f);
	AddVertsForSphereProp(*m_sphereProp);

	m_entities.push_back(m_player);
	m_entities.push_back(m_cubeProp);
	m_entities.push_back(m_cubeProp2);
	m_entities.push_back(m_sphereProp);
}

void Game::AddBasisAtOrigin()
{
	// basis arrows
	Vec3 origin = Vec3(0.f, 0.f, 0.f);
	Vec3 xForward = Vec3(1.f, 0.f, 0.f);
	Vec3 yLeft = Vec3(0.f, 1.f, 0.f);
	Vec3 zUp = Vec3(0.f, 0.f, 1.f);

	float radius = 0.1f;
	float duration = -1.f;
	DebugRenderMode mode = DebugRenderMode::USE_DEPTH;
	DebugAddWorldArrow(origin, xForward, radius, duration, Rgba8::RED, Rgba8::RED, mode);
	DebugAddWorldArrow(origin, yLeft, radius, duration, Rgba8::GREEN, Rgba8::GREEN, mode);
	DebugAddWorldArrow(origin, zUp, radius, duration, Rgba8::BLUE, Rgba8::BLUE, mode);

	// text
	Mat44 alongXAxisTransform(Vec3(0.f, -1.f, 0.f), Vec3(1.f, 0.f, 0.f), Vec3(0.f, 0.f, 1.f), Vec3(0.2f, 0.f, 0.2f));
	float textHeight = 0.2f;
	Vec2 alignment = Vec2(0.f, 0.f);
	float textDuration = -1.f;
	DebugAddWorldText("x - forward", alongXAxisTransform, textHeight, alignment, textDuration, Rgba8::RED, Rgba8::RED, DebugRenderMode::USE_DEPTH);

	Mat44 alongYAxisTransform(Vec3(-1.f, 0.f, 0.f), Vec3(0.f, -1.f, 0.f), Vec3(0.f, 0.f, 1.f), Vec3(0.f, 0.2f, 0.2f));
	alignment = Vec2(1.f, 0.f);
	DebugAddWorldText("y - left", alongYAxisTransform, textHeight, alignment, textDuration, Rgba8::GREEN, Rgba8::GREEN, DebugRenderMode::USE_DEPTH);

	Mat44 alongZAxisTransform(Vec3(-1.f, 0.f, 0.f), Vec3(0.f, 0.f, 1.f), Vec3(0.f, 1.f, 0.f), Vec3(0.f, -0.2f, 0.2f));
	alignment = Vec2(0.f, 1.f);
	DebugAddWorldText("z - up", alongZAxisTransform, textHeight, alignment, textDuration, Rgba8::BLUE, Rgba8::BLUE, DebugRenderMode::USE_DEPTH);
}

void Game::AddVertsForCubeProp(Prop& prop)
{
	float x = 0.5f;
	float y = 0.5f;
	float z = 0.5f;

	std::vector<Vertex_PCU>& verts = prop.m_vertexes;
	verts.clear();

	// face 3 : +x plane : back
	AddVertsForQuad3D(verts, Vec3(x, -y, -z), Vec3(x, y, -z), Vec3(x, y, z), Vec3(x, -y, z), Rgba8::RED, AABB2::ZERO_TO_ONE);

	// face 1 : -x plane : front
	AddVertsForQuad3D(verts, Vec3(-x, y, -z), Vec3(-x, -y, -z), Vec3(-x, -y, z), Vec3(-x, y, z), Rgba8::CYAN, AABB2::ZERO_TO_ONE);

	// face 4 : +y plane : west
	AddVertsForQuad3D(verts, Vec3(x, y, -z), Vec3(-x, y, -z), Vec3(-x, y, z), Vec3(x, y, z), Rgba8::GREEN, AABB2::ZERO_TO_ONE);

	// face 2 : -y plane : east
	AddVertsForQuad3D(verts, Vec3(-x, -y, -z), Vec3(x, -y, -z), Vec3(x, -y, z), Vec3(-x, -y, z), Rgba8::MAGENTA, AABB2::ZERO_TO_ONE);

	// face 6 : +z plane : top
	AddVertsForQuad3D(verts, Vec3(x, -y, z), Vec3(x, y, z), Vec3(-x, y, z), Vec3(-x, -y, z), Rgba8::BLUE, AABB2::ZERO_TO_ONE);

	// face 5 : -z plane : bottom
	AddVertsForQuad3D(verts, Vec3(x, y, -z), Vec3(x, -y, -z), Vec3(-x, -y, -z), Vec3(-x, y, -z), Rgba8::YELLOW, AABB2::ZERO_TO_ONE);
}

void Game::AddVertsForSphereProp(Prop& prop)
{
	std::vector<Vertex_PCU>& verts = prop.m_vertexes;
	verts.clear();

	Vec3 center = prop.m_position;
	float radius = 1.f;
	AddVertsForSphere3D(verts, Vec3(), radius, Rgba8::WHITE, AABB2::ZERO_TO_ONE, 8);
}



/*Vec3 start(5.f, 7.f, 1.f);
Vec3 end(1.f, 3.f, -2.f);
float radius = 1.f;
int numSlices = 16;
AddVertsForCylinder3D(verts, start, end, radius, Rgba8::WHITE, AABB2::ZERO_TO_ONE, numSlices);*/


void Game::BeginFrame()
{
}

void Game::Update()
{
	UpdateGameState();
	UpdateCubePropColor();
	UpdateAllEnteties();
	AddDebugRenderObjects();
	UpdateParametricT();
}


void Game::UpdateGameState()
{
	XboxController const& controller = g_theInput->GetController(0);

	// Pause
	if (g_theInput->WasKeyJustPressed('P'))
	{
		if (!m_GameClock->IsPaused())
		{
			m_GameClock->Pause();
		}
		else
		{
			m_GameClock->Unpause();
		}
	}

	// Start Slow Motion
	if (g_theInput->IsKeyDown('T'))
	{
		m_GameClock->SetTimeScale(0.1f);
	}
	else
	{
		m_GameClock->SetTimeScale(1.f);
	}

	// Step Through Single Frame
	if (g_theInput->WasKeyJustPressed('O'))
	{
		m_GameClock->StepSingleFrame();
	}

	// Debug Mode (f1)
	if (g_theInput->WasKeyJustPressed(KEYCODE_F1) ||
		controller.WasButtonJustPressed(XBOX_BUTTON_BACK))
	{
		m_showDebugView = !m_showDebugView;
	}
}

void Game::UpdateCubePropColor()
{
	float getTotalSeconds = m_GameClock->GetTotalSeconds();
	
	// change color of cube2
	float sineValue = sinf(getTotalSeconds);
	m_testColorValue = (unsigned char)RangeMap(sineValue, -1.f, 1.f, 0.f, 255.f);
	m_cubeProp2->m_color = Rgba8(m_testColorValue, m_testColorValue, m_testColorValue);
}

void Game::UpdateAllEnteties()
{
	float deltaSeconds = m_GameClock->GetDeltaSeconds();
	
	// update all entities
	for (int index = 0; index < m_entities.size(); index++)
	{
		Entity* entity = m_entities[index];
		if (entity != nullptr)
		{
			entity->Update(deltaSeconds);
		}
	}
}

void Game::AddDebugRenderObjects()
{
	// wireframe sphere
	if (g_theInput->WasKeyJustPressed('1'))
	{
		EulerAngles playerOrientation = m_player->m_orientation;
		Vec3 player_iForward;
		Vec3 player_jLeft;
		Vec3 player_kUp;
		playerOrientation.GetAsVectors_XFwd_YLeft_ZUp(player_iForward, player_jLeft, player_kUp);
		Vec3 playerPosition = m_player->m_position;
		Vec3 twoUnitsInFrontOfPlayer = (playerPosition + (player_iForward * 2.f));
		
		Vec3 center = twoUnitsInFrontOfPlayer;
		float radius = 0.5f;
		float duration = 5.f;
		Rgba8 startColor = Rgba8::GREEN;
		Rgba8 endColor = Rgba8::RED;
		DebugRenderMode mode = DebugRenderMode::USE_DEPTH;
		DebugAddWorldWireSphere(center, radius, duration, startColor, endColor, mode);
	}

	// x-ray line 
	if (g_theInput->WasKeyJustPressed('2'))
	{
		EulerAngles playerOrientation = m_player->m_orientation;
		Vec3 player_iForward;
		Vec3 player_jLeft;
		Vec3 player_kUp;
		playerOrientation.GetAsVectors_XFwd_YLeft_ZUp(player_iForward, player_jLeft, player_kUp);

		Vec3 start = m_player->m_position;
		Vec3 end = start + (player_iForward * 20.f);
		float radius = 0.1f;
		float duration = 10.f;
		Rgba8 startColor = Rgba8::BLUE;
		Rgba8 endColor = Rgba8::BLUE;
		DebugRenderMode mode = DebugRenderMode::X_RAY;
		DebugAddWorldLine(start, end, radius, duration, startColor, endColor, mode);
	}

	// player basis (i, j, k)
	if (g_theInput->WasKeyJustPressed('3'))
	{
		Mat44 playerModelMatrix = m_player->GetModelMatrix();
		Vec3 player_iForward = playerModelMatrix.GetIBasis3D();
		Vec3 player_jLeft = playerModelMatrix.GetJBasis3D();
		Vec3 player_kUp = playerModelMatrix.GetKBasis3D();
		
		float radius = 0.1f;
		float duration = 20.f;
		DebugRenderMode mode = DebugRenderMode::USE_DEPTH;
		// i
		Vec3 iStart = m_player->m_position;
		Vec3 iEnd = iStart + (player_iForward * 1.f);
		DebugAddWorldArrow(iStart, iEnd, radius, duration, Rgba8::RED, Rgba8::RED, mode);
		// j
		Vec3 jStart = m_player->m_position;
		Vec3 jEnd = jStart + (player_jLeft * 1.f);
		DebugAddWorldArrow(jStart, jEnd, radius, duration, Rgba8::GREEN, Rgba8::GREEN, mode);
		// k
		Vec3 kStart = m_player->m_position;
		Vec3 kEnd = kStart + (player_kUp * 1.f);
		DebugAddWorldArrow(kStart, kEnd, radius, duration, Rgba8::BLUE, Rgba8::BLUE, mode); 
	}

	// billboard camera opposing text
	if (g_theInput->WasKeyJustPressed('4'))
	{
		Vec3 playerPosition = m_player->m_position;
		EulerAngles playerOrientation = m_player->m_orientation;
		float textHeight = 0.2f;
		Vec2 alignment = Vec2(0.5f, 0.5f);
		float textDuration = 5.f;
		std::string playerInfoStr = Stringf("Position: %.2f, %.2f, %.2f Orientation: %.2f, %.2f, %.2f ", playerPosition.x, playerPosition.y, playerPosition, playerOrientation.m_yawDegrees, playerOrientation.m_pitchDegrees, playerOrientation.m_rollDegrees);
		DebugAddWorldBillboardText(playerInfoStr, playerPosition, textHeight, alignment, textDuration, Rgba8::WHITE, Rgba8::RED, DebugRenderMode::USE_DEPTH);
	}
	 
	// wireframe cylinder
	if (g_theInput->WasKeyJustPressed('5'))
	{
		Vec3 start = m_player->m_position;
		Vec3 end = Vec3(start.x, start.y, start.z + 1);
		float radius = 0.5f;
		float duration = 10.f;
		Rgba8 startColor = Rgba8::WHITE;
		Rgba8 endColor = Rgba8::RED;
		DebugRenderMode mode = DebugRenderMode::USE_DEPTH;
		DebugAddWorldWireCylinder(start, end, radius, duration, startColor, endColor, mode);
	}

	// point on x-y plane
	if (g_theInput->IsKeyDown('6'))
	{
		Vec3 position = m_player->m_position;
		position.z = 0.f;
		float radius = 0.25f;
		float duration = 60.f;
		Rgba8 startColor = Rgba8(150, 45, 0);
		Rgba8 endColor = Rgba8(150, 45, 0);
		DebugRenderMode mode = DebugRenderMode::USE_DEPTH;
		DebugAddWorldPoint(position, radius, duration, startColor, endColor, mode);
	}

	// camera orientation screen text
	if (g_theInput->WasKeyJustPressed('7'))
	{
		EulerAngles cameraOrientation = m_player->m_worldCamera->GetOrientation().GetAsEulerAngles();
		std::string cameraOrientationStr = Stringf("Camera Orientation: %.2f, %.2f, %.2f", cameraOrientation.m_pitchDegrees, cameraOrientation.m_yawDegrees, cameraOrientation.m_rollDegrees);
		float duration = 5.f;
		Rgba8 startColor = Rgba8::WHITE;
		Rgba8 endColor = Rgba8::RED;
		DebugAddMessage(cameraOrientationStr, duration, startColor, endColor);
	}

	// player position screen text
	Vec3 playerPosition = m_player->m_position;
	
	std::string playerPositionStr = Stringf("Player Position: %.2f, %.2f, %.2f", playerPosition.x, playerPosition.y, playerPosition.z);
	float fontSize = 15.f;
	AABB2 cameraBounds(m_screenCamera.GetOrthographicBottomLeft(), m_screenCamera.GetOrthographicTopRight());
	float textMinX = cameraBounds.m_mins.x;
	float textMinY = cameraBounds.m_maxs.y - fontSize;
	Vec2 topLeftLinePosition(textMinX, textMinY);
	Vec2 topLeftAlignment = Vec2(0.f, 1.f);
	float duration = 0.f;	// one frame
	DebugAddScreenText(playerPositionStr, topLeftLinePosition, fontSize, topLeftAlignment, duration);

	// player orientation screen text
	textMinX = cameraBounds.m_maxs.x;
	textMinY = cameraBounds.m_maxs.y - fontSize;
	Vec2 topRightLinePosition(textMinX, textMinY);
	Vec2 topRightAlignment = Vec2(1.f, 1.f);
	float totalSeconds = m_GameClock->GetTotalSeconds();
	float fps = (float)m_GameClock->GetFrameCount() / totalSeconds;
	float scale = m_GameClock->GetTimeScale();
	std::string timeValuesStr = Stringf("Time: %.2f, FPS: %.1f, Scale: %.2f", totalSeconds, fps, scale);
	DebugAddScreenText(timeValuesStr, topRightLinePosition, fontSize, topRightAlignment, duration);
}


void Game::Render() const
{
	g_theRenderer->ClearScreen(m_backGroundColor);

	// world camera (for entities)
	g_theRenderer->BeginCamera(*m_player->m_worldCamera);
	
	RenderGridLines();

	// Render all entities
	for (int index = 0; index < m_entities.size(); index++)
	{
		Entity* entity = m_entities[index];
		if (entity != nullptr)
		{
			entity->Render();
		}
	}

	DebugRenderWorld(*m_player->m_worldCamera);

	RenderMovingPoint();

	// screen camera (for HUD / UI)
	g_theRenderer->BeginCamera(m_screenCamera);
	// add text / UI code here

	
	DebugRenderScreen(m_screenCamera);

	g_theRenderer->EndCamera(m_screenCamera);
}


void Game::RenderColorChangingTriangle() const
{
	// create verts
	Vertex_PCU* verts = new Vertex_PCU[MAX_VERTEXAS];
	verts[0].m_position = Vec3(1.f, 0.f, 0.f);
	verts[1].m_position = Vec3(-1.f, 1.f, 0.f);
	verts[2].m_position = Vec3(-1.f, -1.f, 0.f);
	for (int vertIndex = 0; vertIndex < MAX_VERTEXAS; vertIndex++)
	{
		verts[vertIndex].m_color = Rgba8(0, m_testColorValue, 0);
	}

	// transform to world space
	float scale = 20.f;
	float orientationDegrees = 0.f;
	Camera& worldCamera = *m_player->m_worldCamera;
	Vec2 position = (worldCamera.GetOrthographicBottomLeft() + worldCamera.GetOrthographicTopRight()) / 2.f;
	TransformVertexArrayXY3D(MAX_VERTEXAS, verts, scale, orientationDegrees, position);

	// render
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray(MAX_VERTEXAS, verts);
	delete[] verts;
}

float Game::RangeMapX(float value) const
{
	return RangeMap(value, -1.f, 1.f, 0.f, WORLD_TOP_RIGHT_ORTHO.x);
}

float Game::RangeMapY(float value) const
{
	return RangeMap(value, -1.f, 1.f, 0.f, WORLD_TOP_RIGHT_ORTHO.y);
}

void Game::RenderTestTriangle() const
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


void Game::RenderGridLines() const
{
	std::vector<Vertex_PCU> verts;

	// axis lines
	/*AABB3 xOriginPipe(Vec3(0.f, 0.f, 0.f), Vec3(1.f, 0.1f, 0.1f));
	AddVertsForAABB3D(verts, xOriginPipe, Rgba8(255, 0, 0, 175));

	AABB3 yOriginPipe(Vec3(0.f, 0.f, 0.f), Vec3(0.1f, 1.f, 0.1f));
	AddVertsForAABB3D(verts, yOriginPipe, Rgba8(0, 255, 0, 175));

	AABB3 zOriginPipe(Vec3(0.f, 0.f, 0.f), Vec3(0.1f, 0.1f, 1.0f));
	AddVertsForAABB3D(verts, zOriginPipe, Rgba8(0, 0, 255, 175));*/

	// thin x axis grid lines
	// lines parallel to x-axis going in the +y direction
	float yPos = 0.f;
	float halfLength = 50.f;
	Rgba8 xPipeColor(200, 0, 0, 175);
	for (int numXLines = 0; numXLines < 50; numXLines++)
	{
		AABB3 pipe(Vec3(-halfLength, yPos, 0.f), Vec3(halfLength, yPos + 0.02f, 0.02f));
		AddVertsForAABB3D(verts, pipe, xPipeColor);
		yPos += 1.f;
	}

	// lines parallel to x-axis going in the -y direction
	yPos = -1.f;
	for (int numXLines = 0; numXLines < 50; numXLines++)
	{
		AABB3 pipe(Vec3(-halfLength, yPos, 0.f), Vec3(halfLength, yPos + 0.02f, 0.02f));
		AddVertsForAABB3D(verts, pipe, xPipeColor);
		yPos -= 1.f;
	}

	// thin y grid lines
	Rgba8 yPipeColor(0, 200, 0, 175);
	for (float xPos = -50.f; xPos <= 50.f; xPos += 1.f)
	{
		AABB3 yPipe(Vec3(xPos, -halfLength, 0.f), Vec3(xPos + 0.02f, halfLength, 0.02f));
		AddVertsForAABB3D(verts, yPipe, yPipeColor);
	}

	// thick y Grids lines
	//Rgba8 yPipeColor(0, 200, 0, 175);
	for (float xPos = -50.f; xPos <= 50.f; xPos += 5.f)
	{
		AABB3 yPipe(Vec3(xPos, -halfLength, 0.f), Vec3(xPos + 0.08f, halfLength, 0.08f));
		AddVertsForAABB3D(verts, yPipe, yPipeColor);
	}

	// thick x Grid lines
	for (yPos = -50.f; yPos <= 50.f; yPos += 5.f)
	{
		AABB3 pipe(Vec3(-halfLength, yPos, 0.f), Vec3(halfLength, yPos + 0.08f, 0.08f));
		AddVertsForAABB3D(verts, pipe, xPipeColor);
	}


	// lines passing through origin
	AABB3 xOriginPipeLong(Vec3(-halfLength, 0.f, 0.f), Vec3(halfLength, 0.1f, 0.1f));
	AddVertsForAABB3D(verts, xOriginPipeLong, xPipeColor);

	AABB3 yOriginPipeLong(Vec3(0.f, -halfLength, 0.f), Vec3(0.11f, halfLength, 0.11f));
	AddVertsForAABB3D(verts, yOriginPipeLong, yPipeColor);

	/*AABB3 zOriginPipeLong(Vec3(0.f, 0.f, 0.f), Vec3(0.42f, 0.42f, halfLength));
	AddVertsForAABB3D(verts, zOriginPipeLong, Rgba8(0, 0, 200, 175));*/

	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray((int)verts.size(), verts.data());
}

void Game::EndFrame()
{

}

bool Game::IsDubugViewOn()
{
	return m_showDebugView;
}


//----------------------------------------------------------------------------------------------------------
void Game::InitMovingPoint()
{
	m_movingPoint = { 0.f, 0.f, 0.f };

	AddVertsForSphere3D(m_pointVerts, m_movingPoint, 0.1f);
}


//----------------------------------------------------------------------------------------------------------
void Game::UpdateParametricT()
{

	if (g_theInput->IsKeyDown(KEYCODE_NUMPAD0))
	{
		m_parametricT += 1.f;
		m_movingPoint = { CosDegrees(m_parametricT), SinDegrees(m_parametricT), m_parametricT/500.f };

		AddVertsForSphere3D(m_pointVerts, m_movingPoint, 0.01f, Rgba8::RED);
	}
	else if (g_theInput->IsKeyDown(KEYCODE_NUMPAD1))
	{
		m_pointVerts.clear();


		m_parametricT += 0.2f;

		m_movingPoint = { 0.f, 0.f, SinDegrees(m_parametricT + 20.f) };
		m_pointVerts.clear();
		AddVertsForSphere3D(m_pointVerts, m_movingPoint, 0.05f, Rgba8::GREEN);

		m_movingPoint = { 0.f, 0.f, SinDegrees(2.f * m_parametricT) };
		AddVertsForSphere3D(m_pointVerts, m_movingPoint, 0.05f, Rgba8::BLUE);

		m_movingPoint = { 0.f, 0.f, 2.f * SinDegrees(m_parametricT) };
		AddVertsForSphere3D(m_pointVerts, m_movingPoint, 0.05f, Rgba8::CYAN);

		m_movingPoint = { 0.f, 0.f, 2.f + SinDegrees(m_parametricT) };
		AddVertsForSphere3D(m_pointVerts, m_movingPoint, 0.05f, Rgba8::DUSTY_ROSE);

		m_movingPoint = { 0.f, 0.f, SinDegrees(m_parametricT) };
		AddVertsForSphere3D(m_pointVerts, m_movingPoint, 0.05f, Rgba8::RED);
		//DebugAddWorldText("");
	}

	/*float totalSeconds = m_GameClock->GetTotalSeconds();
	Vec3 point2 = { 0.f, 0.f, SinDegrees(totalSeconds) };
	AddVertsForSphere3D(m_pointVerts, point2, 0.01f, Rgba8::GREEN);*/
}


//----------------------------------------------------------------------------------------------------------
void Game::RenderMovingPoint() const
{
	Mat44 transform;
	transform.AppendTranslation3D(Vec3(-1.f, 1.f, 1.f));

	g_theRenderer->BindShader(nullptr);
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->SetModelConstants(transform);
	g_theRenderer->DrawVertexArray(m_pointVerts);
}
