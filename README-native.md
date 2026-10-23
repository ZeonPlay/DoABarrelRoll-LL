# Do A Barrel Roll — Native DLL prototype

This branch starts a Windows x64 DLL path intended for Minecraft Bedrock launched through Wine, including BedrockOnLinux. It does **not** require LeviLamina at build or runtime.

## Target

- Minecraft Bedrock for Windows: **1.26.52.3** (user-reported target)
- Runtime: Wine/GDK-Proton through BedrockOnLinux
- Artifact: `DoABarrelRoll.dll`

## Current status

This is an initial native-DLL scaffold, **not a playable mod yet**.

Implemented:
- A standalone DLL entry point with debug logging.
- A standalone rotation-math module derived from the reference project's local-frame rotation approach.
- A Windows x64 GitHub Actions build.
- A PE executable-section signature scanner with wildcard byte patterns and match-count reporting.
- Unit tests for rotation math and signature-pattern parsing.

Not implemented yet:
- Verified byte signatures for the exact 1.26.52.3 player/input/camera functions.
- Installing hooks into the game.
- Reading gliding state and input from Bedrock.
- Applying player and camera rotations.
- In-game configuration and controls.

The scanner is only infrastructure: it does not yet contain verified Minecraft signatures and does not install hooks. Those parts must be implemented against the exact game binary. Guessing addresses or reusing LeviLamina hook code would produce a DLL that may load but cannot safely control the player.

## Build

Use a Windows x64 Visual Studio Build Tools environment with CMake:

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release
```

The output is `build/Release/DoABarrelRoll.dll`.

The project intentionally does not link LeviLamina. A DLL injector that supports the target Wine process can load the DLL, but loading it alone will only emit a debug message until game hooks are implemented.

## Next milestone

Inspect the exact Bedrock 1.26.52.3 binary and establish version-specific hook signatures for input, player rotation, and camera orientation. Then add the hook backend and validate it in a disposable world before testing survival or multiplayer.
