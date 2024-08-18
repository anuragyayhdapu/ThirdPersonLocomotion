//-----------------------------------------------------------------------------------------------
// GameCommon.hpp
//
#pragma once

#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"

constexpr int TOTAL_NUM_KEYS = 255;

constexpr float DEBUG_LINE_THICKNESS = 0.3f;
constexpr float DEBUG_RING_THICKNESS = 0.3f;


const Vec2 SCREEN_BOTTOM_LEFT_ORTHO(0.f, 0.f);
const Vec2 SCREEN_TOP_RIGHT_ORTHO(1600.f, 800.f);

const Vec2 WORLD_BOTTON_LEFT_ORTHO(-1.f, -1.f);
const Vec2 WORLD_TOP_RIGHT_ORTHO(1.f, 1.f);

// temporary game-code functions
void DebugDrawRing(const Vec2& center, float radius, float thickness, const Rgba8& color);
void DebugDrawLine(const Vec2& startPoint, const Vec2& endPoint, float thickness, const Rgba8& color);

extern InputSystem* g_theInput;
extern Renderer* g_theRenderer;

class Window;
extern Window* g_theWindow;

class BitmapFont;
extern BitmapFont* g_simpleBitmapFont;

enum GameState
{
	ATTRACT_MODE,
	PLAY_MODE
};