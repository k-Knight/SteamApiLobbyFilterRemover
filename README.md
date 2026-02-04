# SteamApiLobbyFilterRemover

**SteamApiLobbyFilterRemover** is a proxy utility designed to intercept and modify Steamworks API communication. By acting as a wrapper for `steam_api64.dll`, this tool hooks internal functions to target `ISteamMatchmaking::RequestLobbyList`, effectively stripping search filters to allow for unrestricted lobby enumeration.

## ⚠️ Critical Warnings

> [!CAUTION]
> **VAC BAN RISK:** This software utilizes function hooking on the Steam API. Use of this proxy in any VAC-secured (Valve Anti-Cheat) game **will result in a permanent account ban**. Use is strictly intended for non-secure environments or research purposes.

> [!IMPORTANT]
> **INSTALLATION REQUIREMENT:** To function correctly, the original Steam API library must be renamed. Failure to do so will result in application crashes or initialization errors.

## Installation

1. Navigate to the target application's directory containing the original `steam_api64.dll`.
2. Rename the original file from `steam_api64.dll` to **`steam_api64_o.dll`**.
3. Place the proxy `steam_api64.dll` from this repository into the same directory.
4. Launch the application as usual.

## Technical Overview

The proxy library exports the standard function set of the official Steam API. Upon execution, it:
* Loads the renamed original library (`steam_api64_o.dll`) to maintain legitimate API functionality.
* Implements hooks on the Matchmaking interface.
* Intercepts `RequestLobbyList()` calls to remove or bypass developer-defined filters before the request is sent to Steam servers.

## Disclaimer

This project is provided for informational and educational purposes only. Modifying API behavior may violate the Steam Subscriber Agreement and lead to account restrictions. The developers assume no liability for any consequences resulting from the use of this software.