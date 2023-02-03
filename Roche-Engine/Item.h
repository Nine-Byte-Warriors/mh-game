#pragma once
#include "EventSystem.h"

class Item : Listener
{
	int m_itemCost;
	std::string m_itemName;

public:
	// Inherited via Listener
	virtual void HandleEvent(Event* event) override;
	void AddToEvent() noexcept;
};

