#pragma once

#include "Game/GameCommon.hpp"

#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec2.hpp"


class Game;
class AttractMode;

class App
{
public:
	App();
	~App();

	void Startup();
	void Shutdown();
	void Run();
	void RunFrame();

	bool IsQuitting() const { return m_isQuitting; }
	bool HandleQuitRequested();

private:
	void BeginFrame();
	void Update();
	void Render() const;
	void EndFrame();

private:
	bool m_isQuitting = false;
	Game* m_theGame = nullptr;
	AttractMode* m_theAttractMode = nullptr;

	void LoadFonts();
	void LoadTextures();
	static bool EventHandler_CloseWindow(EventArgs& eventArgs);
	void AddGameKeyText();
	void RenderTestMouse() const;
	void UpdateCursorState();

public:
	GameState m_gameState = ATTRACT_MODE;
};

