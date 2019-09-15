#include "NetworkBehavior.h"
#include "Application.h"
#include "NetworkSystem.h"

NetworkBehavior::NetworkBehavior(GameObject* a_pObj) :
	BehaviorScript(a_pObj)
{
	m_pClient = Application::GetInstance()->GetClient();
	m_pNetworkIdentity = nullptr;
	m_pNetworkSystem = nullptr;
	SetType(E_Networking);
}

void NetworkBehavior::RegisterCommand(unsigned a_uiId, std::function<void(RakNet::BitStream&)> a_func)
{
	m_mNetworkFuncs[a_uiId] = a_func;
}

void NetworkBehavior::DeSpawn() const
{
	m_pNetworkSystem->DeSpawnObject(m_pNetworkIdentity->GetId());
}

void NetworkBehavior::OnEventReceived(unsigned int a_uiEventId, RakNet::BitStream& a_stream)
{
	const auto iter = m_mNetworkFuncs.find(a_uiEventId);
	if (iter != m_mNetworkFuncs.end())
		iter->second(a_stream);
}
