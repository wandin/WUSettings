# WUSettings Guidelines

Architecture and coding conventions for the WUSettings plugin. Keep this file up to date when a design decision changes.

## Modules

| Module | Type | Purpose |
|---|---|---|
| `WUSettingsRuntime` | Runtime | Core: descriptors, registry, values, persistence, subsystems, appliers. |
| `WUSettingsUI` | Runtime | UMG settings menu. Logic in C++, Blueprint only for styling. |
| `WUSettingsEditor` | Editor | Editor tooling. |

Inside a module, code is organized **by domain folder** rather than by Public/Private:

```
Source/WUSettingsRuntime/
  Appliers/        IWUSettingApplier, FWUSettingsApplierRegistry, Audio/, Video/
  Defaults/        Built-in descriptors (WUSettingsDefaults::Register*Settings)
  Logging/         Log categories
  Persistence/     FWUSettingValue, UWUSettingSaveGame
  Settings/        UWUSettingsDeveloperSettings (Project Settings > WU Settings)
  SettingsCore/    FWUSettingDescriptor, FWUSettingsRegistry, enums
  Subsystems/      UWUSettingsSubsystem (GameInstance), UWUSettingsWorldSubsystem
  Video/           Video-specific types (EWUWindowMode, FWUResolution)
  Public/ Private/ Module class only
```

Includes always use the module-rooted path: `#include "WUSettingsRuntime/SettingsCore/WUSettingsRegistry.h"`.

## Data flow

```
FWUSettingDescriptor ──register──> FWUSettingsRegistry (singleton)
                                          │ default value
UI / debug ──SetSettingValue──> UWUSettingsSubsystem ──> SanitizeValue (type check + clamp)
                                          │ cache ──> SaveSettings (.sav, slot "WUSettings")
                                          └──> FWUSettingsApplierRegistry::FindApplier ──> IWUSettingApplier::Apply
World BeginPlay ──> UWUSettingsWorldSubsystem ──> ApplyAllRegisteredSettings(World)
```

- **Value precedence:** descriptor default, then config, then SaveGame.
- **Registration** happens explicitly in `FWUSettingsRuntimeModule::StartupModule`. Do not use static auto-registration objects. `ShutdownModule` resets both registries. Registering a name that already exists replaces it, which lets a project override plugin defaults.
- **Validation:** unknown names and type mismatches are rejected with a Warning. Float and Int values are clamped to the descriptor's Min/Max. On load, saved entries whose name is unknown or whose type no longer matches are dropped.
- **Saving:** every successful `SetSettingValue` saves, even when there is no world.
- **Applying:** `UWUSettingsSubsystem::Initialize` only loads, because no world exists at that point. `UWUSettingsWorldSubsystem::OnWorldBeginPlay` applies everything, for Game and PIE worlds only.
- **SaveGame versioning:** `UWUSettingSaveGame::CurrentVersion`. Migrations go in `LoadSettings`.

## Rules

- **UI talks only to `UWUSettingsSubsystem`** (`GetSettingValue` / `SetSettingValue`), never to Exec commands or appliers.
- **No hardcoded asset paths.** Reference assets with `TSoftObjectPtr` in `UWUSettingsDeveloperSettings`, which is stored in `DefaultGame.ini` under `[/Script/WUSettingsRuntime.WUSettingsDeveloperSettings]`.
- **Non-UObject classes that hold UObjects** use `TStrongObjectPtr`, or `TWeakObjectPtr` when they intentionally don't own them.
- **Appliers:**
  - Each applier declares what it handles through `CanApply(Descriptor)`.
  - `Apply` must be idempotent, because it runs on every world BeginPlay.
  - Video appliers never call `UGameUserSettings::ApplySettings`. Use `ApplyNonResolutionSettings`, or the ApplyConfirm flow for resolution and window mode.
- **Setting names** use the form `Category.Name`, for example `Audio.MasterVolume` or `Video.FOV`.
- **Plugin Content** holds only shipping assets. Test assets live in the Sample project.

## Naming and style

- Always **`WUSettings...`** with uppercase WU, never `WuSettings`. Use the UE prefixes: `UWU`, `FWU`, `EWU`, `IWU`, `AWU`, plus `DA_WU` and `SC_` for assets.
- Every source file starts with `// Copyright (c) 2026 Wanderson Lopes. All rights reserved.`
- **Logging:**
  - Use only `LogWUSettings`, `LogWUSettingsAudio`, `LogWUSettingsVideo` and `LogWUSettingsUI`. Never use `LogTemp`. Add a new category per domain when needed.
  - `Error` means something is broken.
  - `Warning` means bad input was rejected.
  - `Log` means a normal lifecycle event.
- Mark getters `const`. Prefer early returns. Use `TObjectPtr` for `UPROPERTY` object members.
- Comments are in English and explain *why*, not *what*.

## Adding a new setting

1. Register an `FWUSettingDescriptor` in the matching `Defaults/WU<Category>Defaults.cpp`. Set Name, DisplayName, Description, Category, Type, ApplyStrategy, Min/Max/Step for numeric types, and DefaultValue. The DefaultValue type must match Type.
2. If no applier handles it, create `Appliers/<Category>/WU<Category>SettingApplier` and register it in `StartupModule`.
3. Test with `WU_SetSetting <Name> <Value>` in PIE: the value applies, persists after restarting PIE, and an out-of-range value is clamped.

## Commits

- One commit per completed phase, with an imperative summary line and a bullet body.
- Stage with `git add -u` plus explicit paths for new files. Never stage test assets.
