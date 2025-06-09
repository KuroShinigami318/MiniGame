#include "stdafx.h"
#include "Components/IPlayer.h"
#include "Components/MovableComponent.h"
#include "GameplaySystem/IBreakable.h"
#include "GameplaySystem/ICollidable.h"
#include "UI/Map.h"
#include "UI/UiTypes.h"

Map::Map(const UIContext& i_uiContext)
	: IUIComponent(i_uiContext)
{
}

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
		o_renderStream << std::endl << "\033[1G";
	}
}

utils::unique_ref<IComponent> Map::Clone()
{
	MapT clonedMap;
	for (size_t row = 0; row < m_map.size(); ++row)
	{
		clonedMap.emplace_back(m_map[row].size());
		for (size_t col = 0; col < m_map[row].size(); ++col)
		{
			if (m_map[row][col].component)
			{
				clonedMap[row][col].component = utils::to_std_unique_ptr(m_map[row][col].component->Clone());
			}
		}
	}
	Map* map = new Map(m_uiContext);
	map->Initialize(std::move(clonedMap)).assertSuccess();
	return map;
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

IMap::MapHolder* Map::RetrieveMapHolder(Position& io_position)
{
	if (!IsValidPosition(io_position))
	{
		return nullptr;
	}
	return &m_map[io_position.y.value][io_position.x.value];
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
	IBreakable* breakableA = dynamic_cast<IBreakable*>(RetrieveComponent(io_position));
	IBreakable* breakableB = dynamic_cast<IBreakable*>(RetrieveComponent(io_destinationPosition));
	utils::CallableBound<void()> refreshOnCollision(&Map::OnCollision, this, io_position, io_destinationPosition);
	if (breakableA)
	{
		MapHolder* holder = RetrieveMapHolder(io_position);
		holder->connections.insert_or_assign(utils::get_type_index(breakableA->sig_onBroken), breakableA->sig_onBroken.Connect(&Map::OnComponentBroken, this, io_position, refreshOnCollision));
	}
	if (breakableB)
	{
		MapHolder* holder = RetrieveMapHolder(io_position);
		holder->connections.insert_or_assign(utils::get_type_index(breakableB->sig_onBroken), breakableB->sig_onBroken.Connect(&Map::OnComponentBroken, this, io_destinationPosition, refreshOnCollision));
	}

	ICollidable* collidableA = dynamic_cast<ICollidable*>(RetrieveComponent(io_position));
	ICollidable* collidableB = dynamic_cast<ICollidable*>(RetrieveComponent(io_destinationPosition));
	if (collidableA && collidableB)
	{
		collidableA->OnCollision(*collidableB);
		collidableB->OnCollision(*collidableA);
	}
	else
	{
		MapHolder holder = ExtractComponent(io_position);
		if (MovableComponent* movableComponent = dynamic_cast<MovableComponent*>(holder.component.get()))
		{
			holder.connections.insert_or_assign(utils::get_type_index(movableComponent->sig_onMoved), movableComponent->sig_onMoved.Connect(&Map::OnComponentMoved, this, io_destinationPosition));
		}
		MapHolder destinationHolder = AddComponent(std::move(holder), io_destinationPosition);
		if (destinationHolder.component)
		{
			std::for_each(destinationHolder.connections.begin(), destinationHolder.connections.end(), [](auto& connection)
			{
				connection.second.Lock();
			});
			m_extractedComponents.push_back({ io_destinationPosition, std::move(destinationHolder) });
		}
	}
}

void Map::OnComponentBroken(Position i_position, utils::CallableBound<void()> i_callbackAction, IBreakable& o_breakable)
{
	StartOptionalTask(m_uiContext.thisFrameQueue, utils::CallableBound<void()>(&Map::ExtractComponent, this, i_position));
	if (i_callbackAction)
	{
		StartOptionalTask(m_uiContext.thisFrameQueue, i_callbackAction);
	}
}

void Map::ReinsertExtractedComponents()
{
	for (auto it = m_extractedComponents.begin(); it != m_extractedComponents.end();)
	{
		if (RetrieveComponent(it->position) == nullptr)
		{
			std::for_each(it->mapHolder.connections.begin(), it->mapHolder.connections.end(), [](auto& connection)
			{
				connection.second.Unlock();
			});
			m_map[it->position.y.value][it->position.x.value] = std::move(it->mapHolder);
			it = m_extractedComponents.erase(it);
		}
		else
		{
			++it;
		}
	}
}