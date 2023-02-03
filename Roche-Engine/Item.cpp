#include "stdafx.h"
#include "Item.h"

Item::Item(ItemType type, int cost)
{
	m_itemType = type;
	m_itemCost = cost;
}

Item::~Item()
{
}

void Item::HandleEvent(Event* event)
{
}

void Item::AddToEvent() noexcept
{
}
