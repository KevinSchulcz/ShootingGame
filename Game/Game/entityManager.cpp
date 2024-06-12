#include "entityManager.h"

entityManager::entityManager()
{

}

void entityManager::update()
{
	// could be after removing to be more efficient?
	for (auto& e : m_entitiesToAdd)
	{
		m_entities.push_back(e);
		m_entityMap[e->m_tag].push_back(e);
	}
	m_entitiesToAdd.clear();
	
	// remove dead entities from full list and map
	removeDeadEntities(m_entities);
	for (auto& [tag, entityVec] : m_entityMap)
	{
		removeDeadEntities(entityVec);
	}
}

void entityManager::removeDeadEntities(entityVector& vector)
{
	// this works. (but if it removes this from m_entities first, does it still exist in map vecs,
	//	probably not, lambda function probably returns false as e->isAlive anyway cause doesnt exists?)
	std::vector<std::shared_ptr<entity>>::iterator pend = std::remove_if(vector.begin(), vector.end()
		, [](std::shared_ptr<entity> e) { return !e->isAlive(); });
	vector.erase(pend, vector.end());
}

std::shared_ptr<entity> entityManager::addEntity(const int tag)
{
	auto e = std::shared_ptr<entity>(new entity(m_totalEntities++, tag));

	m_entitiesToAdd.push_back(e);

	return e;
}

const entityVector& entityManager::getEntities()
{
	return m_entities;
}

const entityVector& entityManager::getEntities(const int tag)
{
	return m_entityMap[tag];
}

