#pragma once
#include "IMap.h"
#include "UI/UiTypes.h"

class Map : public IMap
{
public:
	using IMap::IMap;
	void Initialize(size_t i_width, size_t i_height);
	Result Initialize(MapT&& i_map);
	void OnComponentMoved(Position io_position, const Vec2f& i_distanceMoved);
	void Render(RendererT& o_renderStream) const override;
	void OnShow() const override;
	void OnHide() const override;
	utils::unique_ref<IComponent> Clone() override;
	size_t GetWidth() const override;
	size_t GetHeight() const override;
	IComponent* RetrieveComponent(Position& io_position) override;
	MapHolder* RetrieveMapHolder(Position& io_position) override;
	MapHolder ExtractComponent(Position& io_position) override;
	MapHolder AddComponent(MapHolder i_mapHolder, Position& io_position) override;
	Result CheckValidMap() override;

protected:
	bool IsValidPosition(Position& io_position) const;

private:
	struct ExtractedComponent
	{
		Position position;
		MapHolder mapHolder;
	};

	void OnCollision(Position io_position, Position io_destinationPosition);
	void OnComponentBroken(Position i_position, utils::CallableBound<void()> i_callbackAction, class IBreakable&);
	void ReinsertExtractedComponents();

private:
	MapT m_map;
	std::list<ExtractedComponent> m_extractedComponents;
};