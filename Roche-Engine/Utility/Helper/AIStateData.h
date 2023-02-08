#pragma once
#ifndef AISTATEDATA_H
#define AISTATEDATA_H

#include "AIStateMachine.h"
#include "JsonLoading.h"

using namespace AILogic;

namespace AIStateData
{
	struct FleeParams
	{
		float fMaxRange;
		float fMinRange;
	};
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(FleeParams, fMaxRange, fMinRange);
	
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

		FleeParams oFleeParams;
		PatrolParams oPatrolParams;
		FollowParams oFollowParams;
		WanderParams oWanderParams;
		FireParams oFireParams;
	};
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AIStateJson, iStateType, iTypeID, fMin, fMax, fActivate, oFleeParams, oPatrolParams, oFollowParams, oWanderParams, oFireParams);
}

#endif // !AISTATEDATA_H