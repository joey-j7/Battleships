#pragma once
#include "Component.h"
#include "NetworkClient.h"
#include <glm/gtc/quaternion.hpp>

struct NetTransform
{
	glm::vec3 Position;
	glm::quat Rotation;
	glm::vec3 Scale;
};

class NetworkIdentity : public Component
{
public:
	friend class NetworkSystem;

	NetworkIdentity(GameObject* a_pObj, unsigned int a_uiId, 
		RakNet::RakNetGUID a_controllerId = RakNet::UNASSIGNED_RAKNET_GUID);

	Transform* GetTransform() const { return GetObject()->GetTransform(); }
	unsigned int GetId() const { return m_uiNetId; }
	RakNet::RakNetGUID GetOwnerGuid() const { return m_controllerGuid; }

	bool HasAuthority() const;
	bool SyncTransform() const { return m_bSyncTransform; }
	bool LerpTransform() const { return m_bLerpTransform; }
	float GetLerpSpeed() const { return m_fTransformLerpSpeed; }
	
	void SetSyncTransform(bool a_bSync) { m_bSyncTransform = a_bSync; }
	void SetLerpTransform(bool a_bLerp) { m_bLerpTransform = a_bLerp; }
	void SetTransformLerpSpeed(float a_fSpeed) { m_fTransformLerpSpeed = a_fSpeed; }
	void SetLerpStopThreshold(float a_fThreshold) { m_fLerpStopThreshold = a_fThreshold; }

	static unsigned int GenerateId() { return ++m_uiNetIdCounter; }

private:
	unsigned int m_uiNetId;
	RakNet::RakNetGUID m_controllerGuid;

	NetTransform m_netTransform;
	bool m_bSyncTransform;
	bool m_bLerpTransform;
	float m_fTransformLerpSpeed;
	float m_fLerpStopThreshold;

	NetworkClient* m_pClient;

	static unsigned int m_uiNetIdCounter;
};
