/*
** EPITECH PROJECT, 2022
** B-YEP-400-STG-4-1-indiestudio-mathieu.brujan
** File description:
** Network
*/

#pragma once

#include <memory>
#include <sstream>
#include <iostream>
#include <chrono>
#include <unordered_map>
#include "network/IClient.hpp"
#include "network/IServer.hpp"
#include "network/CPSocket.hpp"
#include "ecs/engine/World.hpp"
#include "ecs/engine/PlayerId.hpp"

namespace ecs {
    using ConnId = network::ConnId;

    const std::size_t NB_MIRROR_COMPONENTS = 7;
    extern const ComponentHash MIRROR_COMPONENTS[NB_MIRROR_COMPONENTS];

    enum class NetworkCommand : uint8_t {
        UPDATE_ENTITY,
        KILL_ENTITY,
        INIT_PLAYERS,
        PLAYERS_CREATED,
        PLAYERS_REJECTED,
        DISCONNECT_CLIENT,
    };

    struct MirrorEntity {};

    class ServerManager {
        std::unique_ptr<network::IServer> _server;
        std::vector<ConnId> _activeConns;
        std::unordered_map<network::ConnId, std::unordered_map<Entity, Entity>> _clientToServer;

        void sendCmd(ConnId conn, NetworkCommand cmd);

        bool tryRead(ConnId conn, void *buf, std::size_t size);
        void handleClientDisconnection(ConnId conn, World &world);
        bool readClientEntityUpdate(ConnId conn, std::stringbuf &buffer);

        void spawnOrUpdateClientEntity(ConnId conn, World &world);
        void spawnClientEntity(ConnId conn, Entity serverEntity, std::stringbuf &buffer, World &world);
        void updateClientEntity(ConnId conn, Entity serverEntity, std::stringbuf &buffer, World &world);
        void killClientEntity(ConnId conn, World &world);

        void createUpdateLocalEntityBuffer(Entity entity, World &world, std::stringbuf &buffer);
        void createKillLocalEntityBuffer(Entity entity, std::stringbuf &buffer);

        void initPlayers(ConnId conn, World &world);

        public:
        ServerManager() : _server(network::CPSocket::createServer()) {};
        ~ServerManager() = default;

        void startServer(const std::string &port) { _server->createServer("127.0.0.1", port); };
        void closeServer() { _server->closeServer(); };

        void acceptNewConns();
        void handleNetworkCommands(World &world);

        void updateLocalEntity(Entity entity, World &world);
        void killLocalEntity(Entity entity, World &world);
    };

    using ConnectionSuccessFct = void (*)(void *obj, ecs::World &world);
    using ConnectionFailedFct = void (*)(void *obj, ecs::World &world);

    class ClientManager {
        std::unique_ptr<network::IClient> _client;
        std::unordered_map<Entity, Entity> _serverToClient;

        std::string _ip;
        std::string _port;
        bool _connAttempted = false;
        int _tryConnCount = 0;
        void *_obj = nullptr;
        ConnectionSuccessFct _success;
        ConnectionFailedFct _failed;
        std::chrono::time_point<std::chrono::system_clock> _lastTry;

        bool tryRead(void *buf, std::size_t size);
        void handleDisconnect(World &world);
        bool readServerEntityUpdate(std::stringbuf &buffer);

        void spawnOrUpdateServerEntity(World &world);
        void spawnServerEntity(Entity serverEntity, std::stringbuf &buffer, World &world);
        void updateServerEntity(Entity serverEntity, std::stringbuf &buffer, World &world);
        void killServerEntity(World &world);

        void handlePlayersCreated(World &world);

        public:
        ClientManager() : _client(network::CPSocket::createClient()) {};
        ~ClientManager() = default;

        void attemptConnection(const std::string &ip, const std::string &port,
        void *obj, ConnectionSuccessFct success, ConnectionFailedFct failed);
        void tryConnection(ecs::World &world);
        bool isConnectionAttempt() { return _connAttempted; };
        void initPlayers(std::uint32_t nbPlayers);

        void handleNetworkCommands(World &world);

        void updateLocalEntity(Entity entity, World &world);
        void killLocalEntity(Entity entity, World &world);

        void connectTo(const std::string &ip = "127.0.0.1", const std::string &port = "4242") { _client->connectTo(ip, port); };
        void disconnect() { _client->disconnect(); };
        bool isConnected() { return _client->isConnected(); };
    };

    class ServerUpdateSystem : public ASystem {
        public:
        ServerUpdateSystem() { _stage = NETWORK_UPDATE; };

        void setSignature(ComponentManager &component);
        void update(World &world);
    };

    class ClientUpdateSystem : public ASystem {
        public:
        ClientUpdateSystem() { _stage = NETWORK_UPDATE; };

        void setSignature(ComponentManager &component);
        void update(World &world);
    };

    class NetworkSceneModule {
        public:
        virtual int getNbPlayersOnClient() = 0;
        virtual void onDisconnect(World &world) = 0;
        virtual void playerIdAssigned(PlayerId id, World &world) = 0;
    };
}
