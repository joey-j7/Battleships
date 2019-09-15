#pragma once
#include "BehaviorScript.h"
#include "NetworkIdentity.h"
#include <map>

struct NetworkBehaviorEvent
{
	int IdentityId;
	unsigned int EventId;
};

class NetworkSystem;
class NetworkBehavior : public BehaviorScript
{
public:
	friend class NetworkSystem;
	NetworkBehavior(GameObject* a_pObj);

	virtual RakNet::RakString GetName() const = 0;

	NetworkIdentity* GetIdentity() const { return m_pNetworkIdentity; }
	NetworkClient* GetClient() const { return m_pClient; }
	NetworkSystem* GetNetworkSystem() const { return m_pNetworkSystem; }

	void RegisterCommand(unsigned int a_uiId, std::function<void(RakNet::BitStream&)> a_func);

	template <typename ...T>
	void RunCommandAll(unsigned int a_uiEventId) const;
	template <typename ...T>
	void RunCommandAll(unsigned int a_uiEventId, T... a_params);
	template <typename ...T>
	void RunCommand(unsigned int a_uiEventId, int a_uiNetworkId, T... a_params);
	template <typename ...T>
	void RunCommand(unsigned int a_uiEventId, int a_uiNetworkId);

	void DeSpawn() const;

private:
	void OnEventReceived(unsigned int a_uiEventId, RakNet::BitStream& a_stream);

	template<typename F, typename ...R> 
	void WriteParams(RakNet::BitStream& a_stream, F first, R... rest);
	template<typename T> 
	void WriteParams(RakNet::BitStream& a_stream, T param);

	NetworkIdentity* m_pNetworkIdentity;
	NetworkClient* m_pClient;
	NetworkSystem* m_pNetworkSystem;
	std::map<unsigned int, std::function<void(RakNet::BitStream&)>> m_mNetworkFuncs;
};

template <typename ... T>
void NetworkBehavior::RunCommandAll(unsigned a_uiEventId) const
{
	RakNet::BitStream stream;
	stream.Write((unsigned char)E_BEHAVIOR_EVENT);

	NetworkBehaviorEvent event;
	event.IdentityId = -1;
	event.EventId = a_uiEventId;

	stream.Write(event);
	stream.Write(GetName());

	if (m_pClient->IsHost())
		m_pClient->SendToSelf(stream);
	else m_pClient->SendCommand(stream);
}

template <typename ... T>
void NetworkBehavior::RunCommandAll(unsigned int a_uiEventId, T... a_params)
{
	//Calls run command with -1 as network identity id which means all identities that equal this type
	RunCommand(a_uiEventId, -1, a_params...);
}

template <typename ... T>
void NetworkBehavior::RunCommand(unsigned a_uiEventId, int a_uiNetworkId, T... a_params)
{
	RakNet::BitStream stream;
	stream.Write((unsigned char)E_BEHAVIOR_EVENT);

	NetworkBehaviorEvent event;
	event.IdentityId = a_uiNetworkId;
	event.EventId = a_uiEventId;

	stream.Write(event);
	stream.Write(GetName());
	WriteParams(stream, a_params...);

	if (m_pClient->IsHost())
		m_pClient->SendToSelf(stream);
	else m_pClient->SendCommand(stream);
}

template <typename ... T>
void NetworkBehavior::RunCommand(unsigned a_uiEventId, int a_uiNetworkId)
{
	RakNet::BitStream stream;
	stream.Write((unsigned char)E_BEHAVIOR_EVENT);

	NetworkBehaviorEvent event;
	event.IdentityId = a_uiNetworkId;
	event.EventId = a_uiEventId;

	stream.Write(event);
	stream.Write(GetName());

	if (m_pClient->IsHost())
		m_pClient->SendToSelf(stream);
	else m_pClient->SendCommand(stream);
}

template <typename F, typename ... R>
void NetworkBehavior::WriteParams(RakNet::BitStream& a_stream, F first, R... rest)
{
	a_stream.Write(first);	
	WriteParams(a_stream, rest...);
}

template <typename T>
void NetworkBehavior::WriteParams(RakNet::BitStream& a_stream, T param)
{
	a_stream.Write(param);	
}
