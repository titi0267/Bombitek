/*
** EPITECH PROJECT, 2022
** B-YEP-400-STG-4-1-indiestudio-mathieu.brujan
** File description:
** setup
*/

#include "Setup.hpp"

#include "ecs/engine/Network.hpp"

#include "ecs/components/DrawableCube.hpp"
#include "ecs/components/DrawableModel.hpp"
#include "ecs/components/Hitbox.hpp"
#include "ecs/components/Clickable.hpp"
#include "ecs/components/Hoverable.hpp"
#include "ecs/components/HoverTint.hpp"
#include "ecs/components/HoverRotate.hpp"
#include "ecs/components/Text3D.hpp"
#include "ecs/components/ColorTexture.hpp"
#include "ecs/components/SceneMoveElement.hpp"
#include "ecs/components/PlayerInputs.hpp"

#include "network/sockets/SocketInclude.hpp"

// Those components are mirrored between clients and server
// THOSE COMPONENTS SHOULD ALWAYS BE REGISTERED FIRST !!
void bomberman::registerCriticalComponents(ecs::World &world)
{
    world.registerComponents<Transform, ecs::Hitbox, ecs::PlayerInputs,
    ecs::TextureRef, ecs::ModelRef, ecs::FontRef, ecs::Tint>();
}

void bomberman::registerRender(ecs::World &world)
{
    world.registerComponent<ecs::ModelRef>();
    world.registerSystem<ecs::DrawableModelSystem>();

    world.registerComponents<ecs::Text3D>();
    world.registerSystem<ecs::Draw3DTextSystem>();
}

void bomberman::registerInputs(ecs::World &world)
{
    world.registerComponents<ecs::Clickable, ecs::Hoverable>();
    world.registerSystems<ecs::ClickUpdateSystem, ecs::HoverUpdateSystem>();

    world.registerSystems<ecs::PlayerInputsUpdateSystem>();

    world.registerComponents<ecs::HoverTint, ecs::HoverRotate, ecs::SceneMoveElement>();
    world.registerSystems<ecs::HoverTintUpdateSystem, ecs::HoverRotateUpdateSystem, ecs::SceneMoveElementSystem>();
}

void bomberman::registerNetwork(ecs::World &world, bool client)
{
    world.registerComponent<ecs::MirrorEntity>();
    if (client) {
        world.registerSystem<ecs::ClientUpdateSystem>();
    } else {
        world.registerSystem<ecs::ServerUpdateSystem>();
    }
}

int bomberman::WSA(void)
{
    #ifdef _WIN32
    WSACleanup();
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData))
        return (1);
    #endif
    return (0);
}
