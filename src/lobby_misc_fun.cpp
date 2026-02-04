#include <steam_api.h>

struct CallbackBase {
    uint8 m_nCallbackFlags;
    int m_iCallback;
    virtual void Run(void* pvParam) = 0;
    virtual void Run(void* pvParam, bool bIOFailure, SteamAPICall_t hSteamAPICall) = 0;
    virtual int GetCallbackSizeBytes() = 0;
};

struct my_LobbyEnter_t { enum { k_iCallback = 504 }; CSteamID m_ulSteamIDLobby; uint32 m_rgfChatPermissions; bool m_bLocked; uint32 m_EChatRoomEnterResponse; };
typedef void (*SteamAPI_RegisterCallResult_t)(CallbackBase* pCallback, SteamAPICall_t hAPICall);

class ManualJoinResult : public CallbackBase {
public:
    void Run(void* pvParam) override {}
    void Run(void* pvParam, bool bIOFailure, SteamAPICall_t hSteamAPICall) override {
        auto* pCallback = (my_LobbyEnter_t*)pvParam;
        if (pCallback->m_EChatRoomEnterResponse == 1) { // 1 = Success
            printf("[SteamApiLobbyFilterRemover] Successfully joined lobby: %llu\n", pCallback->m_ulSteamIDLobby.ConvertToUint64());
        }
        else {
            printf("[SteamApiLobbyFilterRemover] Failed to join lobby. Response code: %d\n", pCallback->m_EChatRoomEnterResponse);
        }
    }
    int GetCallbackSizeBytes() override { return sizeof(my_LobbyEnter_t); }
};


ManualJoinResult g_ManualJoinResult;

void JoinTargetLobby(CSteamID lobbyID) {
    ISteamMatchmaking* pMatch = TrueSteamMatchmaking_v009();

    printf("[SteamApiLobbyFilterRemover] Attempting to join lobby: %llu...\n", lobbyID.ConvertToUint64());

    SteamAPICall_t hCall = pMatch->JoinLobby(lobbyID);

    g_ManualJoinResult.m_iCallback = my_LobbyEnter_t::k_iCallback;
    auto RegisterCall = (SteamAPI_RegisterCallResult_t)GetProcAddress(steam_ddl_instance, "SteamAPI_RegisterCallResult");
    if (RegisterCall) {
        RegisterCall(&g_ManualJoinResult, hCall);
    }
}

class ManualLobbyResult : public CallbackBase {
public:
    void Run(void* pvParam) override {}

    void Run(void* pvParam, bool bIOFailure, SteamAPICall_t hSteamAPICall) override {
        auto* pCallback = (LobbyMatchList_t*)pvParam;
        if (bIOFailure) {
            printf("[SteamApiLobbyFilterRemover] Lobby request failed (IO Error)\n");
            return;
        }

        printf("[SteamApiLobbyFilterRemover] Found %d lobbies!\n", pCallback->m_nLobbiesMatching);
        ISteamMatchmaking* pMatch = TrueSteamMatchmaking_v009();
        for (uint32 i = 0; i < pCallback->m_nLobbiesMatching; i++) {
            CSteamID id = pMatch->GetLobbyByIndex(i);
            printf("[SteamApiLobbyFilterRemover]   Lobby %d: %llu\n", i, id.ConvertToUint64());
            //if (i == 0) {
            //    JoinTargetLobby(id);
            //}
        }
    }

    int GetCallbackSizeBytes() override { return sizeof(LobbyMatchList_t); }
};

ManualLobbyResult g_ManualLobbyResult;

void list_lobbies() {
    g_pMatchmaking->AddRequestLobbyListDistanceFilter(k_ELobbyDistanceFilterWorldwide);
    g_pMatchmaking->AddRequestLobbyListResultCountFilter(100);
    SteamAPICall_t hSteamAPICall = g_pMatchmaking->RequestLobbyList();

    g_ManualLobbyResult.m_iCallback = LobbyMatchList_t::k_iCallback; // 510
    g_ManualLobbyResult.m_nCallbackFlags = 0;

    auto RegisterCall = (SteamAPI_RegisterCallResult_t)GetProcAddress(steam_ddl_instance, "SteamAPI_RegisterCallResult");
    if (RegisterCall) {
        RegisterCall(&g_ManualLobbyResult, hSteamAPICall);
        printf("[SteamApiLobbyFilterRemover] callback registered manually !\n");
    }
}