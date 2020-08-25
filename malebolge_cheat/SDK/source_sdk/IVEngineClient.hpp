#pragma once
#include <cstdint>
#include "../VirtualMethod.hpp"
#include "../Vector.hpp"

struct Matrix4x4 {
    union {
        struct {
            float _11, _12, _13, _14;
            float _21, _22, _23, _24;
            float _31, _32, _33, _34;
            float _41, _42, _43, _44;

        };
        float m[4][4];
    };
};

struct PlayerInfo {
    std::uint64_t version;
    union {
        std::uint64_t xuid;
        struct {
            std::uint32_t xuidLow;
            std::uint32_t xuidHigh;
        };
    };
    char name[128];
    int userId;
    char guid[33];
    std::uint32_t friendsId;
    char friendsName[128];
    bool fakeplayer;
    bool hltv;
    int customfiles[4];
    unsigned char filesdownloaded;
    int entityIndex;
};

class NetworkChannel;

class Engine {
public:
		VIRTUAL_METHOD(bool, getPlayerInfo, 8, (int entityIndex, PlayerInfo& playerInfo), (this, entityIndex, std::ref(playerInfo)))
        VIRTUAL_METHOD(int, getPlayerForUserID, 9, (int userId), (this, userId))
        VIRTUAL_METHOD(int, getMaxClients, 20, (), (this))
        VIRTUAL_METHOD(bool, isInGame, 26, (), (this))
        VIRTUAL_METHOD(bool, isConnected, 27, (), (this))
        VIRTUAL_METHOD(void*, getBSPTreeQuery, 43, (), (this))
        VIRTUAL_METHOD(const char*, getLevelName, 53, (), (this))
        VIRTUAL_METHOD(NetworkChannel*, getNetworkChannel, 78, (), (this))
        VIRTUAL_METHOD(void, clientCmdUnrestricted, 114, (const char* cmd), (this, cmd, false))
        VIRTUAL_METHOD(const Matrix4x4&, worldToScreenMatrix, 37, (), (this))
};