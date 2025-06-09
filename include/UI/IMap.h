#pragma once
#include "IUIComponent.h"

class Position;

class IMap : virtual public IUIComponent
{
public:
	struct MapHolder
	{
		MapHolder() = default;
		MapHolder(std::unique_ptr<IComponent> i_component) : component(std::move(i_component)) {}
		std::unique_ptr<IComponent> component;
		std::unordered_map<std::type_index, utils::Connection> connections;
	};

public:
	virtual IComponent* RetrieveComponent(Position& io_position) = 0;
	virtual MapHolder* RetrieveMapHolder(Position& io_position) = 0;
	virtual MapHolder ExtractComponent(Position& io_position) = 0;
	virtual MapHolder AddComponent(MapHolder i_mapHolder, Position& io_position) = 0;
};