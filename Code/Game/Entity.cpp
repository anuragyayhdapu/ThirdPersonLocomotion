
#include "Game/Entity.hpp"


Entity::Entity(Game* game) :
	m_game(game)
{
}

Entity::~Entity()
{
}

Mat44 Entity::GetModelMatrix() const
{
	Mat44 modelMatrix = m_orientation.GetAsMatrix_XFwd_YLeft_ZUp();
	modelMatrix.SetTranslation3D(m_position);

	return modelMatrix;
}