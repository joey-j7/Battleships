#pragma once
#include "ComponentSystem.h"
#include "NetworkClient.h"
#include <map>
#include "NetworkIdentity.h"
#include "NetworkBehavior.h"

#define MAX_SYNC_AMOUNT 25

struct SpawnEvent
{
	unsigned int SpawnId;
	unsigned int IdentityId;
	RakNet::RakNetGUID Guid;

	NetTransform Transform;
};

class NetworkSystem : public ComponentSystem
{
public:
	NetworkSystem(float a_fSyncRate);
	NetworkSystem(float a_fSyncRate, std::map<unsigned int, std::function<GameObject*(SpawnEvent&, RakNet::BitStream&)>> a_mSpawnFuncs);
	~NetworkSystem();

	Event<SpawnEvent&, GameObject*> OnObjectSpawn;

	void SpawnObject(unsigned int a_uiObjectId, RakNet::BitStream* a_pStream = nullptr, RakNet::RakNetGUID a_guid = RakNet::UNASSIGNED_RAKNET_GUID) const;
	void SpawnObject(unsigned int a_uiObjectId, glm::vec3 a_v3Pos, glm::vec3 a_v3Rot, glm::vec3 a_v3Scale, RakNet::BitStream* a_pStream = nullptr, RakNet::RakNetGUID a_guid = RakNet::UNASSIGNED_RAKNET_GUID) const;
	void DeSpawnObject(unsigned int a_uiNetworkIdentityId) const;

protected:
	bool CanProcessComponent(Component* a_pComponent) override;
	void ProcessComponents() override;
	void OnComponentDestroyed(Component* a_pComponent) override;
	void OnComponentAdded(Component* a_pComponent) override;

private:
	void OnPacketReceived(RakNet::Packet* a_pPacket);
	NetworkClient* m_pNetworkClient;

	/* NetworkIdentity methods */
	static const int TRANSFORM_PACKET_SIZE = sizeof(NetTransform) + sizeof(unsigned int);
	std::map<unsigned int, NetworkIdentity*> m_identities;
	float m_fSyncTimer;
	float m_fSyncRate;

	void SyncObjects(RakNet::BitStream& a_readStream);
	void SendTransformData();
	void InterpolateTransforms();

	/* Object spawning - despawning */
	std::map<unsigned int, std::function<GameObject*(SpawnEvent&, RakNet::BitStream&)>> m_SpawnFunctions;

	void HandleOnDespawnObject(RakNet::BitStream& a_readStream);
	void HandleOnSpawnObject(RakNet::BitStream& a_stream);
	
	/* NetworkBehavior methods */
	std::vector<NetworkBehavior*> m_vBehaviors;
	void HandleBehaviorEvent(RakNet::BitStream& a_readStream);
};
