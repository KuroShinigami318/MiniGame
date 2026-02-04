#pragma once
#include "UI/IMap.h"
#include "GameplaySystem/ILevel.h"

namespace utils
{
class TimerDelayer;
}

class Level : public IMap, public ILevel
{
public:
	Level(utils::unique_ref<IMap> i_map);
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
	void IncreaseScore() override;
	void DecreaseScore() override;
	void ResetScore() override;
	long long GetScore() const override;
	void AttachProgressComponent(IProgressComponent& i_progressComponent) override;
	bool Respawn() override;
	void SetObjectiveScore(long long i_objectiveScore);
	void SetAllowedRespawns(int i_allowedRespawns);
	IMap& GetMap() const;
	void SetDebugEnable(bool i_enable) override;
	bool IsDebugEnabled() const override;

private:
	utils::unique_ref<IMap> m_map;
	IProgressComponent* m_progressComponent;
	int m_allowedRespawns = 0;
	long long m_objectiveScore = 0;
	long long m_score = 0;
	bool m_debugEnabled = false;
};