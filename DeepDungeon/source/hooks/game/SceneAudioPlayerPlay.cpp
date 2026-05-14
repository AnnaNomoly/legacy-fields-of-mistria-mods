#include "../../utils/Utils.h"

using namespace State::Player;
using namespace State::Floor;
using namespace State::Maps;

// TODO: Add new songs when updated
static const std::vector<std::string> MUSIC_INTERNAL_NAMES = { // As of 0.15.1
	"Music/Crystal Tracks/BarleyMoon",
	"Music/Crystal Tracks/CrystalCaves",
	"Music/Crystal Tracks/PinkTwintails",
	"Music/Events/DragonCutscene",
	"Music/Events/LightAndShadow",
	"Music/Events/SharedLight",
	"Music/Events/ShootingStarNight",
	"Music/Events/TheSeal",
	"Music/Events/ThemeOfDarkness",
	"Music/Events/TsukihotaruFinale",
	"Music/Location Tracks/AnimalFestival",
	"Music/Location Tracks/Bathhouse",
	"Music/Location Tracks/Blacksmith",
	"Music/Location Tracks/Carpenter",
	"Music/Location Tracks/Clinic",
	"Music/Location Tracks/Deep Woods",
	"Music/Location Tracks/Festival",
	"Music/Location Tracks/General Store",
	"Music/Location Tracks/HarvestFestivalTheme",
	"Music/Location Tracks/InnLessBusy",
	"Music/Location Tracks/InnMoreBusy",
	"Music/Location Tracks/MinesEntry",
	"Music/Location Tracks/Player Home/Day",
	"Music/Location Tracks/Player Home/Night",
	"Music/Location Tracks/SeridiaVoidRoom",
	"Music/Location Tracks/ShootingStarFestival",
	"Music/Location Tracks/SpringFestival",
	"Music/Location Tracks/TheFinalSeal",
	"Music/Location Tracks/VoidSeal",
	"Music/Menu Tracks/Tsukihotaru",
	"Music/Npc Tracks/Adeline",
	"Music/Npc Tracks/Balor",
	"Music/Npc Tracks/Caldarus",
	"Music/Npc Tracks/Celine",
	"Music/Npc Tracks/Eiland",
	"Music/Npc Tracks/Hayden",
	"Music/Npc Tracks/Juniper",
	"Music/Npc Tracks/March",
	"Music/Npc Tracks/Reina",
	"Music/Npc Tracks/Ryis",
	"Music/Npc Tracks/Seridia",
	"Music/Npc Tracks/Valen",
	"Music/Playlists/MinesDeepEarth",
	"Music/Playlists/MinesLavaCaves",
	"Music/Playlists/MinesTideCaverns",
	"Music/Playlists/MinesUpper",
	"Music/Playlists/MinesRuins",
	"Music/Playlists/Spring",
	"Music/Playlists/Spring Rain",
	"Music/Playlists/Summer",
	"Music/Playlists/Fall",
	"Music/Playlists/Winter",
	"Music/Playlists/Winter Snow"
};

// MMAPI Game::Hooks::BeforePlayAudio callback.
// Replaces DD's pre-migration latching of script_name_to_reference_map[SCENE_AUDIO_PLAYER_PLAY];
// MMAPI doesn't expose a SceneAudioPlayer context, but DD only used those refs to call back into
// the same script — that path is now MMAPI::Game::Hooks::BeforePlayAudio direct mutation.
void BeforePlayAudio(MMAPI::Game::PlayAudioContext& ctx)
{
	if (game_is_active && Config::config.randomize_dungeon_music && AriCurrentGmRoomIsDungeonFloor() && floor_number != 91)
	{
		static thread_local pcg32 random_generator([] {
			std::random_device rd;
			return pcg32(
				(static_cast<uint64_t>(rd()) << 32) | rd(),
				(static_cast<uint64_t>(rd()) << 32) | rd()
			);
		}());
		std::uniform_int_distribution<size_t> game_music_distribution(0, MUSIC_INTERNAL_NAMES.size() - 1);
		ctx.SetAudioName(MUSIC_INTERNAL_NAMES.at(game_music_distribution(random_generator)));
	}
}
