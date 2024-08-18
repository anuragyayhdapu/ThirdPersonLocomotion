
#include "Game/Prop.hpp"
#include "Game/GameCommon.hpp"

#include "Engine/Renderer/Texture.h"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"


Prop::Prop(Game* game) :
	Entity(game)
{
	m_position = Vec3(0.f, 0.f, 0.f);
}

Prop::~Prop()
{
}

void Prop::Update(float deltaseconds)
{
	// update current orientation according to the angular velocity
	m_orientation.m_yawDegrees += m_angularVelocity.m_yawDegrees * deltaseconds;
	m_orientation.m_pitchDegrees += m_angularVelocity.m_pitchDegrees * deltaseconds;
	m_orientation.m_rollDegrees += m_angularVelocity.m_rollDegrees * deltaseconds;
}

void Prop::Render() const
{
	Mat44 modelMatrix = GetModelMatrix();
	g_theRenderer->SetModelConstants(modelMatrix, m_color);

	//g_theRenderer->SetBlendMode(BlendMode::OPAQUE);
	g_theRenderer->BindTexture(m_texture);
	g_theRenderer->DrawVertexArray((int)m_vertexes.size(), m_vertexes.data());
}
