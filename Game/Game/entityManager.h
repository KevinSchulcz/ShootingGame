#pragma once

#include "entity.h"
#include <vector>
#include <map>

typedef std::vector<std::shared_ptr<entity>> entityVector;
typedef std::map<const int, entityVector> entityMap;


class entityManager
{
	entityVector m_entities;
	entityVector m_entitiesToAdd;
	entityMap    m_entityMap;
	size_t       m_totalEntities = 0;

	void removeDeadEntities(entityVector& vec);

public:

	entityManager();

	void update();

	std::shared_ptr<entity> addEntity(const int tag);
	const entityVector& getEntities();
	const entityVector& getEntities(const int tag);
};