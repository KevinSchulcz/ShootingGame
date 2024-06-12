#pragma once

#include "constants.h"
#include "components.h"
#include <memory>

class entity
{
	friend class entityManager;

	size_t    m_id    = 0;
	bool      m_alive = true;
	const int m_tag   = TAG_DEFAULT;

	// private constructor to give manager sole access
	entity(const size_t id, const int tag);

public:
	
	// entity components
	std::shared_ptr<cTransform> cTransform;
	std::shared_ptr<cShape>     cShape;
	std::shared_ptr<cCollision> cCollision;
	std::shared_ptr<cInput>     cInput;
	std::shared_ptr<cScore>     cScore;
	std::shared_ptr<cLifespan>  cLifespan;

	// getter functions
	bool isAlive() const;
	const int tag() const;
	const size_t id() const;
	void destroy();
};