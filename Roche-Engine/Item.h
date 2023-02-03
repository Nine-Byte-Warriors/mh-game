#pragma once
#include "EventSystem.h"

enum class ItemType
{
	CarrotItem,
	PotatoItem
};

class Item : Listener
{
	int m_itemCost;
	std::string m_itemName;
	ItemType m_itemType;

public:
	Item(ItemType type, int cost);
	~Item();

	// Inherited via Listener
	virtual void HandleEvent(Event* event) override;
	void AddToEvent() noexcept;
};