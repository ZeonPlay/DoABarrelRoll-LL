# Do A Barrel Roll — Bedrock Native DLL

Native Windows x64 DLL prototype for Minecraft Bedrock running through Wine, with BedrockOnLinux as the first intended launcher.

**Target version:** Minecraft Bedrock **1.26.52.3** (reported by the developer).  
**LeviLamina dependency:** none for the native DLL build.

> [!WARNING]
> This branch is currently a buildable architecture scaffold, not a playable mod. The DLL entry point and standalone rotation math exist, but the Minecraft-specific hooks are not implemented yet. Do not expect the player or camera to rotate after injecting it.

## Build the native DLL

On Windows x64 with Visual Studio Build Tools and CMake:

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release
```

Output: `build/Release/DoABarrelRoll.dll`

GitHub Actions also builds the DLL on Windows and publishes it as a workflow artifact.

## Design

- `src/core/`: game-independent rotation math.
- `src/native/`: Windows DLL entry point.
- `CMakeLists.txt`: standalone Windows DLL build, independent of LeviLamina.
- `README-native.md`: current implementation status and next milestone.

The old `xmake.lua` and `tooth.json` are still from the original LeviLamina template and are not used by the CMake build.

## Next milestone

Implement and verify the Bedrock 1.26.52.3-specific hooks for input, player rotation, and camera orientation. Those hooks must be based on the exact game binary; guessed offsets are not safe or reliable.

## Reference

The rotation approach is informed by [Do-A-Barrel-Roll-Bedrock](https://github.com/theaddonn/Do-A-Barrel-Roll-Bedrock). This project is a separate native-DLL implementation and is not affiliated with Mojang or Microsoft.
