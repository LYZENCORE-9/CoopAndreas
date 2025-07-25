#include "../core/CNetwork.h"
#include "../core/CPacketListener.h"
#include "../core/CPacket.h"

#include "../core/CPlayerManager.h"
#include "../core/CPlayerPackets.h"
#include "../core/CVehicleManager.h"
#include "../core/CVehiclePackets.h"
#include "../core/CPedManager.h"
#include "../core/CPedPackets.h"

#include "../../shared/semver.h"
#include <cstring>

//#define COOP_SERVER_HOSTING_MODE

std::unordered_map<unsigned short, CPacketListener*> CNetwork::m_packetListeners;

bool CNetwork::Init(char hostname[], unsigned short &port, int max_slots)
{
    // init packet listeners
    CNetwork::InitListeners();

    if (enet_initialize() != 0) // try to init enet
    {
        printf("[ERROR] : ENET_INIT FAILED TO INITIALIZE\n");
        return false;
    }

    ENetAddress address;

    hostname[strcspn(hostname, "\n")] = 0; // fix to remove '\n' coming from iem-dini library , i will fix the bug later in the library
    enet_address_set_host(&address, hostname);
    address.port = port; // bind server port


    ENetHost* server = enet_host_create(&address, max_slots, 2, 0, 0); // create enet host

    if (server == NULL)
    {
        printf("[ERROR] : ENET_UDP_SERVER_SOCKET FAILED TO CREATE\n");
        return false;
    }

    printf("[!] : Server Will Start  on (IP : %s) and (Port : %d) (%s:%d)\n", hostname, port, hostname, port);

#if not defined(COOP_SERVER_HOSTING_MODE)
    ENetEvent *event = new ENetEvent;
    memset(event, 0, sizeof(ENetEvent));
    CNetwork::shared_loop_value = true;
    std::thread CNetwork_Thread(CNetwork::HandleServerPacketsThread, server, event, nullptr, nullptr, nullptr);
    CNetwork_Thread.detach();
#else
    ENetEvent event_v;

    while (true) // waiting for event
    {
      enet_host_service(server, &event_v, 1);
        switch (event_v.type)
        {
            case ENET_EVENT_TYPE_CONNECT:
            {
              //CNetwork::HandlePlayerConnected(event);
              HandlePlayerConnected(event_v); // // i'm not sure here , *p_event to return value of pointer and make it as reference
              break;
            }
            case ENET_EVENT_TYPE_RECEIVE:
            {
              //CNetwork::HandlePacketReceive(event);
              HandlePacketReceive(event_v); // // i'm not sure here , *p_event to return value of pointer and make it as reference
              enet_packet_destroy(event_v.packet);
              break;
            }
            case ENET_EVENT_TYPE_DISCONNECT:
            {
              //CNetwork::HandlePlayerDisconnected(event);
              HandlePlayerDisconnected(event_v); // // i'm not sure here , *p_event to return value of pointer and make it as reference
              break;
            }
        }
    }

   enet_host_destroy(server);
   enet_deinitialize();
   printf("[!] : Server Shutdown (ENET_DEINITIALIZE)\n");
#endif
    
    return 0;
}

void* CNetwork::HandleServerPacketsThread(ENetHost* p_server, ENetEvent* p_event, void (*p_HandlePlayerConnected)(ENetEvent&), void (*p_HandlePlayerDisconneted)(ENetEvent&), void (*p_HandlePacketReceive)(ENetEvent&))
{
  printf("[!] : Server Thread Started \n");
  p_HandlePlayerConnected = CNetwork::HandlePlayerConnected;
  p_HandlePlayerDisconneted = CNetwork::HandlePlayerDisconnected;
  p_HandlePacketReceive = CNetwork::HandlePacketReceive;
  
  while (CNetwork::shared_loop_value) // waiting for event
    {
      enet_host_service(p_server, p_event, 1);
        switch (p_event->type)
        {
            case ENET_EVENT_TYPE_CONNECT:
            {
              //CNetwork::HandlePlayerConnected(event);
              p_HandlePlayerConnected(*p_event); // // i'm not sure here , *p_event to return value of pointer and make it as reference
              break;
            }
            case ENET_EVENT_TYPE_RECEIVE:
            {
              //CNetwork::HandlePacketReceive(event);
              p_HandlePacketReceive(*p_event); // // i'm not sure here , *p_event to return value of pointer and make it as reference
              enet_packet_destroy(p_event->packet);
              break;
            }
            case ENET_EVENT_TYPE_DISCONNECT:
            {
              //CNetwork::HandlePlayerDisconnected(event);
              p_HandlePlayerDisconneted(*p_event); // // i'm not sure here , *p_event to return value of pointer and make it as reference
              break;
            }
        }
    }

   enet_host_destroy(p_server);
   enet_deinitialize();
   printf("[!] : Server Shutdown (ENET_DEINITIALIZE)\n");
  
  return NULL;
}

void CNetwork::InitListeners()
{
    CNetwork::AddListener(CPacketsID::PLAYER_ONFOOT, CPlayerPackets::PlayerOnFoot::Handle);
    CNetwork::AddListener(CPacketsID::PLAYER_BULLET_SHOT, CPlayerPackets::PlayerBulletShot::Handle);
    CNetwork::AddListener(CPacketsID::PLAYER_PLACE_WAYPOINT, CPlayerPackets::PlayerPlaceWaypoint::Handle);
    CNetwork::AddListener(CPacketsID::PLAYER_GET_NAME, CPlayerPackets::PlayerGetName::Handle);
    CNetwork::AddListener(CPacketsID::ADD_EXPLOSION, CPlayerPackets::AddExplosion::Handle);
    CNetwork::AddListener(CPacketsID::VEHICLE_SPAWN, CVehiclePackets::VehicleSpawn::Handle);
    CNetwork::AddListener(CPacketsID::VEHICLE_REMOVE, CVehiclePackets::VehicleRemove::Handle);
    CNetwork::AddListener(CPacketsID::VEHICLE_ENTER, CVehiclePackets::VehicleEnter::Handle);
    CNetwork::AddListener(CPacketsID::VEHICLE_EXIT, CVehiclePackets::VehicleExit::Handle);
    CNetwork::AddListener(CPacketsID::VEHICLE_DRIVER_UPDATE, CVehiclePackets::VehicleDriverUpdate::Handle);
    CNetwork::AddListener(CPacketsID::VEHICLE_IDLE_UPDATE, CVehiclePackets::VehicleIdleUpdate::Handle);
    CNetwork::AddListener(CPacketsID::VEHICLE_DAMAGE, CVehiclePackets::VehicleDamage::Handle);
    CNetwork::AddListener(CPacketsID::VEHICLE_COMPONENT_ADD, CVehiclePackets::VehicleComponentAdd::Handle);
    CNetwork::AddListener(CPacketsID::VEHICLE_COMPONENT_REMOVE, CVehiclePackets::VehicleComponentRemove::Handle);
    CNetwork::AddListener(CPacketsID::VEHICLE_PASSENGER_UPDATE, CVehiclePackets::VehiclePassengerUpdate::Handle);
    CNetwork::AddListener(CPacketsID::PLAYER_CHAT_MESSAGE, CPlayerPackets::PlayerChatMessage::Handle);
    CNetwork::AddListener(CPacketsID::PED_SPAWN, CPedPackets::PedSpawn::Handle);
    CNetwork::AddListener(CPacketsID::PED_REMOVE, CPedPackets::PedRemove::Handle);
    CNetwork::AddListener(CPacketsID::PED_ONFOOT, CPedPackets::PedOnFoot::Handle);
    CNetwork::AddListener(CPacketsID::GAME_WEATHER_TIME, CPlayerPackets::GameWeatherTime::Handle); // CPlayerPacket
    CNetwork::AddListener(CPacketsID::PLAYER_KEY_SYNC, CPlayerPackets::PlayerKeySync::Handle); 
    CNetwork::AddListener(CPacketsID::PED_ADD_TASK, CPedPackets::PedAddTask::Handle);
    CNetwork::AddListener(CPacketsID::PED_DRIVER_UPDATE, CPedPackets::PedDriverUpdate::Handle);
    CNetwork::AddListener(CPacketsID::PED_SHOT_SYNC, CPedPackets::PedShotSync::Handle);
    CNetwork::AddListener(CPacketsID::PED_PASSENGER_UPDATE, CPedPackets::PedPassengerSync::Handle);
    CNetwork::AddListener(CPacketsID::PLAYER_AIM_SYNC, CPlayerPackets::PlayerAimSync::Handle);
    CNetwork::AddListener(CPacketsID::PLAYER_STATS, CPlayerPackets::PlayerStats::Handle);
    CNetwork::AddListener(CPacketsID::REBUILD_PLAYER, CPlayerPackets::RebuildPlayer::Handle);
    CNetwork::AddListener(CPacketsID::RESPAWN_PLAYER, CPlayerPackets::RespawnPlayer::Handle);
    CNetwork::AddListener(CPacketsID::START_CUTSCENE, CPlayerPackets::StartCutscene::Handle);
    CNetwork::AddListener(CPacketsID::SKIP_CUTSCENE, CPlayerPackets::SkipCutscene::Handle);
    CNetwork::AddListener(CPacketsID::OPCODE_SYNC, CPlayerPackets::OpCodeSync::Handle);
    CNetwork::AddListener(CPacketsID::ON_MISSION_FLAG_SYNC, CPlayerPackets::OnMissionFlagSync::Handle);
    CNetwork::AddListener(CPacketsID::UPDATE_ENTITY_BLIP, CPlayerPackets::UpdateEntityBlip::Handle);
    CNetwork::AddListener(CPacketsID::REMOVE_ENTITY_BLIP, CPlayerPackets::RemoveEntityBlip::Handle);
    CNetwork::AddListener(CPacketsID::ADD_MESSAGE_GXT, CPlayerPackets::AddMessageGXT::Handle);
    CNetwork::AddListener(CPacketsID::REMOVE_MESSAGE_GXT, CPlayerPackets::RemoveMessageGXT::Handle);
    CNetwork::AddListener(CPacketsID::CLEAR_ENTITY_BLIPS, CPlayerPackets::ClearEntityBlips::Handle);
    CNetwork::AddListener(CPacketsID::PLAY_MISSION_AUDIO, CPlayerPackets::PlayMissionAudio::Handle);
    CNetwork::AddListener(CPacketsID::UPDATE_CHECKPOINT, CPlayerPackets::UpdateCheckpoint::Handle);
    CNetwork::AddListener(CPacketsID::REMOVE_CHECKPOINT, CPlayerPackets::RemoveCheckpoint::Handle);
    CNetwork::AddListener(CPacketsID::ENEX_SYNC, CPlayerPackets::EnExSync::Handle);
    CNetwork::AddListener(CPacketsID::CREATE_STATIC_BLIP, CPlayerPackets::CreateStaticBlip::Handle);
    CNetwork::AddListener(CPacketsID::SET_VEHICLE_CREATED_BY, CVehiclePackets::SetVehicleCreatedBy::Handle);
    CNetwork::AddListener(CPacketsID::SET_PLAYER_TASK, CPlayerPackets::SetPlayerTask::Handle);
    CNetwork::AddListener(CPacketsID::PED_SAY, CPlayerPackets::PedSay::Handle);
    CNetwork::AddListener(CPacketsID::PED_RESET_ALL_CLAIMS, CPedPackets::PedResetAllClaims::Handle);
    CNetwork::AddListener(CPacketsID::PED_TAKE_HOST, CPedPackets::PedTakeHost::Handle);
}

void CNetwork::SendPacket(ENetPeer* peer, unsigned short id, void* data, size_t dataSize, ENetPacketFlag flag)
{
    // 2 == sizeof(unsigned short)

    // packet size `id + data`
    size_t packetSize = 2 + dataSize;

    // create buffer
    char* packetData = new char[packetSize];

    // copy id
    memcpy(packetData, &id, 2);

    // copy data
    memcpy(packetData + 2, data, dataSize);

    // create packet
    ENetPacket* packet = enet_packet_create(packetData, packetSize, flag);

    delete[] packetData;

    // send packet
    enet_peer_send(peer, 0, packet);
}

void CNetwork::SendPacketToAll(unsigned short id, void* data, size_t dataSize, ENetPacketFlag flag, ENetPeer* dontShareWith)
{
    size_t packetSize = 2 + dataSize;
    char* packetData = new char[packetSize];
    memcpy(packetData, &id, 2);
    memcpy(packetData + 2, data, dataSize);
    ENetPacket* packet = enet_packet_create(packetData, packetSize, flag);

    delete[] packetData;

    for (int i = 0; i != CPlayerManager::m_pPlayers.size(); i++)
    {
        if (CPlayerManager::m_pPlayers[i]->m_pPeer != dontShareWith)
        {
            enet_peer_send(CPlayerManager::m_pPlayers[i]->m_pPeer, 0, packet);
        }
    }
}

void CNetwork::SendPacketRawToAll(void* data, size_t dataSize, ENetPacketFlag flag, ENetPeer* dontShareWith)
{
    ENetPacket* packet = enet_packet_create(data, dataSize, flag);

    for (int i = 0; i != CPlayerManager::m_pPlayers.size(); i++)
    {
        if (CPlayerManager::m_pPlayers[i]->m_pPeer != dontShareWith)
        {
            enet_peer_send(CPlayerManager::m_pPlayers[i]->m_pPeer, 0, packet);
        }
    }
}

void CNetwork::HandlePlayerConnected(ENetEvent& event)
{
    uint32_t packedVersion = semver_parse(COOPANDREAS_VERSION, nullptr);
    char buffer[23];
    semver_t playerVersion;
    semver_unpack(event.data, &playerVersion);
    semver_to_string(&playerVersion, buffer, sizeof(buffer));
    buffer[22] = '\0';

    printf("[Game] : A new client connected from %i.%i.%i.%i:%u. Version: %s\n", 
        event.peer->address.host & 0xFF, 
        (event.peer->address.host >> 8) & 0xFF, 
        (event.peer->address.host >> 16) & 0xFF, 
        (event.peer->address.host >> 24) & 0xFF, 
        event.peer->address.port, buffer);

    if (packedVersion != event.data)
    {
        printf("Wrong version, disconnecting...\n");
        //enet_peer_disconnect_now(event.peer, packedVersion);
        return;
    }

    // set player disconnection timeout
    enet_peer_timeout(event.peer, 5000, 3000, 5000); //timeoutLimit, timeoutMinimum, timeoutMaximum

    // create new player and send to all players

    // get free id
    int freeId = CPlayerManager::GetFreeID();

    // create new player instance
    CPlayer* player = new CPlayer(event.peer, freeId);

    // add player to list
    CPlayerManager::Add(player);

    // create PlayerConnected packet struct
    CPlayerPackets::PlayerConnected packet =
    {
        freeId // id
    };

    // send to all
    CNetwork::SendPacketToAll(CPacketsID::PLAYER_CONNECTED, &packet, sizeof (CPlayerPackets::PlayerConnected), ENET_PACKET_FLAG_RELIABLE, event.peer);

    // send PlayerConnected packets for a new player
    for (auto i : CPlayerManager::m_pPlayers)
    {
        if (i->m_iPlayerId == freeId)
            continue;

        packet.id = i->m_iPlayerId;
        packet.isAlreadyConnected = true;

        CNetwork::SendPacket(event.peer, CPacketsID::PLAYER_CONNECTED, &packet, sizeof (CPlayerPackets::PlayerConnected), ENET_PACKET_FLAG_RELIABLE);

        CPlayerPackets::PlayerGetName getNamePacket{};
        getNamePacket.playerid = i->m_iPlayerId;
        strcpy(getNamePacket.name, i->m_Name);
        CNetwork::SendPacket(event.peer, CPacketsID::PLAYER_GET_NAME, &getNamePacket, sizeof (CPlayerPackets::PlayerGetName), ENET_PACKET_FLAG_RELIABLE);

        if (i->m_ucSyncFlags.bStatsModified)
        {
            CPlayerPackets::PlayerStats statsPacket{};
            statsPacket.playerid = i->m_iPlayerId;
            memcpy(statsPacket.stats, i->m_afStats, sizeof(i->m_afStats));
            CNetwork::SendPacket(event.peer, CPacketsID::PLAYER_STATS, &statsPacket, sizeof(statsPacket), ENET_PACKET_FLAG_RELIABLE);
        }

        if (i->m_ucSyncFlags.bClothesModified)
        {
            CPlayerPackets::RebuildPlayer rebuildPacket{};
            rebuildPacket.playerid = i->m_iPlayerId;
            memcpy(rebuildPacket.m_anModelKeys, i->m_anModelKeys, sizeof(i->m_anModelKeys));
            memcpy(rebuildPacket.m_anTextureKeys, i->m_anTextureKeys, sizeof(i->m_anTextureKeys));
            rebuildPacket.m_fFatStat = i->m_fFatStat;
            rebuildPacket.m_fMuscleStat = i->m_fMuscleStat;
            CNetwork::SendPacket(event.peer, CPacketsID::REBUILD_PLAYER, &rebuildPacket, sizeof(rebuildPacket), ENET_PACKET_FLAG_RELIABLE);
        }

        if (i->m_ucSyncFlags.bWaypointModified)
        {
            CPlayerPackets::PlayerPlaceWaypoint waypointPacket{};
            waypointPacket.playerid = i->m_iPlayerId;
            waypointPacket.position = i->m_vecWaypointPos;
            waypointPacket.place = true;
            CNetwork::SendPacket(event.peer, CPacketsID::PLAYER_PLACE_WAYPOINT, &waypointPacket, sizeof(waypointPacket), ENET_PACKET_FLAG_RELIABLE);
        }
    }

    for (auto i : CVehicleManager::m_pVehicles)
    {
        CVehiclePackets::VehicleSpawn vehicleSpawnPacket{};
        vehicleSpawnPacket.vehicleid = i->m_nVehicleId;
        vehicleSpawnPacket.modelid = i->m_nModelId;
        vehicleSpawnPacket.pos = i->m_vecPosition;
        vehicleSpawnPacket.rot = static_cast<float>(i->m_vecRotation.z * (3.141592 / 180)); // convert to radians
        vehicleSpawnPacket.color1 = i->m_nPrimaryColor;
        vehicleSpawnPacket.color2 = i->m_nSecondaryColor;

        CNetwork::SendPacket(event.peer, CPacketsID::VEHICLE_SPAWN, &vehicleSpawnPacket, sizeof vehicleSpawnPacket, ENET_PACKET_FLAG_RELIABLE);
        
        bool modifiedDamageStatus = false;

        for (unsigned char j = 0; j < 23; j++)
        {
            if (i->m_damageManager_padding[j])
            {
                modifiedDamageStatus = true; 
                break;
            }
        }

        if (modifiedDamageStatus)
        {
            CVehiclePackets::VehicleDamage vehicleDamagePacket{};
            vehicleDamagePacket.vehicleid = i->m_nVehicleId;
            memcpy(&vehicleDamagePacket.damageManager_padding, i->m_damageManager_padding, 23);
            CNetwork::SendPacket(event.peer, CPacketsID::VEHICLE_DAMAGE, &vehicleDamagePacket, sizeof vehicleDamagePacket, ENET_PACKET_FLAG_RELIABLE);
        }

        for (int component : i->m_pComponents) 
        {
            CVehiclePackets::VehicleComponentAdd vehicleComponentAddPacket{};
            vehicleComponentAddPacket.vehicleid = i->m_nVehicleId;
            vehicleComponentAddPacket.componentid = component;
            CNetwork::SendPacket(event.peer, CPacketsID::VEHICLE_COMPONENT_ADD, &vehicleComponentAddPacket, sizeof vehicleComponentAddPacket, ENET_PACKET_FLAG_RELIABLE);
        }
    }

    for (auto i : CPedManager::m_pPeds)
    {
        CPedPackets::PedSpawn packet{};
        packet.pedid = i->m_nPedId;
        packet.modelId = i->m_nModelId;
        packet.pos = i->m_vecPos;
        packet.pedType = i->m_nPedType;
        packet.createdBy = i->m_nCreatedBy;
        strncpy_s(packet.specialModelName, i->m_szSpecialModelName, strlen(i->m_szSpecialModelName));
        CNetwork::SendPacket(event.peer, CPacketsID::PED_SPAWN, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
    }

    if (CPlayerPackets::EnExSync::ms_pLastPlayerOwner)
    {
        if (std::find(CPlayerManager::m_pPlayers.begin(), CPlayerManager::m_pPlayers.end(), CPlayerPackets::EnExSync::ms_pLastPlayerOwner)
            != CPlayerManager::m_pPlayers.end())
        {
            CNetwork::SendPacket(event.peer, CPacketsID::ENEX_SYNC, CPlayerPackets::EnExSync::ms_vLastData.data(), CPlayerPackets::EnExSync::ms_vLastData.size(), ENET_PACKET_FLAG_RELIABLE);
        }
    }

    CPlayerPackets::PlayerHandshake handshakePacket = { freeId };
    CNetwork::SendPacket(event.peer, CPacketsID::PLAYER_HANDSHAKE, &handshakePacket, sizeof handshakePacket, ENET_PACKET_FLAG_RELIABLE);

    CPlayerManager::AssignHostToFirstPlayer();
}

void CNetwork::HandlePlayerDisconnected(ENetEvent& event)
{
    // disconnect player

    // find player instance by enetpeer
    CPlayer* player = CPlayerManager::GetPlayer(event.peer);
    
    CVehicle* vehicle = CVehicleManager::GetVehicle(player->m_nVehicleId);

    if (vehicle != nullptr)
    {
        vehicle->m_pPlayers[player->m_nSeatId] = nullptr;
    }
    
    if (CPlayerPackets::EnExSync::ms_pLastPlayerOwner == player)
    {
        CPlayerPackets::EnExSync::ms_pLastPlayerOwner = nullptr;
    }

    CPedManager::RemoveAllHostedAndNotify(player);
    CVehicleManager::RemoveAllHostedAndNotify(player);

    // remove
    CPlayerManager::Remove(player);

    // create PlayerDisconnected packet struct
    CPlayerPackets::PlayerDisconnected packet =
    {
        player->m_iPlayerId // id
    };

    // send to all
    CNetwork::SendPacketToAll(CPacketsID::PLAYER_DISCONNECTED, &packet, sizeof (CPlayerPackets::PlayerDisconnected), (ENetPacketFlag)0, event.peer);

    printf("[Game] : %i Disconnected.\n", player->m_iPlayerId);

    CPlayerManager::AssignHostToFirstPlayer();
}

void CNetwork::HandlePacketReceive(ENetEvent& event)
{
    // get packet id
    unsigned short id;
    memcpy(&id, event.packet->data, 2);

    if (id == CPacketsID::MASS_PACKET_SEQUENCE)
    {
        CNetwork::SendPacketRawToAll(event.packet->data, event.packet->dataLength, (ENetPacketFlag)event.packet->flags, event.peer);
    }
    else
    {
        // get data
        char* data = new char[event.packet->dataLength - 2];
        memcpy(data, event.packet->data + 2, event.packet->dataLength - 2);
        // call listener's callback by id
        auto it = m_packetListeners.find(id);
        if (it != m_packetListeners.end())
        {
            it->second->m_callback(event.peer, data, (int)event.packet->dataLength - 2);
        }

        delete[] data;
    }
}

void CNetwork::AddListener(unsigned short id, void(*callback)(ENetPeer*, void*, int))
{
    CPacketListener* listener = new CPacketListener(id, callback);
    m_packetListeners.insert({ id, listener });
}
