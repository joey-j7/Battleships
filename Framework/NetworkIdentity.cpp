#include "NetworkIdentity.h"
#include "Application.h"

unsigned int NetworkIdentity::m_uiNetIdCounter = 0;

NetworkIdentity::NetworkIdentity(GameObject* a_pObj, unsigned a_uiId, RakNet::RakNetGUID a_controllerId) :
	Component(a_pObj)
{
	m_pClient = Application::GetInstance()->GetClient();
	m_uiNetId = a_uiId;
	m_controllerGuid = a_controllerId;
	m_bSyncTransform = true;
	m_bLerpTransform = true;
	m_fTransformLerpSpeed = 3.f;
	m_fLerpStopThreshold = 0.01;
	SetType(E_Networking);
}

bool NetworkIdentity::HasAuthority() const
{
	return m_pClient->GetGuid() == m_controllerGuid || m_controllerGuid == RakNet::UNASSIGNED_RAKNET_GUID;
}
