# Do A Barrel Roll — Bedrock Native DLL

Native Windows x64 DLL prototype for Minecraft Bedrock running through Wine, with BedrockOnLinux as the first intended launcher.

**Target version:** Minecraft Bedrock **1.26.52.3**.  
**LeviLamina dependency:** none for the native DLL build.

> [!WARNING]
> This branch is **not yet a playable barrel-roll mod**. The current DLL is a diagnostic build: it logs the in-memory PE header and section metadata of the main game module. It does not hook input, player rotation, camera rotation, or gliding state. Do not enable it as a gameplay client expecting rotation.

## Current diagnostic build

After injection, the DLL writes:

`%TEMP%\\DoABarrelRoll-diagnostics.log`

Under BedrockOnLinux/Wine, find the active prefix's Windows TEMP directory. The exact mapped path depends on the prefix and Wine configuration. The log reports the main module path (when available), image base, PE timestamp, image size, and section metadata. It does not dump game memory or modify the game.

The diagnostic log is needed to establish what image is actually loaded at runtime. The encrypted Store file on disk is not the decrypted executable image used by the game.

## Build the native DLL

On Windows x64 with Visual Studio Build Tools and CMake:

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

GitHub Actions builds the DLL on Windows and publishes it as a workflow artifact.

## Design

- `src/core/`: game-independent rotation math.
- `src/native/`: Windows DLL entry point, diagnostics, and signature scanner infrastructure.
- `CMakeLists.txt`: standalone Windows DLL build, independent of LeviLamina.

The old `xmake.lua` and `tooth.json` are from the original LeviLamina template and are not used by the CMake build.

## Remaining work before gameplay

1. Inspect the loaded 1.26.52.3 image and identify candidate functions.
2. Verify signatures against the actual runtime image; do not use guessed offsets.
3. Implement safe input handling and a camera/player rotation hook.
4. Detect gliding/elytra state and apply smooth pitch/yaw/roll with configurable controls.
5. Test in a local world, validate cleanup/unload behavior, and only then publish a gameplay DLL.

A PE header and successful DLL injection are not proof that gameplay hooks work. The hook addresses and data layouts must be verified for this exact game version.

## Reference

The rotation approach is informed by [Do-A-Barrel-Roll-Bedrock](https://github.com/theaddonn/Do-A-Barrel-Roll-Bedrock). This project is a separate native-DLL implementation and is not affiliated with Mojang or Microsoft.
