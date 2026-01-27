#pragma once
#include "IUIComponent.h"

class Position;

class IMap : virtual public IUIComponent
{
public:
	DeclareScopedEnum(MapErrorCode, uint8_t,
		InvalidMap)
		using MapError = utils::Error<MapErrorCode>;

	struct MapHolder
	{
		MapHolder() = default;
		MapHolder(std::unique_ptr<IComponent> i_component) : component(std::move(i_component)) {}
		std::unique_ptr<IComponent> component;
		std::unordered_map<std::type_index, utils::Connection> connections;
	};
	using MapT = std::vector<std::vector<MapHolder>>;
	using Result = utils::Result<void, MapError>;

public:
	using IUIComponent::IUIComponent;
	virtual size_t GetWidth() const = 0;
	virtual size_t GetHeight() const = 0;
	virtual IComponent* RetrieveComponent(Position& io_position) = 0;
	virtual MapHolder* RetrieveMapHolder(Position& io_position) = 0;
	virtual MapHolder ExtractComponent(Position& io_position) = 0;
	virtual MapHolder AddComponent(MapHolder i_mapHolder, Position& io_position) = 0;
	virtual Result CheckValidMap() = 0;
};
DefineScopeEnumOperatorImpl(MapErrorCode, IMap)