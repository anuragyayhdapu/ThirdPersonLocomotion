#pragma once

#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Rgba8.hpp"

class Clock;

class AttractMode
{
public:
	void Startup();
	void Update();
	void Render() const;

private:
	float m_circleRadius = 10.0f;
	bool isIncreasing = true;
	Clock* m_attractModeClock = nullptr;

protected:
	void RenderRingAndTexture() const;
	Rgba8 m_backgroundColor = Rgba8(157, 187, 227);
	void RenderTestTriangle() const;

public:
	Camera m_screenCamera;
};