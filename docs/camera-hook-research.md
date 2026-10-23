# Camera-hook research notes

Target: Minecraft Bedrock for Windows x64, version 1.26.52.3, launched with BedrockOnLinux/Wine.

## Runtime fingerprint captured so far

The diagnostic DLL reported:
- TimeDateStamp: 0x6AB54E37
- SizeOfImage: 0x12C01000
- image base: 0x140000000
- PE32+ x64 image with 8 sections

The diagnostic output did not include the optional-header checksum. The diagnostic logger now records that field as well, because the reference resolver fingerprints a loaded image using timestamp, image size, and checksum. The checksum is not assumed to be nonzero or unique by itself.

## Camera implementation research

The MIT-licensed Minecraft Bedrock Edition Head Tracking project (https://github.com/itsloopyo/minecraft-bedrock-edition-headtracking) provides a useful technical reference for native camera manipulation:
- It resolves the camera setup function from the in-memory executable instead of relying only on a hard-coded function address.
- It resolves camera layout from the code path that reads it, and leaves the game unchanged when resolution fails.
- It composes rotation into the camera's post-view transform, then restores the game's original transform.
- It has a separate crosshair-projection path because changing only the rendered view can otherwise make the crosshair disagree with the game's aim direction.
- Its discovery mode applies one synthetic axis at a time so pitch, yaw, and roll can be measured rather than guessed.

This is research and attribution, not a claim that its resolver has been ported or verified for 1.26.52.3. Its camera component offsets and function signatures must not be copied blindly into this DLL. Its implementation also has session-policy checks and multiple dependent modules; transplanting only the hook body would be unsafe and incomplete.

## Next implementation gates

1. Capture the new runtime checksum and compare the exact fingerprint.
2. Port or independently implement a camera resolver with strict uniqueness checks.
3. Confirm the resolved camera setup ABI and field layout before any write.
4. Add a guarded render hook that restores the original transform on every path.
5. Add configurable input, smooth pitch/yaw/roll, and gliding-state detection.
6. Test in a local world and verify aim/crosshair behavior and clean shutdown.

Until these gates pass, this branch is not a playable mod. A failed resolver must leave the game untouched.
