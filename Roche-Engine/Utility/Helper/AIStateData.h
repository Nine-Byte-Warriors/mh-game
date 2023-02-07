#pragma once
#ifndef AISTATEDATA_H
#define AISTATEDATA_H

#include "AIStateMachine.h"
#include "JsonLoading.h"

using namespace AILogic;

namespace AIStateData
{
	struct PatrolParams
	{
		float fSensingRange;
		int iWaypointCount;
		float fDistanceToWaypoint;
		PatrolType ePatrolType;
	};
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PatrolParams, fSensingRange, iWaypointCount, fDistanceToWaypoint, ePatrolType);

	struct FollowParams
	{
		float fFollowRange;
		float fRepulseRange;
		bool bKeepRange;
	};
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(FollowParams, fFollowRange, fRepulseRange, bKeepRange);

	struct WanderParams
	{
		float fWanderAngle;
		float fWanderDelay;
	};
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(WanderParams, fWanderAngle, fWanderDelay);

	struct FireParams
	{
		float fRate;
		float fRange;
	};
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(FireParams, fRate, fRange);

	struct AIStateJson
	{
		AIStateTypes iStateType;
		int iTypeID;
		float fMin, fMax;
		float fActivate;

		PatrolParams oPatrolParams;
		FollowParams oFollowParams;
		WanderParams oWanderParams;
		FireParams oFireParams;
	};
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AIStateJson, iStateType, iTypeID, fMin, fMax, fActivate, oPatrolParams, oFollowParams, oWanderParams, oFireParams);
}

#endif // !AISTATEDATA_H