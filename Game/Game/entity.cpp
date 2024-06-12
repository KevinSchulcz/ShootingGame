#include "entity.h"

entity::entity(const size_t id, const int tag)
	: m_id(id), m_tag(tag) {}

bool entity::isAlive() const
{
	return m_alive;
}

const int entity::tag() const
{
	return m_tag;
}

const size_t entity::id() const
{
	return m_id;
}

void entity::destroy()
{
	m_alive = false;
}