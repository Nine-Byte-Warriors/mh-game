#pragma once
#ifndef AISTATETYPE_H
#define AISTATETYPE_H

#include "AIStateData.h"

namespace AILogic
{
	enum class AIStateTypes
	{
		Idle = 0,
		Seek = 1,
		Flee = 2,
		Patrol = 3,
		Follow = 4,
		Wander = 5,
		Fire = 6,
	};

	static const std::map <AIStateTypes, std::string> m_mapStateType =
	{
		{ AIStateTypes::Idle, "Idle" },
		{ AIStateTypes::Seek, "Seek" },
		{ AIStateTypes::Flee, "Flee" },
		{ AIStateTypes::Patrol, "Patrol" },
		{ AIStateTypes::Follow, "Follow" },
		{ AIStateTypes::Wander, "Wander" },
		{ AIStateTypes::Fire, "Fire" }
	};

	class StateType
	{
	public:
		static const std::string GetString(const AIStateTypes eStateType) {
			auto itStateType = m_mapStateType.find(eStateType);
			return m_mapStateType.contains(eStateType) ? itStateType->second : "";
		}
		static const AIStateTypes GetType(const std::string sStateType) {
			for (auto stateType : m_mapStateType)
				if (stateType.second == sStateType)
					return stateType.first;
			return AIStateTypes::Idle;
		}
	};
}

#endif // !AISTATETYPE_H
