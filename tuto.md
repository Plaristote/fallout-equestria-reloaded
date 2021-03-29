# Getting started
## What do you need to make games using the FoE engine ?
You need:

- Tiled (http://mapeditor.org), used to create the tilemaps for the game levels,
- Fallout Equestria Development Release, a build of the engine that includes a game editor,
- Knowledge of modular JavaScript.

## What do you need to build FoE engine ?
We've made great efforts to stay minimalist in our requirements. As such, FoE engine fully relies on the Qt framework.

In order to build FoE engine, you will need CMake and Qt 5.15 (note that Qt 6 isn't supported yet).

# Scripting basics
## Contexts
There are three different context in which your scripts might interfere:

### Game
This is the global game manager. You can use it to store global variables, accessibles anywhere, anytime. It is also responsible for context changes: if you want the player to leave the current level, join another one, or go to the worldmap, the `game` object will make that magic happen for you.

To interact with that context, the `game` variable is globally available in all of your scripts, and can be interacted with at all times.

### WorldMap
The worldmap is what appears whenever the player is no longer in any level. Just as the game context, it is always available, even when it is inactive. Indeed, even if the player isn't on the worldmap, you may want to change it's world position, or lift some of the worldmap's fog, when the players acquire information about a new location, for instance.

The worldmap is always available as a property of `game`: use `game.worldmap` to interact with the worldmap.

### Level
Levels are where the player will spend most of their time. There can always be only one level loaded at a time, and when it is loaded, it is globally available through the `level` variable.
The level context is very rich and complex, and you may use it to do in your scripts everything you would be able to do in the Game Editor: adding objects, removing objects, moving them around. However, most of the time, you won't directly interact with the level itself, as you will mostly be working with DynamicObjects.

The level context is available whenever a level is active. Use the `level` global variable to interact with it.

## DynamicObjects, or the citizen of the game's world
DynamicObjects are what populates the world of this game engine. Pretty much anything that is scriptable and that can interact with the player is a DynamicObject: other characters, doorways, traps, or anything your creative mind may want to put in the game.

While it is perfectly possible to develop your own behaviour from a DynamicObject, some sub-types also exist to make this task easier in some cases. We will go over those right now:

### Specific types of DynamicObjects
#### InventoryItem
InventoryItems are objects that characters can pick up, drop, and optionally, use on other DynamicObjects. They all implement an `itemType`, defined in the Object Editor (from the Game Editor), defining basic properties, such as icon, weight, value...

The particularity of InventoryItems as DynamicObjects is that they can be extracted from the Level context, and stored in an Inventory. It is when they are inside a character's inventory that InventoryItems can deploy all their features: they can be used as equipment (armor, weapons, usable objects), or used on other objects.

InventoryItems can be scripted to implement certain behaviours when they are used on another DynamicObject, or directly from the inventory.

Some InventoryItems can be marked as offensive items, by setting the `triggersCombat` property to `true` in an InventoryItems. Such items will trigger combat when used on another character.

#### StorageObject
StorageObjects are regular objects with an Inventory attached to them. By default, interacting with a StorageObject will open the looting interface for the Player, allowing him to exchange items with his own Inventory.

StorageObjects can be scripted to implement specific behaviours when items are picked up or dropped in their inventory.

#### Doorway
Doorways are objects that may block access to parts of the tilemap, as long as they are closed. By default, interacting with a Doorway will try to open it, or close it. It is also possible to lock Doorways: characters then won't be able to open the door anymore, unless something unlocks it.

Locking and unlocking doors is done through scripting. You can checkout `scripts/behaviour/door.mjs` to see the basic behaviour for doors, implementing lockpicking and keys.

It is also possible to destroy doors, using the `bustOpen` method.

#### Character
Characters are the most compplex of DynamicObjects. They are DynamicObjects featuring a few more components:

- CMAP's StatModel, which is basically the character's character sheet.
- FieldOfView, which updates regularly to define which other characters are visible or not.
- Inventory, which works the same way as for StorageObjects.
- ActionQueue, a very important component that allows scripts to program series of actions for a character to take.
- TaskManager, a component that allows scripts to schedule delayed calls to themselves, using game-time (instead of real-world time).

On top of that, if a character's script define dialogs or text bubbles, it is possible for a player to use the "talk-to" interaction with them. The talk-to interaction will open a dialog UI, if the character has an available dialog. Otherwise, if text bubbles have been defined for the character, it will randomly pop one of those. If no dialog or text bubbles have been defined, the talk-to interaction won't pop up in the interaction menu.

## Cutie Mark Acquisition Program
The CMAP is the part of the Game Engine that handles character sheets, their statistics, experience, and appearence.

Through scripting, it is possible to implement plugins for the CMAP. Basic plugins can modify a character's base statistics when they are toggled on or off on a character sheet. There are several types of plugins for the CMAP:

### Traits
Traits are typically modifiers that  that the player picks on character creation, bringing him a balance of advantages and disadvantages.

### Perks
Perks are similar to traits. They are occasionally awarded to the player when he levels up, according to the perk rate.

### Races
Races are the most important plugins. While a Race plugin can also modify a character's base statistic, it is also used to determine the character's sprite sheet.

## Dialogs
While simple dialogs can be entirely implemented in the Game Editor, more specific behaviors can be implemented through scripting.

The dialog scripts allow you to generate your own answers or responses, to filter out the player's available responses, to change the dialog's mood (have the interlocutor frown, smile, and so on), all while still being able to interact with the rest of the game engine.

## Quests
Last but not least, you may write scripts for quests. The quest script defines a list of objectives to fulfill, and can then be called from other part of the game to set an objective as complete.

Quests scripts can also monitor any killed characters, and objects picked up by the player, as a way to allow a quest script to easily set its objectives as complete, without having to meddle with too much code unrelated to quests. For more complex objectives, you will have to trigger their completion through other scripts (such as a DynamicObject or a Dialog script).

# Scripting from the inside-out
## Folder structure
First, let's take a quick look at the folders and files you will be working with:

- `assets/tilemaps/`: the target folder for your Tiled tilemaps
- `assets/tilesets/`: the target folder for your Tiled tilesets
- `assets/locales/`: translations files are located there
- `assets/videos/`: root folder for video files
- `assets/audio/`: root folder for audio files
- `assets/audio.json`: maps audio files to nicknames
- `assets/musics.json`: set of playlists prepared for certain contexts (specific level, worldmap, main menu)
- `assets/sprites/`: root folder for spritesheets
- `assets/sprites.json`: library of animation and animations groups (should be edited using the Game Editor)
- `assets/objects.json`: library of available InventoryItems' item types.
- `assets/worldmap.png`: Worldmap source
- `scripts/cmap/traits`: CMAP traits plugins folder
- `scripts/cmap/races`: CMAP races plugins folder
- `scripts/cmap/perks`: CMAP perks plugins folder
- `scripts/quests`: Quest folder
- `scripts/dialogs`: Dialog folder
- `scripts/levels`: Level-context scripts (each level can implement one)
- `scripts/pnjs`: Characters scripts
- `scripts/items`: InventoryItems scripts
- `scripts/behaviour`: All other DynamicObjects scripts
- `scripts/main.mjs`: Game-context script
- `scripts/initialize.mjs`: A simple script that initializes the Game context once a new game starts.

## Initializing a new game
The first thing you want to do is to take control of the entry point of the game. You want the player to start at a specific place in the WorldMap, and possibly to start directly inside a Level.

To achieve such results, we will use the `scripts/initialize.mjs` file. It gets triggered once the players finishes character creation, allowing you to insert any sort of transition and set up the beginning of the game.

The `initialize.mjs` is a very simple script, and must only export a single `initialize` function, as such:

```
export function initialize() {
  game.onCityEntered("stable-cavern", "demo-begin");
  game.worldmap.setPosition(150, 250);
  game.transitionRequired("intro.mp4", 1);
}
```

This scripts does three things:

- The first line uses `game.onCityEntered` to load the`stable-cavern` level (corresponding to the tilemap stored in `assets/tilemaps/stable-cavern.json`). The second parameter `demo-begin` defines the zone in which to insert the player.

- The second lines instantly move the character to a specific position on the worldmap.

- The third lines starts a transition using `game.transitionRequired`. Transitions can be started at anytime during the game: the first paremeter specifies which video file to play (the path is relative to the `assets/videos` folder), and the second parameter defines how much game-world seconds should pass during the transition (in this case, only one second will pass in the game world while the player watches the video).
