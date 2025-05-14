#include "stdafx.h"
#include "GameplaySystem/Level.h"

Level::Level(utils::unique_ref<IMap> i_map)
	: m_map(std::move(i_map))
{
	
}

void Level::Render(RendererT& o_renderStream) const
{
	m_map->Render(o_renderStream);
}

utils::unique_ref<IComponent> Level::Clone()
{
	return utils::make_unique<Level>(utils::dynamic_unique_cast<IMap>(m_map->Clone()));
}

IComponent* Level::RetrieveComponent(Position& io_position)
{
	return m_map->RetrieveComponent(io_position);
}

IMap::MapHolder Level::ExtractComponent(Position& io_position)
{
	return m_map->ExtractComponent(io_position);
}

IMap::MapHolder Level::AddComponent(IMap::MapHolder i_mapHolder, Position& io_position)
{
	return m_map->AddComponent(std::move(i_mapHolder), io_position);
}