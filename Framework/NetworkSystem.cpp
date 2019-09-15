#include "NetworkSystem.h"
#include "Application.h"
#include "NetworkIdentity.h"
#include "GameTime.h"
#include "Transform.h"
#include "Debug.h"

NetworkSystem::NetworkSystem(float a_fSyncRate)
{
	m_fSyncTimer = 0.f;
	m_fSyncRate = 1.f / a_fSyncRate;
	m_pNetworkClient = Application::GetInstance()->GetClient();
	m_pNetworkClient->OnPacketReceived.Sub(std::bind(&NetworkSystem::OnPacketReceived, this, std::placeholders::_1), this);
}

NetworkSystem::NetworkSystem(float a_fSyncRate, std::map<unsigned, std::function<GameObject*(SpawnEvent&, RakNet::BitStream&)>> a_mSpawnFuncs)
{
	m_fSyncTimer = 0.f;
	m_fSyncRate = 1.f / a_fSyncRate;
	m_SpawnFunctions = a_mSpawnFuncs;
	m_pNetworkClient = Application::GetInstance()->GetClient();
	m_pNetworkClient->OnPacketReceived.Sub(std::bind(&NetworkSystem::OnPacketReceived, this, std::placeholders::_1), this);
}

NetworkSystem::~NetworkSystem()
{
	m_pNetworkClient->OnPacketReceived.UnSub(this);
}

void NetworkSystem::SpawnObject(unsigned a_uiObjectId, RakNet::BitStream* a_pStream, RakNet::RakNetGUID a_guid) const
{
	SpawnObject(a_uiObjectId, glm::vec3(), glm::vec3(), glm::vec3(1, 1, 1), a_pStream, a_guid);
}

void NetworkSystem::SpawnObject(unsigned a_uiObjectId, glm::vec3 a_v3Pos, glm::vec3 a_v3Rot, glm::vec3 a_v3Scale, RakNet::BitStream* a_pStream, RakNet::RakNetGUID a_guid) const
{
	RakNet::BitStream s;
	s.Write((unsigned char)E_SPAWN_OBJECT);

	SpawnEvent e;
	e.SpawnId = a_uiObjectId;
	e.Guid = a_guid;
	e.Transform.Position = a_v3Pos;
	e.Transform.Rotation = a_v3Rot;
	e.Transform.Scale = a_v3Scale;

	s.Write(e);
	if (a_pStream)
		a_pStream->Read(s);

	if (m_pNetworkClient->IsHost())
		m_pNetworkClient->SendToSelf(s);
	else m_pNetworkClient->SendCommand(s, HIGH_PRIORITY, RELIABLE_ORDERED);
}

void NetworkSystem::DeSpawnObject(unsigned int a_uiNetworkIdentityId) const
{
	RakNet::BitStream s;
	s.Write((unsigned char)E_DESPAWN_OBJECT);
	s.Write(a_uiNetworkIdentityId);

	if (m_pNetworkClient->IsHost())
		m_pNetworkClient->SendToSelf(s);
	else m_pNetworkClient->SendCommand(s, HIGH_PRIORITY, RELIABLE_ORDERED);
}

bool NetworkSystem::CanProcessComponent(Component* a_pComponent)
{
	return a_pComponent->GetType() == Component::E_Networking;
}

void NetworkSystem::ProcessComponents()
{
	if (m_pNetworkClient->IsHost())
	{
		m_fSyncTimer += GameTime::GetDeltaTime();
		if (m_fSyncTimer >= m_fSyncRate)
		{
			m_fSyncTimer = 0.f;
			SendTransformData();
		}
	}
	else
	{
		InterpolateTransforms();
	}
}

void NetworkSystem::OnComponentDestroyed(Component* a_pComponent)
{
	if (NetworkIdentity* pIdentity = dynamic_cast<NetworkIdentity*>(a_pComponent))
	{
		const std::map<unsigned int, NetworkIdentity*>::iterator iter = m_identities.find(pIdentity->GetId());
		if (iter != m_identities.end())
			m_identities.erase(iter);
	}
	else if (NetworkBehavior* pBehavior = dynamic_cast<NetworkBehavior*>(a_pComponent))
	{
		const std::vector<NetworkBehavior*>::iterator iter = std::find(m_vBehaviors.begin(), m_vBehaviors.end(), pBehavior);
		if (iter != m_vBehaviors.end())
			m_vBehaviors.erase(iter);
	}
}

void NetworkSystem::OnComponentAdded(Component* a_pComponent)
{
	if (NetworkIdentity* pIdentity = dynamic_cast<NetworkIdentity*>(a_pComponent))
	{
		m_identities[pIdentity->GetId()] = pIdentity;
	}
	else if (NetworkBehavior* pBehavior = dynamic_cast<NetworkBehavior*>(a_pComponent))
	{
		assert(pBehavior->m_pNetworkSystem == nullptr); // Another NetworkSystem has owned this object
		pBehavior->m_pNetworkIdentity = pBehavior->GetComponent<NetworkIdentity>();
		pBehavior->m_pNetworkSystem = this;
		assert(pBehavior->m_pNetworkIdentity != nullptr); // NetworkIdentity component not found on NetworkBehavior

		m_vBehaviors.push_back(pBehavior);
	}
}

void NetworkSystem::SendTransformData()
{
	std::map<unsigned int, NetworkIdentity*>::iterator iter = m_identities.begin();
	unsigned int totalSend = 0;
	while (iter != m_identities.end())
	{
		unsigned short reserve = MAX_SYNC_AMOUNT;
		if (m_identities.size() - totalSend < MAX_SYNC_AMOUNT)
			reserve = m_identities.size() - totalSend;

		RakNet::BitStream s(reserve * TRANSFORM_PACKET_SIZE + 1);
		s.Write((unsigned char)E_NETWORK_TRANSFORM_SYNC);
		unsigned short count = 0;
		for (; iter != m_identities.end(); ++iter)
		{
			if (!iter->second) continue;
			if (!iter->second->SyncTransform()) continue;

			++count;
			Transform* pTransform = iter->second->GetTransform();
			NetTransform netTransform;
			netTransform.Position = pTransform->GetPosition();
			netTransform.Rotation = pTransform->GetRotation();
			netTransform.Scale = pTransform->GetScale();
			s.Write(netTransform);
			s.Write(iter->first);

			if (count >= MAX_SYNC_AMOUNT)
				break;
		}
		m_pNetworkClient->SendMessage(s);
		totalSend += count;
	}
}

void NetworkSystem::InterpolateTransforms()
{
	std::map<unsigned int, NetworkIdentity*>::iterator iter = m_identities.begin();
	for (; iter != m_identities.end(); ++iter)
	{
		if (!iter->second) continue;
		if (!iter->second->SyncTransform() || !iter->second->LerpTransform()) continue;

		Transform* pTransform = iter->second->GetTransform();
		float lerp = GameTime::GetDeltaTime() * iter->second->m_fTransformLerpSpeed;
		if (lerp > 1.f)
			lerp = 1.f;

		const glm::vec3 currentPos = pTransform->GetPosition();
		const glm::vec3 targetPos = iter->second->m_netTransform.Position;
		const glm::quat currentRot = pTransform->GetRotation();
		const glm::quat targetRot = iter->second->m_netTransform.Rotation;

		if (glm::distance(currentPos, targetPos) < iter->second->m_fLerpStopThreshold)
			pTransform->SetPosition(targetPos);
		else pTransform->SetPosition(glm::lerp(currentPos, targetPos, lerp));
			
		if (glm::angle(currentRot, targetRot) < iter->second->m_fLerpStopThreshold)
			pTransform->SetRotation(targetRot);
		else pTransform->SetRotation(glm::slerp(currentRot, targetRot, lerp));
	}
}

void NetworkSystem::OnPacketReceived(RakNet::Packet* a_pPacket)
{
	RakNet::BitStream readStream(a_pPacket->data, a_pPacket->length, false);
	unsigned char messageId;
	readStream.Read(messageId);
	switch (messageId)
	{
	case E_NETWORK_TRANSFORM_SYNC:
	{
		SyncObjects(readStream);
		break;
	}
	case E_SPAWN_OBJECT:
	{
		HandleOnSpawnObject(readStream);
		break;
	}
	case E_BEHAVIOR_EVENT:
		HandleBehaviorEvent(readStream);
		break;
	case E_DESPAWN_OBJECT:
		HandleOnDespawnObject(readStream);
		break;
	default: ;
	}
}

void NetworkSystem::HandleOnDespawnObject(RakNet::BitStream& a_readStream)
{
	unsigned int objectId;
	a_readStream.Read(objectId);
	NetworkIdentity* pIdentity = m_identities[objectId];
	if (pIdentity)
	{
		pIdentity->GetObject()->Destroy();
		if (m_pNetworkClient->IsHost())
		{
			m_pNetworkClient->SendMessage(a_readStream);
		}
	}
}

void NetworkSystem::HandleOnSpawnObject(RakNet::BitStream& a_stream)
{
	SpawnEvent e;
	a_stream.Read(e);
	Debug::Log("Received spawn object command with id: " + to_string(e.SpawnId));

	if (m_pNetworkClient->IsHost())
		e.IdentityId = NetworkIdentity::GenerateId();

	GameObject* pObj = m_SpawnFunctions[e.SpawnId](e, a_stream);

	if (m_pNetworkClient->IsHost())
	{
		a_stream.SetReadOffset(sizeof(SpawnEvent) * 8 + 8);
		RakNet::BitStream s;
		s.Write((unsigned char)E_SPAWN_OBJECT);
		s.Write(e);
		if (a_stream.GetNumberOfUnreadBits() > 0)
			a_stream.Read(s);

		m_pNetworkClient->SendMessage(s);
	}

	pObj->Init.Sub([this, e](GameObject* a_pObj) mutable
	{
		OnObjectSpawn(e, a_pObj);
	}, this);
}

void NetworkSystem::SyncObjects(RakNet::BitStream& a_readStream)
{
	const int numTransforms = std::ceil(a_readStream.GetNumberOfUnreadBits() * 0.125f / TRANSFORM_PACKET_SIZE);

	for (int i = 0; i < numTransforms; ++i)
	{
		NetTransform netTransform;
		unsigned int id;
		a_readStream.Read(netTransform);
		a_readStream.Read(id);

		NetworkIdentity* pIdentity = m_identities[id];
		if (pIdentity)
		{
			pIdentity->m_netTransform = netTransform;
			Transform* pTransform = pIdentity->GetTransform();
			pTransform->SetScale(netTransform.Scale);

			if (!pIdentity->LerpTransform())
			{
				pTransform->SetPosition(netTransform.Position);
				pTransform->SetRotation(netTransform.Rotation);
			}
		}
	}
}

void NetworkSystem::HandleBehaviorEvent(RakNet::BitStream& a_readStream)
{
	if (m_pNetworkClient->IsHost())
	{
		a_readStream.ResetReadPointer();
		m_pNetworkClient->SendMessage(a_readStream);
		a_readStream.SetReadOffset(8);
	}

	RakNet::RakString behaviorName;
	NetworkBehaviorEvent event;
	a_readStream.Read(event);
	a_readStream.Read(behaviorName);

	for (NetworkBehavior* pBehavior : m_vBehaviors)
	{
		if (pBehavior->GetName() == behaviorName &&
			(event.IdentityId == -1 || event.IdentityId == pBehavior->m_pNetworkIdentity->GetId()))
		{
			pBehavior->OnEventReceived(event.EventId, a_readStream);
			a_readStream.SetReadOffset(sizeof(NetworkBehaviorEvent) * 8 + 8);
		}
	}
}