#pragma once
#include "IMap.h"
#include "UI/UiTypes.h"

class Map : public IMap
{
public:
	DeclareScopedEnum(MapErrorCode, uint8_t,
		InvalidMap)
	using MapError = utils::Error<MapErrorCode>;
	using Result = utils::Result<void, MapError>;
	using MapT = std::vector<std::vector<MapHolder>>;

public:
	void Initialize(size_t i_width, size_t i_height);
	Result Initialize(MapT&& i_map);
	void OnComponentMoved(Position io_position, const Vec2f& i_distanceMoved);
	void Render(RendererT& o_renderStream) const override;
	utils::unique_ref<IComponent> Clone() override;
	IComponent* RetrieveComponent(Position& io_position) override;
	MapHolder ExtractComponent(Position& io_position) override;
	MapHolder AddComponent(MapHolder i_mapHolder, Position& io_position) override;

protected:
	Result CheckValidMap(const MapT& i_map) const;
	bool IsValidPosition(Position& io_position) const;

private:
	struct ExtractedComponent
	{
		Position position;
		MapHolder mapHolder;
	};

	void OnCollision(Position io_position, Position io_destinationPosition);
	void ReinsertExtractedComponents();

private:
	MapT m_map;
	std::list<ExtractedComponent> m_extractedComponents;
};
DefineScopeEnumOperatorImpl(MapErrorCode, Map)