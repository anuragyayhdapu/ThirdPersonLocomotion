#pragma once

#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Rgba8.hpp"

class Game;

class Entity
{
public:
	Entity(Game* game);
	virtual ~Entity();

	virtual void Update(float deltaseconds) = 0;
	virtual void Render() const = 0;

public:
	Game* m_game = nullptr;

	Vec3 m_position;
	Vec3 m_velocity;

	EulerAngles m_orientation;
	EulerAngles m_angularVelocity;

	Mat44 GetModelMatrix() const;

	Rgba8 m_color = Rgba8::WHITE;
};

