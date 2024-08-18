#pragma  once

#include "Game/Entity.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include <vector>

class Texture;

class Prop : public Entity
{
public:
	Prop(Game* game);
	virtual ~Prop();

	// Inherited via Entity
	virtual void Update(float deltaseconds) override;
	virtual void Render() const override;

public:
	std::vector<Vertex_PCU> m_vertexes;
	Texture*				m_texture = nullptr;
};