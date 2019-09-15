#pragma once
#include "raknet/include/RakPeer.h"
#include <raknet/include/MessageIdentifiers.h>
#include "Event.h"
#include <string>
#include <vector>

enum NetMessage
{
	E_NETWORK_TRANSFORM_SYNC = ID_USER_PACKET_ENUM,
	E_SPAWN_OBJECT,
	E_DESPAWN_OBJECT,
	E_BEHAVIOR_EVENT,
	E_CUSTOM_PACKET_ENUM,
};

struct NetworkUser
{
	NetworkUser(): Name(""), IsHost(false) {}
	NetworkUser(std::string a_sName, RakNet::RakNetGUID a_guid, RakNet::SystemAddress a_address, bool a_bIshost)
	{
		Name = a_sName;
		Guid = a_guid;
		Address = a_address;
		IsHost = a_bIshost;
	}

	std::string Name;
	RakNet::RakNetGUID Guid;
	RakNet::SystemAddress Address;
	bool IsHost;
};

class NetworkClient
{
public:
	NetworkClient(unsigned int a_uiPort, unsigned short a_usMaxAllowedConnections);
	~NetworkClient();

	/// @brief Event is called every time a packet is received, the packet will be destroyed directly after this call
	Event<RakNet::Packet*> OnPacketReceived;

	/* Connection events */
	Event<RakNet::SystemAddress> OnConnectionFailed;
	Event<NetworkUser&> OnConnectionAccepted;
	Event<NetworkUser&> OnNewConnection;
	Event<NetworkUser&> OnRemoteDisconnection;
	Event<NetworkUser&> OnRemoteConnectionLost;
	Event<> OnConnectionLost;
	Event<> OnDisconnected;

	void Update();

	///Disconnects from all currently connected users
	void Disconnect();

	///Disconnects from a specific user
	void Disconnect(NetworkUser& a_user);

	///Tries to connect to another instance of raknet via ip address
	bool Connect(const std::string& a_sHostAddress);

	///Sends a message with the given parameters
	void SendMessage(RakNet::BitStream& a_bitStream, bool a_bBroadcast = true, RakNet::RakNetGUID a_guid = RakNet::UNASSIGNED_RAKNET_GUID, PacketPriority a_priority = HIGH_PRIORITY,  PacketReliability a_reliability = RELIABLE_ORDERED, char a_cOrderingChannel = 0) const;
	void SendMessage(unsigned int a_uiMessageId, bool a_bBroadcast = true, RakNet::RakNetGUID a_guid = RakNet::UNASSIGNED_RAKNET_GUID);
	
	///Sends message to all (including self) users if host, otherwise only to host
	void SendCommand(unsigned int a_uiMessageId, PacketPriority a_priority = HIGH_PRIORITY, PacketReliability a_reliability = RELIABLE);
	void SendCommand(RakNet::BitStream& a_bitStream, PacketPriority a_priority = HIGH_PRIORITY, PacketReliability a_reliability = RELIABLE);

	///Sends a message to itself
	void SendToSelf(unsigned int a_uiMessageId, const char* a_data = nullptr) const;
	void SendToSelf(RakNet::BitStream& a_data) const;

	bool IsHost() const { return m_bIsHost; }
	void SetHost(bool a_bIsHost) { m_bIsHost = a_bIsHost; }

	void SetAllowIncomingConnection(bool a_allow);

	RakNet::RakNetGUID GetGuid() const { return m_pClient->GetMyGUID(); }

	const std::vector<NetworkUser>& GetConnections() const { return m_vConnections; }
	const std::string& GetLocalIP() const { return m_sLocalIP; };

	const std::string& GetConnectionMessage() const { return m_sConnectionMessage; }
	void ClearConnectionMessage() { m_sConnectionMessage = ""; }

private:
	void AddConnection(NetworkUser&& a_user);
	void RemoveConnection(RakNet::RakNetGUID& a_guid);
	const std::vector<NetworkUser>::iterator FindConnection(RakNet::RakNetGUID& a_guid);

	///Gets the host NetworkUser if the total number of connection > 0.
	///If the requester is the host itself it will return nullptr
	NetworkUser* GetHost();

	RakNet::RakPeer* m_pClient;
	std::vector<NetworkUser> m_vConnections;

	std::string m_sLocalIP;
	std::string m_sConnectionMessage;

	bool m_bIsHost;
	bool m_bAllowConnection;
	unsigned int m_uiMaxAllowedConnections;
	unsigned int m_uiPort;
};