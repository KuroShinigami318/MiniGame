#pragma once
#include "UI/IMap.h"
#include "GameplaySystem/ILevel.h"

class Level : public IMap, public ILevel
{
public:
	Level(utils::unique_ref<IMap> i_map);
	void Render(RendererT& o_renderStream) const override;
	utils::unique_ref<IComponent> Clone() override;
	IComponent* RetrieveComponent(Position& io_position) override;
	MapHolder* RetrieveMapHolder(Position& io_position) override;
	MapHolder ExtractComponent(Position& io_position) override;
	MapHolder AddComponent(MapHolder i_mapHolder, Position& io_position) override;
	void IncreaseScore() override;
	void DecreaseScore() override;
	void ResetScore() override;
	size_t GetScore() const override;
	void SetObjectiveScore(size_t i_objectiveScore);

private:
	utils::unique_ref<IMap> m_map;
	size_t m_objectiveScore = 0;
	size_t m_score = 0;
};