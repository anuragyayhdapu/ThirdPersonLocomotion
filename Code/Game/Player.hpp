#pragma once

#include "Game/Entity.hpp"

class Camera;

class Player : public Entity
{
public:
	Player(Game* game);
	virtual ~Player();

	void Startup();

	// Inherited via Entity
	virtual void Update(float deltaseconds) override;
	virtual void Render() const override;

	Camera* m_worldCamera = nullptr;

protected:
	void UpdatePlayerMovement(float deltaseconds);
	void ClampOrientation();

	void UpdateVerticalMovement(float deltaseconds);
	void UpdateHorizontalMovement(float deltaseconds);
	void UpdateOrientation();
};