#include "stdafx.h"
#include "Components/IPlayer.h"
#include "Components/MovableComponent.h"
#include "UI/Map.h"
#include "UI/UiTypes.h"

void Map::Initialize(size_t i_width, size_t i_height)
{
	for (size_t i = 0; i < i_height; ++i)
	{
		m_map.emplace_back(i_width);
	}
}

Map::Result Map::Initialize(MapT&& i_map)
{
	if (Result checkResult = CheckValidMap(i_map); checkResult.isErr())
	{
		return checkResult;
	}
	m_map = std::move(i_map);
	return utils::Ok();
}

void Map::OnComponentMoved(Position io_position, const Vec2f& i_distanceMoved)
{
	IComponent* component = RetrieveComponent(io_position);
	if (!io_position.IsValid())
	{
		return;
	}
	Position newPosition = io_position + i_distanceMoved;
	IComponent* destinationComponent = RetrieveComponent(newPosition);
	MovableComponent* movableComponent = dynamic_cast<MovableComponent*>(component);
	if (!newPosition.IsValid())
	{
		if (movableComponent)
		{
			movableComponent->ResetMovement(Vec2f(0));
		}
		return;
	}
	if (destinationComponent == component)
	{
		return;
	}
	if (movableComponent)
	{
		movableComponent->ResetMovement(Vec2f(0));
	}
	OnCollision(io_position, newPosition);
	ReinsertExtractedComponents();
}

void Map::Render(RendererT& o_renderStream) const
{
	for (const auto& row : m_map)
	{
		for (const MapHolder& holder : row)
		{
			if (IUIComponent* uiComponent = dynamic_cast<IUIComponent*>(holder.component.get()))
			{
				uiComponent->Render(o_renderStream);
			}
			else
			{
				o_renderStream << ".";
			}
			o_renderStream << " ";
		}
		o_renderStream << std::endl;
	}
}

utils::unique_ref<IComponent> Map::Clone()
{
	return new Map();
}

Map::Result Map::CheckValidMap(const MapT& i_map) const
{
	for (const auto& row : i_map)
	{
		for (const MapHolder& holder : row)
		{
			if (IPlayer* player = dynamic_cast<IPlayer*>(holder.component.get()))
			{
				return utils::Ok();
			}
		}
	}

	return make_error<MapError>(MapErrorCode::InvalidMap, "Missing player!");
}

IComponent* Map::RetrieveComponent(Position& io_position)
{
	if (!IsValidPosition(io_position))
	{
		return nullptr;
	}
	return m_map[io_position.y.value][io_position.x.value].component.get();
}

IMap::MapHolder Map::ExtractComponent(Position& io_position)
{
	if (!IsValidPosition(io_position))
	{
		return MapHolder(nullptr);
	}
	return std::move(m_map[io_position.y.value][io_position.x.value]);
}

IMap::MapHolder Map::AddComponent(MapHolder i_mapHolder, Position& io_position)
{
	if (!IsValidPosition(io_position))
	{
		return std::move(i_mapHolder);
	}
	MapHolder previousComponent = std::move(m_map[io_position.y.value][io_position.x.value]);
	m_map[io_position.y.value][io_position.x.value] = std::move(i_mapHolder);
	return previousComponent;
}

bool Map::IsValidPosition(Position& io_position) const
{
	if (io_position.y < 0 || io_position.y >= m_map.size())
	{
		io_position.Invalidate();
		return false;
	}
	if (io_position.x < 0 || io_position.x >= m_map[io_position.y.value].size())
	{
		io_position.Invalidate();
		return false;
	}
	return true;
}

void Map::OnCollision(Position io_position, Position io_destinationPosition)
{
	/*if (ICollidable* collidable = dynamic_cast<ICollidable*>(RetrieveComponent(io_position))
	{
	}
	else*/
	{
		MapHolder holder = ExtractComponent(io_position);
		holder.connection.Disconnect();
		if (MovableComponent* movableComponent = dynamic_cast<MovableComponent*>(holder.component.get()))
		{
			holder.connection = movableComponent->sig_onMoved.Connect(&Map::OnComponentMoved, this, io_destinationPosition);
		}
		MapHolder destinationHolder = AddComponent(std::move(holder), io_destinationPosition);
		if (destinationHolder.component)
		{
			destinationHolder.connection.Lock();
			m_extractedComponents.push_back({ io_destinationPosition, std::move(destinationHolder) });
		}
	}
}

void Map::ReinsertExtractedComponents()
{
	for (auto it = m_extractedComponents.begin(); it != m_extractedComponents.end();)
	{
		if (RetrieveComponent(it->position) == nullptr)
		{
			it->mapHolder.connection.Unlock();
			m_map[it->position.y.value][it->position.x.value] = std::move(it->mapHolder);
			it = m_extractedComponents.erase(it);
		}
		else
		{
			++it;
		}
	}
}