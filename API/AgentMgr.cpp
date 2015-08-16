#include "AgentMgr.h"

GWAPI::AgentMgr::AgentArray* GWAPI::AgentMgr::GetAgentArray()
{
	AgentArray *agRet = (AgentArray*)MemoryMgr::agArrayPtr;
	if (agRet->size() == 0) throw 1;
	return agRet;
}

GWAPI::AgentMgr::AgentArray* GWAPI::AgentMgr::GetParty() {
	AgentArray* party = new AgentArray();
	AgentArray agents = *GetAgentArray();

	for (size_t i = 0; i < agents.size(); ++i) {
		if (agents[i]->Allegiance == 1
			&& (agents[i]->TypeMap & 0x20000)) {

			// TODO add agents to party maybe? idk
		}
	}

	return party;
}

size_t GWAPI::AgentMgr::GetPartySize() {
	// TODO
	return 8;
}

DWORD GWAPI::AgentMgr::GetDistance(Agent* a, Agent* b) {
	return (DWORD)sqrtl((DWORD)(a->X - b->X) * (DWORD)(a->X - b->X) + (DWORD)(a->Y - b->Y) * (DWORD)(a->Y - b->Y));
}

DWORD GWAPI::AgentMgr::GetSqrDistance(Agent* a, Agent* b) {
	return (DWORD)(a->X - b->X) * (DWORD)(a->X - b->X) + (DWORD)(a->Y - b->Y) * (DWORD)(a->Y - b->Y);
}

GWAPI::AgentMgr::AgentMgr(GWAPIMgr* obj) : parent(obj)
{
	_ChangeTarget = (ChangeTarget_t)MemoryMgr::ChangeTargetFunction;
}

void GWAPI::AgentMgr::ChangeTarget(Agent* Agent)
{
	parent->GameThread->Enqueue(_ChangeTarget, Agent->Id);
}

GWAPI::AgentMgr::Agent* GWAPI::AgentMgr::AgentArray::GetTarget()
{
	return GetIndex(GetTargetId());
}

GWAPI::AgentMgr::Agent* GWAPI::AgentMgr::AgentArray::GetPlayer()
{
	return GetIndex(GetPlayerId());
}

DWORD GWAPI::AgentMgr::AgentArray::GetTargetId()
{
	return *(DWORD*)MemoryMgr::TargetAgentIDPtr;
}

DWORD GWAPI::AgentMgr::AgentArray::GetPlayerId()
{
	return *(DWORD*)MemoryMgr::PlayerAgentIDPtr;
}
