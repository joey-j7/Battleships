#include "NetworkClient.h"
#include "Debug.h"
#include "raknet/include/MessageIdentifiers.h"
#include "raknet/include/RakPeer.h"
#include "raknet/include/RakNetTypes.h"

NetworkClient::NetworkClient(unsigned int a_uiPort, unsigned short a_usMaxAllowedConnections)
{
	m_uiPort = a_uiPort;
	m_uiMaxAllowedConnections = a_usMaxAllowedConnections;
	m_pClient = new RakNet::RakPeer();

	RakNet::SocketDescriptor socket;
	socket.port = m_uiPort;
	const RakNet::StartupResult result = m_pClient->Startup(m_uiMaxAllowedConnections, &socket, 1);
	if (result == RakNet::RAKNET_STARTED)
		Debug::Log("RakNet succesfully started");
	else Debug::Log("Failed to start RakNet");

	m_sLocalIP = m_pClient->GetInternalID(RakNet::UNASSIGNED_SYSTEM_ADDRESS, 0).ToString(false);

	m_bIsHost = false;
	m_bAllowConnection = false;
}

NetworkClient::~NetworkClient()
{
	RakNet::RakPeer::DestroyInstance(m_pClient);
}

void NetworkClient::Disconnect()
{
	for (NetworkUser& user : m_vConnections)
		m_pClient->CloseConnection(user.Guid, true);
	m_vConnections.clear();
}

void NetworkClient::Disconnect(NetworkUser& a_user)
{
	m_pClient->CloseConnection(a_user.Guid, true);
	RemoveConnection(a_user.Guid);
}

void NetworkClient::SendMessage(RakNet::BitStream& a_bitStream, bool a_bBroadcast, RakNet::RakNetGUID a_guid,
	PacketPriority a_priority, PacketReliability a_reliability, char a_cOrderingChannel) const
{
	m_pClient->Send(&a_bitStream, a_priority, a_reliability, a_cOrderingChannel, a_guid, a_bBroadcast);
}

void NetworkClient::SendMessage(unsigned a_uiMessageId, bool a_bBroadcast, RakNet::RakNetGUID a_guid)
{
	RakNet::BitStream bitStream;
	bitStream.Write((unsigned char)a_uiMessageId);
	m_pClient->Send(&bitStream, IMMEDIATE_PRIORITY, RELIABLE, 1, a_guid, a_bBroadcast);
}

void NetworkClient::SendCommand(unsigned int a_uiMessageId, PacketPriority a_priority, PacketReliability a_reliability)
{
	RakNet::BitStream bitStream;
	bitStream.Write((unsigned char)a_uiMessageId);
	SendCommand(bitStream, a_priority, a_reliability);
}

void NetworkClient::SendCommand(RakNet::BitStream& a_bitStream, PacketPriority a_priority, PacketReliability a_reliability)
{
	NetworkUser* pHost = GetHost();
	if (pHost == nullptr)
	{
		m_pClient->Send(&a_bitStream, a_priority, a_reliability, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
		m_pClient->SendLoopback((const char*)a_bitStream.GetData(), a_bitStream.GetNumberOfBytesUsed());
	}
	else
		m_pClient->Send(&a_bitStream, a_priority, a_reliability, 0, pHost->Guid, false);
}

void NetworkClient::SendToSelf(unsigned int a_uiMessageId, const char* a_data) const
{
	RakNet::BitStream bitStream;
	bitStream.Write((unsigned char)a_uiMessageId);
	if (a_data != nullptr)
		bitStream.Write(a_data);
	m_pClient->SendLoopback((const char*)bitStream.GetData(), bitStream.GetNumberOfBytesUsed());
}

void NetworkClient::SendToSelf(RakNet::BitStream& a_data) const
{
	m_pClient->SendLoopback((const char*)a_data.GetData(), a_data.GetNumberOfBytesUsed());
}

void NetworkClient::SetAllowIncomingConnection(bool a_allow)
{
	if (a_allow)
		m_pClient->SetMaximumIncomingConnections(m_uiMaxAllowedConnections);
	else m_pClient->SetMaximumIncomingConnections(0);

	m_bAllowConnection = a_allow;
}

bool NetworkClient::Connect(const std::string& a_sHostAddress)
{
	const RakNet::ConnectionAttemptResult result = m_pClient->Connect(a_sHostAddress.c_str(), m_uiPort, 0, 0);
	switch (result)
	{
	case RakNet::CONNECTION_ATTEMPT_STARTED:
		m_sConnectionMessage = "Attempting connection to IP '" + a_sHostAddress + "'...";
		Debug::Log(m_sConnectionMessage);
		return true;
	case RakNet::INVALID_PARAMETER: 
		m_sConnectionMessage = "Error: Invalid IP address.";
		Debug::Log(m_sConnectionMessage);
		break;
	case RakNet::CANNOT_RESOLVE_DOMAIN_NAME: 
		m_sConnectionMessage = "Error: Cannot resolve domain name.";
		Debug::Log(m_sConnectionMessage);
		break;
	case RakNet::ALREADY_CONNECTED_TO_ENDPOINT: 
		m_sConnectionMessage = "Error: Already connected to IP '" + a_sHostAddress + "'.";
		Debug::Log(m_sConnectionMessage);
		break;
	case RakNet::CONNECTION_ATTEMPT_ALREADY_IN_PROGRESS: 
		m_sConnectionMessage = "Already connecting to IP '" + a_sHostAddress + "', searching...";
		Debug::Log(m_sConnectionMessage);
		break;
	case RakNet::SECURITY_INITIALIZATION_FAILED: 
		m_sConnectionMessage = "Error: Security initialization failed.";
		Debug::Log(m_sConnectionMessage);
		break;
	default: ;
	}
	return false;
}

void NetworkClient::Update()
{
	for (RakNet::Packet* packet = m_pClient->Receive(); packet != nullptr; packet = m_pClient->Receive())
	{
		switch (packet->data[0])
		{
		case ID_CONNECTION_REQUEST_ACCEPTED:
		{
			NetworkUser host(packet->systemAddress.ToString(), packet->guid, packet->systemAddress, true);
			AddConnection(std::move(host));
			OnConnectionAccepted(host);
			break;
		}
		case ID_CONNECTION_ATTEMPT_FAILED:
			OnConnectionFailed(packet->systemAddress);
			break;
		case ID_REMOTE_DISCONNECTION_NOTIFICATION:
		{
			const std::vector<NetworkUser>::iterator iter = FindConnection(packet->guid);
			if (iter != m_vConnections.end())
			{
				OnRemoteDisconnection(*iter);
			}
			RemoveConnection(packet->guid);
			break;
		}
		case ID_NEW_INCOMING_CONNECTION:
		{
			NetworkUser newUser(packet->systemAddress.ToString(), packet->guid, packet->systemAddress, false);
			AddConnection(std::move(newUser));
			OnNewConnection(newUser);
			break;
		}
		case ID_REMOTE_CONNECTION_LOST:
		{
			const std::vector<NetworkUser>::iterator iter = FindConnection(packet->guid);
			if (iter != m_vConnections.end())
			{
				OnRemoteConnectionLost(*iter);
			}
			RemoveConnection(packet->guid);
			break;
		}	
		case ID_DISCONNECTION_NOTIFICATION:
			OnDisconnected();
			Disconnect();
			break;
		case ID_CONNECTION_LOST:
			OnConnectionLost();
			Disconnect();
			break;
		default:
			break;
		}

		OnPacketReceived(packet);
		m_pClient->DeallocatePacket(packet);
	}
}

void NetworkClient::AddConnection(NetworkUser&& a_user)
{
	if (FindConnection(a_user.Guid) == m_vConnections.end())
		m_vConnections.push_back(a_user);
}

void NetworkClient::RemoveConnection(RakNet::RakNetGUID& a_guid)
{
	std::vector<NetworkUser>::iterator iter = FindConnection(a_guid);
	if (iter != m_vConnections.end())
		m_vConnections.erase(iter);
}

const std::vector<NetworkUser>::iterator NetworkClient::FindConnection(RakNet::RakNetGUID& a_guid)
{
	std::vector<NetworkUser>::iterator iter = m_vConnections.begin();
	for (; iter != m_vConnections.end(); ++iter)
	{
		if ((*iter).Guid.g == a_guid.g)
			return iter;
	}
	return m_vConnections.end();
}

NetworkUser* NetworkClient::GetHost()
{
	if (m_vConnections.size() > 0)
	{
		for (NetworkUser& user : m_vConnections)
		{
			if (user.IsHost)
				return &user;
		}
	}
	return nullptr;
}