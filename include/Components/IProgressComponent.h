#pragma once

class IProgressComponent
{
public:
	virtual ~IProgressComponent() = default;
	virtual void UpdateProgress(const float i_progress) = 0;
	virtual void UpdateMaxProgress(const float& i_maxProgress) = 0;
	virtual const float& GetMaxProgress() const = 0;
};