# (Legacy) Fields of Mistria Mods

> [!CAUTION]
> This repository has been **deprecated and archived**. It contains code for the old "legacy" game engine. It will no longer receive updates. For current mods see my [fields-of-mistria-mods](https://github.com/AnnaNomoly/fields-of-mistria-mods) repository instead.

This repository is a collection of my mods for [Fields of Mistria](https://store.steampowered.com/app/2142790/Fields_of_Mistria/) that use [YYTK](https://github.com/AurieFramework/YYToolkit) and [Aurie](https://github.com/AurieFramework/Aurie) to dynamically inject code into GameMaker games built using the YoYo Compiler at runtime. For more information about these technologies see the [Frequently Asked Questions](#frequently-asked-questions) section.

## Mod Status
The table below will be updated with mod development to indicate the status of all Fields of Mistria mods I develop that use YYTK. For mods that *don't* use YYTK, see my [momi-mod-packages](https://github.com/AnnaNomoly/momi-mod-packages) repository instead. 

```
✅ = Actively Developed
🟡 = Maintence Mode
❌ = Unsupported
```
| Name | Mod Version | Status | Nexus Link |
| ---- | ----------- | ------------------- | ---------- |
| Animal Friends | 1.4.4 | ✅ | https://www.nexusmods.com/fieldsofmistria/mods/241 |
| Bug Radar | 1.0.1 | ✅ | https://www.nexusmods.com/fieldsofmistria/mods/510 |
| Charm Spell | 1.0.0 | ❌ | N/A |
| Chutes and Ladders | 1.4.0 | ✅ | https://www.nexusmods.com/fieldsofmistria/mods/153 |
| Cookbook | 1.1.6 | ✅ | https://www.nexusmods.com/fieldsofmistria/mods/162 |
| Curator | 1.1.1 | ✅ | https://www.nexusmods.com/fieldsofmistria/mods/337 |
| DataMining | 1.0.1 | 🟡 | N/A |
| Deep Dungeon | 0.8.1 | ✅ | https://www.nexusmods.com/fieldsofmistria/mods/523 |
| Directional Attacks | 1.0.1 | ✅ | https://www.nexusmods.com/fieldsofmistria/mods/248 |
| D.I.Y | 1.1.3 | ✅ | https://www.nexusmods.com/fieldsofmistria/mods/243 |
| Dynamic NPC Portraits | 1.0.1 | ✅ | https://www.nexusmods.com/fieldsofmistria/mods/535 |
| Dynamic Object Sprites | 1.0.0 | ✅ | https://www.nexusmods.com/fieldsofmistria/mods/545 |
| Elegance | 1.0.1 | ✅ | https://www.nexusmods.com/fieldsofmistria/mods/453 |
| Faux Sigils | 1.0.1 | ✅ | https://www.nexusmods.com/fieldsofmistria/mods/527 |
| Friday Night Reminder | 1.1.1 | ✅ | https://www.nexusmods.com/fieldsofmistria/mods/290 |
| Infinite Health | 1.0.2 | ✅ | https://www.nexusmods.com/fieldsofmistria/mods/83 |
| Magical Girl Ari | 1.0.1 | ✅ | https://www.nexusmods.com/fieldsofmistria/mods/516 |
| Mill Anywhere | 1.1.1 | ✅ | https://www.nexusmods.com/fieldsofmistria/mods/128 |
| Mistbloom | 1.0.4 | ✅ | https://www.nexusmods.com/fieldsofmistria/mods/348 |
| Mute Any Sound | 1.0.1 | ✅ | https://www.nexusmods.com/fieldsofmistria/mods/508 |
| Quake Spell | 1.2.0 | ✅ | https://www.nexusmods.com/fieldsofmistria/mods/155 |
| Remind Me | 1.0.0 | ✅ | https://www.nexusmods.com/fieldsofmistria/mods/529 |
| Sandbox Mode Crafting | 1.0.0 | ✅ | https://www.nexusmods.com/fieldsofmistria/mods/569 |
| Save Anywhere | 1.2.3 | ✅ | https://www.nexusmods.com/fieldsofmistria/mods/346 |
| Save Backup | 1.0.0 | ✅ | N/A |
| Secret Santa | 1.1.2 | ✅ | https://www.nexusmods.com/fieldsofmistria/mods/343 |
| Ship It | 1.1.1 | ✅ | https://www.nexusmods.com/fieldsofmistria/mods/342 |
| Spawn Any Item (Dig Up Anything) | 2.0.0 | ✅ | https://www.nexusmods.com/fieldsofmistria/mods/138 |
| Statue of Boons | 1.1.2 | ✅ | https://www.nexusmods.com/fieldsofmistria/mods/368 |
| Stop Time Indoors | 1.0.3 | ✅ | https://www.nexusmods.com/fieldsofmistria/mods/507 |
| Summoning Circle | 1.0.3 | ✅ | https://www.nexusmods.com/fieldsofmistria/mods/451 |
| Telepop | 2.2.0 | ✅ | https://www.nexusmods.com/fieldsofmistria/mods/108 |
| The Aldarian Economy | 1.0.0 | ❌ | N/A |
| The Perfect Gift | 1.1.7 | ✅ | https://www.nexusmods.com/fieldsofmistria/mods/244 |
| Utility Sword | 1.0.0 | ✅ | https://www.nexusmods.com/fieldsofmistria/mods/517 |
| Wardrobe | 1.1.2 | ✅ | https://www.nexusmods.com/fieldsofmistria/mods/206 |
| Work Hard | 1.0.0 | ❌ | N/A |
| Zoom Control | 1.0.0 | ❌ | N/A |
| Zoomies | 1.0.0 | 🟡 | N/A |

## Frequently Asked Questions

### What is Aurie?
The [Aurie Framework](https://github.com/AurieFramework/Aurie) developed by [Archie](https://github.com/Archie-osu) is a suite of utilities designed to support arbitrary code execution before a target executable's entrypoint. The framework was specifically tailored to support [YYToolkit](https://github.com/AurieFramework/YYToolkit) (YYTK) by acting as the *delivery mechanism* for GameMaker mods. Aurie accomplishes this by using the `AurieLoader.exe` (AurieLoader) executable, which is capable of injecting code _before the game launches_, to insert the YYTK modules into the game.

However, AurieLoader *itself* must run before the game is started, in order for the code injection to occur. For Fields of Mistria mods, this is accomplished automatically by the [Mods of Mistria Installer](https://github.com/Garethp/Mods-of-Mistria-Installer) (MOMI) when installing mods that use Aurie and YYTK. MOMI uses the Windows Registry in order to set the [Image File Execution Options](https://learn.microsoft.com/en-us/previous-versions/windows/desktop/xperf/image-file-execution-options) (IFEO) keys for the game executable. By doing so, it ultimately instructs Windows to run AurieLoader before the game.

> ⚠️ **False Positive Warning**: Due to how Aurie modifies the target game executable as mentioned above, it is often flagged by Anti-Virus software as a potential threat. For Fields of Mistria modding, it is often recommended to add your entire game directory as an *excluded folder* in Windows Defender or your active Anti-Virus software.

### What is YYTK?
The [YYToolkit](https://github.com/AurieFramework/YYToolkit) (YYTK) developed by [Archie](https://github.com/Archie-osu) is a library used for creating C++ applications (mods) capable of modifying GameMaker games built using the YoYo Compiler at runtime. It is the foundation for all "YYTK" mods developed for Fields of Mistria, providing the mechanisms necessary to interact with the game. All mods included in this repository use YYTK in order to modify Fields of Mistria code at runtime.

### What is MOMI?
The [Mods of Mistria Installer](https://github.com/Garethp/Mods-of-Mistria-Installer) (MOMI) developed by [Gareth](https://github.com/Garethp) is a mod manager for Fields of Mistria. It is capable of installing and uninstalling mods developed for it, and is even capable of automatically installing Aurie along with YYTK mods! All mods included in this repository are designed for MOMI, though their actual "mod packages" are in a separate repository of mine: [momi-mod-packages](https://github.com/AnnaNomoly/momi-mod-packages).

## Support
- Help with YYToolkit or Aurie Framework - [Arch Wizards Server](https://discord.gg/vbT8Ed4cpq)
- Help with Fields of Mistria Modding - [Unofficial Fields of Mistria Server](https://discord.gg/XpCbbGRWTY)

## Modding Developer Resources
- **Aurie Framework & YYToolkit**
  - Developed by: [Archie](https://github.com/Archie-osu)
  - GitHub link: https://github.com/AurieFramework/Aurie
- **Mods of Mistria Installer**
  - Developed by: [Gareth](https://github.com/Garethp)
  - GitHub link: https://github.com/Garethp/Mods-of-Mistria-Installer
