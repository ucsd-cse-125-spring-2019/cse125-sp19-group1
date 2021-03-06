#include "InGameGraphicsEngine.h"

#include "../../network/ClientGame.h"

#include "ItemModelType.h"

#include "Transform.h"
#include "Geometry.h"
#include "DirLight.h"
#include "FogGenerator.h"
#include "FBXObject.h"
#include "UIObject.h"
#include "ParticleSpawner.h"
#include "Skybox.h"

#include "TwoDeeGraphicsEngine.h"  // for createObjectForTexture()

#include <thread>
#include <unordered_map>

#define RACCOON_WALK_MDL_PATH   (ANIMATIONS_PATH "raccoonNewWalk.dae")
#define RACCOON_IDLE_MDL_PATH	(ANIMATIONS_PATH "raccoonIdle.dae")
#define RACCOON_CARRY_MDL_PATH  (ANIMATIONS_PATH "raccoonNewWalkWithItem.dae")
#define RACCOON_IDLE_CARRY_MDL_PATH (ANIMATIONS_PATH "raccoonIdleWithItem.dae")
#define RACCOON_SEARCH_MDL_PATH (ANIMATIONS_PATH "raccoonSearch.dae")
#define RACCOON_MDL_PATH        (MODELS_PATH "raccoon.fbx")
#define RACCOON_TEX_PATH        (TEXTURES_PATH "raccoon.ppm")

#define CAT_WALK_MDL_PATH       (ANIMATIONS_PATH "catWalk.dae")
#define CAT_IDLE_MDL_PATH       (ANIMATIONS_PATH "catIdle.dae")
#define CAT_CARRY_MDL_PATH      (ANIMATIONS_PATH "catWalkWithItem.dae")
#define CAT_IDLE_CARRY_MDL_PATH	(ANIMATIONS_PATH "catIdleWithItem.dae")
#define CAT_SEARCH_MDL_PATH     (ANIMATIONS_PATH "catSearch.dae")
#define CAT_MDL_PATH            (MODELS_PATH "cat.fbx")
#define CAT_TEX_PATH            (TEXTURES_PATH "cat.ppm")

#define DOG_WALK_MDL_PATH       (ANIMATIONS_PATH "dogWalk.dae")
#define DOG_IDLE_MDL_PATH		(ANIMATIONS_PATH "dogIdle.dae")
#define DOG_CARRY_MDL_PATH      (ANIMATIONS_PATH "dogWalkWithItem.dae")
#define DOG_IDLE_CARRY_MDL_PATH	(ANIMATIONS_PATH "dogIdleWithItem.dae")
#define DOG_SEARCH_MDL_PATH     (ANIMATIONS_PATH "dogSearch.dae")
#define DOG_MDL_PATH            (MODELS_PATH "doggo.fbx")
#define DOG_TEX_PATH            (TEXTURES_PATH "doggo.ppm")

#define CHEF_WALK_PATH    (ANIMATIONS_PATH "chefWalk.dae")
#define CHEF_IDLE_PATH	  (ANIMATIONS_PATH "chefIdle.dae")
#define CHEF_WITH_NET_PATH (MODELS_PATH "chefSwing.fbx")
#define CHEF_MDL_PATH     (MODELS_PATH "chef.fbx")
#define CHEF_TEX_PATH     (TEXTURES_PATH "chef.ppm")
#define NET_MDL_PATH      (MODELS_PATH "net.fbx")
#define NET_TEX_PATH      (TEXTURES_PATH "net.png")

#define TILE_MDL_PATH     (MODELS_PATH "tile.fbx")
#define TILE_TEX_PATH     (TEXTURES_PATH "tile.png")

#define SAND_TEX_PATH     (TEXTURES_PATH "Sand_001_COLOR.png")

#define PLAYER_MDL_CLIP_RADIUS 60.f
#define ITEM_MDL_CLIP_RADIUS 60.f

#define CHEF_FOG_DISTANCE 85.0f
#define RACCOON_FOG_DISTANCE 75
#define WALL_MDL_PATH     (MODELS_PATH "wall.fbx")
#define WALL_TEX_PATH     (TEXTURES_PATH "wall.png")

#define BOX_SEARCH_MDL_PATH     (ANIMATIONS_PATH "box_search.dae")

#define CANVAS_PATH      "../Canvas/"
#define CANVAS_MDL_PATH  (CANVAS_PATH "canvas2.fbx")

//particle effects
#define DUST_PARTICLE_TEX (PARTICLES_PATH "dust.png")
#define FLASH_PARTICLE_TEX (PARTICLES_PATH "flash.png")
#define SPEED_PARTICLE_TEX (PARTICLES_PATH "speed.png")
#define SLOW_PARTICLE_TEX (PARTICLES_PATH "slow.png")
#define BUILD_PARTICLE_TEX (PARTICLES_PATH "build.png")
#define BLIND_PARTICLE_TEX (PARTICLES_PATH "blind.png")
#define SEARCH_PARTICLE_TEX (PARTICLES_PATH "search.png")
#define BEAR_PARTICLE_TEX (PARTICLES_PATH "bearFull.png")

#define OBJ_VERT_SHADER_PATH "./obj_shader.vert"
#define OBJ_FRAG_SHADER_PATH "./obj_shader.frag"

#define UI_VERT_SHADER_PATH "./ui_shader.vert"
#define UI_FRAG_SHADER_PATH "./ui_shader.frag"

#define PARTICLE_VERT_SHADER_PATH "./Particle.vertexshader"
#define PARTICLE_FRAG_SHADER_PATH "./Particle.fragmentshader"

// Paths for sounds
#define SOUNDS_PATH			"../../sounds/"
#define SOUNDS_DOOR			(SOUNDS_PATH "frontexit_door_short.mp3")
#define SOUNDS_DOOR_UNLOCK	(SOUNDS_PATH "frontexit_unlock_short.mp3")
#define SOUNDS_FOUND_ITEM	(SOUNDS_PATH "found_item_short.mp3")
#define SOUNDS_NET			(SOUNDS_PATH "pop.mp3")	// FIXME placeholder for swoosh
#define SOUNDS_RACCOON_UP	(SOUNDS_PATH "RaccoonDownUp.mp3")
#define SOUNDS_RACCOON_DOWN	(SOUNDS_PATH "RaccoonUpDown.mp3")
#define SOUNDS_SEARCH_ITEM	(SOUNDS_PATH "search_item.mp3")
#define SOUNDS_TOILET		(SOUNDS_PATH "bathroom_toilet.mp3")
#define SOUNDS_VENT_SCREW	(SOUNDS_PATH "ventexit_screw.mp3")
#define SOUNDS_WINDOW		(SOUNDS_PATH "bathroom_window.mp3")
#define SOUNDS_YAY			(SOUNDS_PATH "Yay.mp3")

#define MAX_PLAYERS 4

// Uncomment to render a repeating pattern of all environment objects
// This is good for debugging scale/positioning/rendering
// #define ENV_OBJS_DEMO

#define TILE_HEIGHT_ADJUST -2.f
#define TILE_SCALE 10.f          /* overall scale of the entire floor. (TILE_SCALE * TILE_STRIDE) should match server tile size, which is currently 20 */
#define TILE_LEVEL_OFFSET 1.0f   /* from first floor to second */
#define TILE_STRIDE 2.0f         /* difference in position from one tile to the next */
#define TILE_SIZE_SERVER 20.f

// Uncomment to render an additional dummy Chef that mirrors the player's movements
// #define DUMMY_ID -4000

// Uncomment to force the camera to any other player
// #define DEBUG_CAUGHT

// Uncomment to force carrying
// #define DEBUG_CARRY

// Uncomment to skip loading player animations
// #define DEBUG_LOAD_LESS

// Uncomment to skip loading UI
// #define DEBUG_NO_UI

// Uncomment to make gates always animate so that you don't have to play through
//#define DEBUG_GATE_ANIMATION

// Set to 0 to disable gate animation
#define ENABLE_GATE_ANIMATION 1

#ifdef DEBUG_CARRY
unsigned fake_carried_idx = 1;
#endif

#define DELETE_IF_NEEDED(x) \
	if (x) {\
		delete x;\
		x = nullptr;\
	}

#define CHEF_IDX     (static_cast<unsigned>(ModelType::CHEF))
#define RACCOON_IDX  (static_cast<unsigned>(ModelType::RACOON))
#define CAT_IDX      (static_cast<unsigned>(ModelType::CAT))
#define DOG_IDX      (static_cast<unsigned>(ModelType::DOG))

#define MINIMAP_MARGIN_LEFT 0.02f
#define MINIMAP_MARGIN_BOTTOM ((windowWidth / (float)windowHeight) * 0.02f)
#define MINIMAP_SCREEN_WIDTH 0.275f
#define MINIMAP_WALL_WIDTH (TILE_SIZE_SERVER * 0.2f)
#define MINIMAP_ALPHA 1.f
#define MINIMAP_BG_COLOR 0.5f, 0.5f, 0.5f, 0.525f
#define MINIMAP_WALL_COLOR 0.f, 0.f, 0.f, 1.f
#define MINIMAP_GATE_COLOR 1.f, 1.f, 1.f, 0.5f
#define MINIMAP_JAIL_COLOR 1.f, 0.5f, 0.5f, 0.5f
#define MINIMAP_PING_DURATION 2.25
#define MINIMAP_PING_FINAL_RADIUS (9.f * TILE_SIZE_SERVER)
#define MINIMAP_PING_BEGIN_RADIUS (0.475f * TILE_SIZE_SERVER)
#define MINIMAP_JAIL_PING_INTERVAL 2.5
#define MINIMAP_ANIMAL_PING_INTERVAL 20.0
#define MINIMAP_ANIMAL_ME_DOT_COLOR      0.3f,  0.6f, 0.95f,   1.f  /* rgba */
#define MINIMAP_ANIMAL_OTHER_DOT_COLOR  0.25f, 0.45f, 0.75f, 0.85f  /* rgba */
#define MINIMAP_CHEF_DOT_COLOR          0.95f,  0.4f,  0.2f,   1.f  /* rgba */
#define MINIMAP_ANIMAL_PING_COLOR       0.25f, 0.45f, 0.75f  /* rgb, no alpha */
#define MINIMAP_JAIL_PING_COLOR         0.95f,  0.4f,  0.2f  /* rgb, no alpha */
#define MINIMAP_CHEF_PING_COLOR         0.95f,  0.4f,  0.2f  /* rgb, no alpha */

struct MapPing {
	glm::vec3 position;
	glm::vec3 fillColor;
	double startTime;
	double interval;

	MapPing() {}

	MapPing(glm::vec3 newPosition, glm::vec3 newFillColor, double newInterval = MINIMAP_ANIMAL_PING_INTERVAL)
		: position(newPosition.x, newPosition.z, 0.f), fillColor(newFillColor), interval(newInterval) {
		startTime = glfwGetTime();
	}
};
unordered_map<int, MapPing> allPings;

#define TABLE_HEIGHT 9.25f
#define TO_TILE_COORD(x) (int)(x / TILE_SIZE_SERVER)
#define TO_TILE_COORDS(x, y) TO_TILE_COORD(x), TO_TILE_COORD(y)
static const struct ExtraObjects {
	int x;
	int z;
	ItemModelType type;
	const char *texture;
} extraObjects[] = {
	// Paintings
	{13, 1, ItemModelType::painting, TEXTURES_PATH "painting.png"},
	{0, 4, ItemModelType::painting, TEXTURES_PATH "paintingathena.png"},
	{19, 7, ItemModelType::painting, TEXTURES_PATH "paintingedward.png"},
	{2, 12, ItemModelType::painting, TEXTURES_PATH "paintingoliver.png"},
	{23, 12, ItemModelType::painting, TEXTURES_PATH "paintingvoelker1.png"},
	{TO_TILE_COORDS(47.619, 227.9), ItemModelType::painting, TEXTURES_PATH "paintingvoelker2.png"},
	{TO_TILE_COORDS(365.061, 115.715), ItemModelType::painting, TEXTURES_PATH "paintingHung.png"},
	{TO_TILE_COORDS(470.262, 111.56), ItemModelType::painting, TEXTURES_PATH "paintingSterling.png"},

	/*// Trash bags
	{TO_TILE_COORDS(7.79725, 249.298), ItemModelType::garbageBag, nullptr},
	{TO_TILE_COORDS(225.56, 310.885), ItemModelType::garbageBag, nullptr},
	{TO_TILE_COORDS(471.777, 152.69), ItemModelType::garbageBag, nullptr},*/
};

#define TO_TILE_CENTER(x, offset) (TO_TILE_COORD(x) + 0.5f + offset) * TILE_SIZE_SERVER
static const struct CustomPositionedObjects {
	float x;
	float y;
	float z;
	ItemModelType type;
} customPositionedObjects[] = {
	{TO_TILE_CENTER(208.88, 0.f), TILE_HEIGHT + TABLE_HEIGHT, TO_TILE_CENTER(111.23, 0.f), ItemModelType::plate},
	{TO_TILE_CENTER(208.88, -0.25f), TILE_HEIGHT + TABLE_HEIGHT, TO_TILE_CENTER(111.23, 0.f), ItemModelType::fork},
	{TO_TILE_CENTER(208.88, 0.25f), TILE_HEIGHT + TABLE_HEIGHT, TO_TILE_CENTER(111.23, 0.f), ItemModelType::knife},
	
	{80, 0, -100, ItemModelType::warrenBear},
	
	// Trash bags
	{TO_TILE_CENTER(-10, 0), 0, TO_TILE_CENTER(249.298, 0), ItemModelType::garbageBag},
	//{TO_TILE_CENTER(225.56, 0), 0, TO_TILE_CENTER(310.885, 0), ItemModelType::garbageBag},
	{TO_TILE_CENTER(491.777, 0), 0, TO_TILE_CENTER(152.69, 0), ItemModelType::garbageBag},
};

static const glm::mat4 identityMat(1.f);
static glm::mat4 orthoP;

static const struct PlayerModelSettings {
	const char *walkModelPath;     // filesystem path to a model geometry file
	const char *walkTexturePath;   // filesystem path to a texture file
	float walkAnimMultiplier;		   // multiply each keyframe by this amount to modify animation speed
	const char *idleModelPath;
	const char *idleTexturePath;
	float idleAnimMultiplier;
	const char *carryModelPath;    // filesystem path to a model geometry file
	const char *carryTexturePath;  // filesystem path to a texture file
	float carryAnimMultiplier;
	const char *idleCarryModelPath;
	const char *idleCarryTexturePath;
	float idleCarryAnimMultiplier;
	const char *actionModelPath;   // filesystem path to a model geometry file
	const char *actionTexturePath; // filesystem path to a texture file
	float actionAnimMultiplier;
	const char *title;             // name for use in debug messages, maybe user-visible too
	const char *name;              // name for use in debug messages, maybe user-visible too
	ModelType modelType;           // A unique ID, like ModelType::CHEF
	bool attachSkel;               // true if animated with a skeleton
	float scale;                   // scale adjustment
	glm::vec3 translate;           // position adjustment
	glm::vec3 carryPosition;       // where carried items should be drawn

	inline const char * getWalkModelPath() const {
		return walkModelPath;
	}

	inline const char * getIdleModelPath() const {
		return idleModelPath ? idleModelPath : walkModelPath;
	}

	inline const char * getCarryModelPath() const {
		return carryModelPath ? carryModelPath : walkModelPath;
	}

	inline const char * getIdleCarryModelPath() const {
		return idleCarryModelPath ? idleCarryModelPath : walkModelPath;
	}

	inline const char * getActionModelPath() const {
		return actionModelPath ? actionModelPath : walkModelPath;
	}

	inline const char * getwalkTexturePath() const {
		return walkTexturePath;
	}

	inline const char * getIdleTexturePath() const {
		return idleTexturePath ? idleTexturePath : walkTexturePath;
	}

	inline const char * getCarryTexturePath() const {
		return carryTexturePath ? carryTexturePath : walkTexturePath;
	}

	inline const char * getIdleCarryTexturePath() const {
		return idleCarryTexturePath ? idleCarryTexturePath : walkTexturePath;
	}

	inline const char * getActionTexturePath() const {
		return actionTexturePath ? actionTexturePath : walkTexturePath;
	}
} playerModelSettings[] = {
#ifndef DEBUG_LOAD_LESS
	// walkModelPath          walkTexturePath    walkAnimMultiplier 	idleModelPath			idleTexturePath		idleAnimMultiplier	carryModelPath           carryTexturePath  carryAnimMultiplier	idleCarryModelPath				idleCarryTexturePath	idleCarryAnimMultiplier	  actionModelPath           actionTexturePath  actionAnimMultiplier  title       name          modelType        attachSkel scale   translate                     carryPosition
	{ CHEF_WALK_PATH,         CHEF_TEX_PATH,     0.5f,                  CHEF_IDLE_PATH,			 nullptr,		    1.0f,		        nullptr,                 nullptr,          1.0f,			    CHEF_IDLE_PATH,					nullptr,			    1.0f,					  CHEF_WITH_NET_PATH,       nullptr,           1.0f,                 "Chef",     "Cheoffrey",  ModelType::CHEF,    true,   1.f,    glm::vec3(0.f),               glm::vec3(0.f, 0.f, 0.f) },
	{ RACCOON_WALK_MDL_PATH,  RACCOON_TEX_PATH,  0.5f,                  RACCOON_IDLE_MDL_PATH,	 nullptr,		    1.0f,		        RACCOON_CARRY_MDL_PATH,  nullptr,          1.0f,				RACCOON_IDLE_CARRY_MDL_PATH,	nullptr,			    1.0f,					  RACCOON_SEARCH_MDL_PATH,  nullptr,           1.0f,                 "Raccoon",  "Hung",       ModelType::RACOON,  true,   0.5f,   glm::vec3(0.f, 0.f,-1.2f),    glm::vec3(0.f, 5.5f, 3.75f) },
	{ CAT_WALK_MDL_PATH,      CAT_TEX_PATH,      0.5f,                  CAT_IDLE_MDL_PATH,		 nullptr,		    1.0f,		        CAT_CARRY_MDL_PATH,      nullptr,          1.0f,				CAT_IDLE_CARRY_MDL_PATH,		nullptr,			    1.0f,					  CAT_SEARCH_MDL_PATH,      nullptr,           1.0f,                 "Cat",      "Kate",       ModelType::CAT,     true,   1.f,    glm::vec3(0.f),               glm::vec3(0.f, 5.5f, 3.5f) },
	{ DOG_WALK_MDL_PATH,      DOG_TEX_PATH,      0.5f,                  DOG_IDLE_MDL_PATH,		 nullptr,		    1.0f,		        DOG_CARRY_MDL_PATH,      nullptr,          1.0f,				DOG_IDLE_CARRY_MDL_PATH,		nullptr,			    1.0f,					  DOG_SEARCH_MDL_PATH,      nullptr,           1.0f,                 "Dog",      "Richard",    ModelType::DOG,     true,   1.f,    glm::vec3(0.f),               glm::vec3(0.f, 5.5f, 3.5f) },
#else
	// walkModelPath          walkTexturePath    walkAnimMultiplier		idleModelPath			idleTexturePath		idleAnimMultiplier	carryModelPath           carryTexturePath  carryAnimMultiplier	idleCarryModelPath				idleCarrTexturePath		idleCarryAnimMultiplier   actionModelPath           actionTexturePath  actionAnimMultiplier  title       name          modelType        attachSkel scale   translate					 carryPosition
	{ CHEF_WALK_PATH,         CHEF_TEX_PATH,     0.5f,					nullptr,				nullptr,		    1.0f,			    nullptr,                 nullptr,          1.0f,				nullptr,						nullptr,				1.0f,					  nullptr,                  nullptr,           1.0f,                 "Chef",     "Cheoffrey",  ModelType::CHEF,    true,   1.f,	   glm::vec3(0.f),				 glm::vec3(0.f, 5.5f, 3.5f) },
	{ RACCOON_WALK_MDL_PATH,  RACCOON_TEX_PATH,  0.5f,					nullptr,				nullptr,		    1.0f,			    nullptr,				 nullptr,          1.0f,				nullptr,						nullptr,				1.0f,					  nullptr,				    nullptr,           1.0f,                 "Raccoon",  "Hung",       ModelType::RACOON,  true,   0.5f,   glm::vec3(0.f, 0.f, -1.2f),	 glm::vec3(0.f, 5.5f, 3.75f) },
	{ CAT_WALK_MDL_PATH,      CAT_TEX_PATH,      0.5f,					nullptr,				nullptr,		    1.0f,			    nullptr,				 nullptr,          1.0f,				nullptr,						nullptr,				1.0f,					  nullptr,				    nullptr,           1.0f,                 "Cat",      "Kate",       ModelType::CAT,     true,   1.f,    glm::vec3(0.f),				 glm::vec3(0.f, 5.5f, 3.5f) },
	{ DOG_WALK_MDL_PATH,      DOG_TEX_PATH,      0.5f,					nullptr,				nullptr,		    1.0f,			    nullptr,				 nullptr,          1.0f,				nullptr,						nullptr,				1.0f,					  nullptr,			        nullptr,           1.0f,                 "Dog",      "Richard",    ModelType::DOG,     true,   1.f,    glm::vec3(0.f),			     glm::vec3(0.f, 5.5f, 3.5f) },
#endif
};

#define ANIM_AND_TEX(m, t) ANIMATIONS_PATH m ".dae", TEXTURES_PATH t
#define MDL_AND_TEX(m, t) MODELS_PATH m ".fbx", TEXTURES_PATH t ".png"
#define MDL_SAME_TEX(x) MDL_AND_TEX(x, x)
#define GLM_H_PI glm::half_pi<float>()
#define GLM_PI glm::pi<float>()

#if ENABLE_GATE_ANIMATION
#define DOOR_FILENAMES ANIM_AND_TEX("betterdoor_build", "betterdoor.ppm")
#define VENT_FILENAMES ANIM_AND_TEX("vent_build", "vent.png")
#define WINDOW_FILENAMES ANIM_AND_TEX("window_build", "window.png")
#define WINDOW_SCALE glm::vec3(11.904f, 6.626f, 0.92f)
#else
#define DOOR_FILENAMES MDL_SAME_TEX("door")
#define VENT_FILENAMES MDL_SAME_TEX("vent")
#define WINDOW_FILENAMES MDL_SAME_TEX("window")
#define WINDOW_SCALE glm::vec3(1.f)
#endif

bool WINDOW_SCALED_UP = true;

static const struct ItemModelSettings {
	const char *modelPath;       // filesystem path to a model geometry file
	const char *texturePath;     // filesystem path to a texture file
	const char *name;            // name for use in debug messages, maybe user-visible too
	ItemModelType id;            // A unique ID, like ItemModelType::apple
	glm::vec3 scale;             // scale adjustment
	glm::vec3 translate;         // position adjustment
	glm::vec3 rotation;          // rotation
	bool wallRotate;             // auto-rotate away from any wall on this tile
	glm::vec3 carryTranslate;    // translation while being carried
	glm::vec3 carryRotate;       // rotation while being carried
} itemModelSettings[] = {
	// model and texture paths                             name                    id                               scale                translate                      rotate                            wallRotate  carryTranslate               carryRotate
	{ MDL_SAME_TEX("apple"),                               "apple",                ItemModelType::apple,             glm::vec3(1.f),     glm::vec3(0.f),                glm::vec3(0.f),                   false,      glm::vec3(0.f, 0.f, 1.f) },
	{ MDL_AND_TEX("banana", "bananagreen"),                "green banana",         ItemModelType::bananaGreen,       glm::vec3(1.f),     glm::vec3(0.f),                glm::vec3(0.f),                   false,      glm::vec3(0.f, 0.75f, 0.f) },
	{ MDL_AND_TEX("banana", "bananaperfect"),              "perfect banana",       ItemModelType::bananaPerfect,     glm::vec3(1.f),     glm::vec3(0.f),                glm::vec3(0.f),                   false,      glm::vec3(0.f, 0.75f, 0.f) },
	{ MDL_AND_TEX("banana", "bananaveryveryripe"),         "very ripe banana",     ItemModelType::bananaVeryRipe,    glm::vec3(1.f),     glm::vec3(0.f),                glm::vec3(0.f),                   false,      glm::vec3(0.f, 0.75f, 0.f) },
	{ MDL_SAME_TEX("box"),                                 "box",                  ItemModelType::box,               glm::vec3(1.5f),    glm::vec3(0.f),                glm::vec3(0.f),                   false,      glm::vec3(0.f, 1.f, 2.5f) },
	{ MDL_SAME_TEX("cake"),                                "cake",                 ItemModelType::cake,              glm::vec3(0.6f),    glm::vec3(0.f),                glm::vec3(0.f, GLM_H_PI, 0.f),    false,      glm::vec3(0.f, 1.f, 2.75f) },
	{ MDL_SAME_TEX("cookingpot"),                          "cooking pot",          ItemModelType::cookingPot,        glm::vec3(1.f),     glm::vec3(0.f),                glm::vec3(0.f),                   true,       glm::vec3(0.f, 1.f, 2.5f) },
	{ DOOR_FILENAMES,                                      "door",                 ItemModelType::door,              glm::vec3(1.f),     glm::vec3(0.f, 0.0f, -0.45f),  glm::vec3(0.f),                   true },
	{ MDL_SAME_TEX("fork"),                                "fork",                 ItemModelType::fork,              glm::vec3(1.f),     glm::vec3(0.05f, 0.0f, 0.f),   glm::vec3(0.f, GLM_PI, 0.f),      true },
	{ MDL_SAME_TEX("garbagebag"),                          "garbage bag",          ItemModelType::garbageBag,        glm::vec3(1.f),     glm::vec3(0.f),                glm::vec3(0.f),                   true,       glm::vec3(0.f, 1.f, 2.5f) },
	{ MDL_SAME_TEX("jail"),                                "jail",                 ItemModelType::jail,              glm::vec3(0.3f),    glm::vec3(0.f),                glm::vec3(0.f),                   false },
	{ MDL_AND_TEX("key", "key1"),                          "key #1",               ItemModelType::key1,              glm::vec3(0.5f),    glm::vec3(0.f),                glm::vec3(GLM_H_PI, 0.f, 0.f),    false,      glm::vec3(1.f, -0.5f, 0.5f),   glm::vec3(-GLM_H_PI, 0.f, 0.f) },
	{ MDL_AND_TEX("key", "key2"),                          "key #2",               ItemModelType::key2,              glm::vec3(0.5f),    glm::vec3(0.f),                glm::vec3(GLM_H_PI, 0.f, 0.f),    false,      glm::vec3(1.f, -0.5f, 0.5f),   glm::vec3(-GLM_H_PI, 0.f, 0.f) },
	{ MDL_AND_TEX("key", "key3"),                          "key #3",               ItemModelType::key3,              glm::vec3(0.5f),    glm::vec3(0.f),                glm::vec3(GLM_H_PI, 0.f, 0.f),    false,      glm::vec3(1.f, -0.5f, 0.5f),   glm::vec3(-GLM_H_PI, 0.f, 0.f) },
	{ MDL_AND_TEX("keydrop", "keydrop_bathroom"),          "bathroom key drop",    ItemModelType::keyDropBathroom,   glm::vec3(2.5f),    glm::vec3(0.f, 0.0f, -0.375f), glm::vec3(0.f, -GLM_H_PI, 0.f),   true },
	{ MDL_AND_TEX("keydrop", "keydrop_frontexit"),         "front exit key drop",  ItemModelType::keyDropFrontExit,  glm::vec3(2.5f),    glm::vec3(0.f, 0.0f, -0.375f), glm::vec3(0.f, -GLM_H_PI, 0.f),   true },
	{ MDL_AND_TEX("keydrop", "keydrop_vent"),              "vent key drop",        ItemModelType::keyDropVent,       glm::vec3(2.5f),    glm::vec3(0.f, 0.0f, -0.375f), glm::vec3(0.f, -GLM_H_PI, 0.f),   true },
	{ MDL_SAME_TEX("knife"),                               "knife",                ItemModelType::knife,             glm::vec3(1.f),     glm::vec3(-0.05f, 0.0f, 0.f),  glm::vec3(0.f, GLM_PI, 0.f),      true },
	{ MDL_SAME_TEX("orange"),                              "orange fruit",         ItemModelType::orange,            glm::vec3(1.f),     glm::vec3(0.f),                glm::vec3(0.f),                   false,      glm::vec3(0.f, 0.f, 1.f) },
	{ MDL_AND_TEX("painting", "paintingbear"),             "bear painting",        ItemModelType::painting,          glm::vec3(1.8f),    glm::vec3(0.f, 0.5f, -0.4f),   glm::vec3(0.f),                   true,       glm::vec3(0.f, 0.f, 2.f) },
	{ MDL_SAME_TEX("pear"),                                "pear",                 ItemModelType::pear,              glm::vec3(1.f),     glm::vec3(0.f),                glm::vec3(0.f),                   false,      glm::vec3(0.f, 0.f, 1.f) },
	{ MDL_SAME_TEX("plate"),                               "plate",                ItemModelType::plate,             glm::vec3(1.f),     glm::vec3(0.f),                glm::vec3(0.f),                   true,       glm::vec3(0.f, 0.1f, 2.f) },
	{ MDL_SAME_TEX("plunger"),                             "plunger",              ItemModelType::plunger,           glm::vec3(0.7f),    glm::vec3(0.f),                glm::vec3(0.f),                   false,      glm::vec3(0.f, 0.f, 1.f) },
	{ MDL_SAME_TEX("restaurantchair"),                     "restaurant chair",     ItemModelType::restaurantChair,   glm::vec3(0.7f),    glm::vec3(0.f),                glm::vec3(0.f),                   true,       glm::vec3(0.f, 1.f, 2.75f) },
	{ MDL_SAME_TEX("rope"),                                "rope",                 ItemModelType::rope,              glm::vec3(1.5f),    glm::vec3(0.f),                glm::vec3(0.f),                   true,       glm::vec3(-0.5f, 0.f, 0.f),    glm::vec3(GLM_H_PI, 0.f, 0.f) },
	{ MDL_AND_TEX("screwdriver", "screwdriver1"),          "screwdriver #1",       ItemModelType::screwdriver1,      glm::vec3(0.225f),  glm::vec3(0.f),                glm::vec3(0.f),                   false,      glm::vec3(-1.f, 0.25f, 0.3f), glm::vec3(0.f, GLM_H_PI, 0.f) },
	{ MDL_AND_TEX("screwdriver", "screwdriver2"),          "screwdriver #2",       ItemModelType::screwdriver2,      glm::vec3(0.225f),  glm::vec3(0.f),                glm::vec3(0.f),                   false,      glm::vec3(-1.f, 0.25f, 0.3f), glm::vec3(0.f, GLM_H_PI, 0.f) },
	{ MDL_AND_TEX("screwdriver", "screwdriver3"),          "screwdriver #3",       ItemModelType::screwdriver3,      glm::vec3(0.225f),  glm::vec3(0.f),                glm::vec3(0.f),                   false,      glm::vec3(-1.f, 0.25f, 0.3f), glm::vec3(0.f, GLM_H_PI, 0.f) },
	{ MDL_SAME_TEX("stove"),                               "stove",                ItemModelType::stove,             glm::vec3(1.45f),   glm::vec3(0.f, 0.f, -0.225f),  glm::vec3(0.f),                   true },
	{ MDL_SAME_TEX("toilet"),                              "toilet",               ItemModelType::toilet,            glm::vec3(0.6f),    glm::vec3(0.f),                glm::vec3(0.f),                   true },
	{ MDL_SAME_TEX("toiletpaper"),                         "toilet paper",         ItemModelType::toiletPaper,       glm::vec3(0.9f),    glm::vec3(0.f),                glm::vec3(0.f),                   false,      glm::vec3(0.f, 0.f, 1.5f) },
	{ VENT_FILENAMES,                                      "vent",                 ItemModelType::vent,              glm::vec3(2.5f),    glm::vec3(0.f, 0.1f, -0.455f), glm::vec3(0.f),                   true },
	{ WINDOW_FILENAMES,                                    "window",               ItemModelType::window,            WINDOW_SCALE,       glm::vec3(0.f, 0.65f, -0.4f),  glm::vec3(0.f),                   true },
	{ MDL_SAME_TEX("table"),                               "table",                ItemModelType::table,             glm::vec3(1.5f),    glm::vec3(0.f),                glm::vec3(0.f),                   true },
    { MDL_SAME_TEX("net"),                                 "net",                  ItemModelType::net,               glm::vec3(1.f),     glm::vec3(0.f),                glm::vec3(0.f),                   false,      glm::vec3(0.f, 0.f, 0.f) },
	{ MODELS_PATH "ucsd-bear-sp10.obj", nullptr,           "warren bear",          ItemModelType::warrenBear,        glm::vec3(4.f),     glm::vec3(0.f),                glm::vec3(0.f),                   false },
};

struct ItemModel {
	const ItemModelSettings *settings;
	FBXObject *object;
	Geometry *geometry;
};

static vector<ItemModel> itemModels;

static glm::mat4 P; // P for projection
static glm::mat4 V; // V for view
static DirLight * light = nullptr;
static FogGenerator * fog = nullptr;

struct PlayerModel {
	const PlayerModelSettings *settings;
	FBXObject *walkObject;
	FBXObject *carryObject;
	FBXObject *actionObject;
	FBXObject *idleObject;
	FBXObject *idleCarryObject;
	Geometry *walkGeometry;
	Geometry *carryGeometry;
	Geometry *actionGeometry;
	Geometry *idleGeometry;
	Geometry *idleCarryGeometry;

	inline FBXObject * getWalkObject() {
		return walkObject;
	}
	inline FBXObject * getCarryObject() {
		return carryObject ? carryObject : walkObject;
	}
	inline FBXObject * getActionObject() {
		return actionObject ? actionObject : walkObject;
	}
	inline FBXObject * getIdleObject() {
		return idleObject ? idleObject : walkObject;
	}
	inline FBXObject * getIdleCarryObject() {
		return idleCarryObject ? idleCarryObject : walkObject;
	}
	inline Geometry * getwalkGeometry() {
		return walkGeometry;
	}
	inline Geometry * getCarryGeometry() {
		return carryGeometry ? carryGeometry : walkGeometry;
	}
	inline Geometry * getActionGeometry() {
		return actionGeometry ? actionGeometry : walkGeometry;
	}
	inline Geometry * getIdleGeometry() {
			return idleGeometry ? idleGeometry : walkGeometry;
	}
	inline Geometry * getIdleCarryGeometry() {
			return idleCarryGeometry ? idleCarryGeometry : walkGeometry;
	}

	void deallocMembers() {
		DELETE_IF_NEEDED(walkObject);
		DELETE_IF_NEEDED(carryObject);
		DELETE_IF_NEEDED(idleObject);
		DELETE_IF_NEEDED(idleCarryObject);
		DELETE_IF_NEEDED(actionObject);
		DELETE_IF_NEEDED(walkGeometry);
		DELETE_IF_NEEDED(idleGeometry);
		DELETE_IF_NEEDED(idleCarryGeometry);
		DELETE_IF_NEEDED(carryGeometry);
		DELETE_IF_NEEDED(actionGeometry);
	}
};

struct AnimatedItem {
	Transform *transform = nullptr;
	Geometry *animating = nullptr;
	ItemModelType type = ItemModelType::EMPTY;
};

struct EnvObject {
	Transform *transform = nullptr;
	ItemModelType type = ItemModelType::EMPTY;
	GLuint textureOverride = 0;
};

static PlayerModel playerModels[NUM_PLAYER_MODEL_TYPES] = { nullptr };

static FBXObject * tileModel = nullptr;
static FBXObject * wallModel = nullptr;
static FBXObject * animatedBoxObjects[MAX_PLAYERS] = {nullptr};
static FBXObject * netModel = nullptr;
static FBXObject * sandModel = nullptr;

static Skybox *skybox = nullptr;

static UICanvas * uiCanvas = nullptr;
static GLuint objShaderProgram = 0;
static GLuint uiShaderProgram = 0;
static GLuint particleShaderProgram = 0;

static GLuint uiTexture = 0;

static Geometry * tileGeometry = nullptr;
static Geometry * wallGeometry = nullptr;
static Geometry * netGeometry = nullptr;
static vector<vector<Transform *>> floorArray;
static vector<vector<Transform *>> northWalls;
static vector<vector<Transform *>> westWalls;
static vector<vector<EnvObject>> envObjs;
static vector<vector<AnimatedItem>> itemTransforms;

/*static Sound * sound_door;
static Sound * sound_door_unlock;
static Sound * sound_found_item;
static Sound * sound_net;
static Sound * sound_raccoon_up;
static Sound * sound_raccoon_down;
static Sound * sound_search_item;
static Sound * sound_toilet;
static Sound * sound_vent_screw;
static Sound * sound_window;
static Sound * sound_yay;*/

static ParticleSpawner * dustSpawner[MAX_PLAYERS] = { nullptr };
static ParticleSpawner * flashSpawner[MAX_PLAYERS] = { nullptr };
static ParticleSpawner * speedSpawner[MAX_PLAYERS] = { nullptr };
static ParticleSpawner * slowSpawner[MAX_PLAYERS] = { nullptr };
//static ParticleSpawner * buildSpawner;
static ParticleSpawner * blindSpawner[MAX_PLAYERS] = { nullptr };
static ParticleSpawner * searchSpawner[MAX_PLAYERS] = { nullptr };
static ParticleSpawner * bearSpawner[MAX_PLAYERS] = { nullptr };


extern ClientGame * sharedClient;

static int elapsedTime = 0;
static int directions = 0;


#define TWO_DEE_PATH "../2D Elements/"
#define ANIMAL_START_PROMPT_PATH (TWO_DEE_PATH "promptGOGOGOAnimals.png")
#define CHEF_START_PROMPT_PATH (TWO_DEE_PATH "promptGOGOGOChef.png")

static double mainLoopBeginTime = 0.0;
static GLuint twoDeeShader = 0;
static FBXObject *animalStartingPrompt = nullptr;
static FBXObject *chefStartingPrompt = nullptr;

static GLuint colorizeShader = 0;
static GLuint solidColorShader = 0;
static FBXObject *solidColorObject = nullptr;
static FBXObject *mapPingObject = nullptr;

struct PlayerState {
	glm::mat4 transform;        // for position and rotation
	float targetAngle;          // the angle it's trying to animate to
	float angle;                // angle it's currently facing (should be approaching targetAngle)
	glm::vec3 position;         // position, important for faking targetAngle above
	int id;                     // the player ID from the server
	int number;					// the player's assigned number from the server
	unsigned geometryIdx;       // index into playerGeometry
	char moving;				// bool indicating whether or not the model should animate
	char building;				// bool indicating whether or not model is doing iteraction animation
	glm::vec3 buildPosition = glm::vec3(0.0f);    // where to spawn build effects
	int movingSpeed = 0;			// -1 = slow, 0 = normal, 1 = fast
	int flashedRecently;		//counts down to fire a burst of flash particles
	bool reversed = false; //will activate if player is reversed: should only activate on chef
	bool instantSearch = false; //will activate if player has instant search. 
	bool restrictRotation = false;
	bool isBear = false; //will activate if player has bear powerup

	ItemModelType previousInventory;
	glm::mat4 inventoryTransform;
	double carryStopTime;
	Location carryStopLoc;
	bool animatingInventory = false;

	PlayerState() : position(0.f), transform(1.f) {
		angle = targetAngle = 0.f;
		geometryIdx = 0;
		previousInventory = ItemModelType::EMPTY;
	}

#ifdef DUMMY_ID
	// Warning: this constructor is only for debugging use
	PlayerState(int newId, int geomIdx) : PlayerState() {
		id = newId;
		geometryIdx = geomIdx;

		setGeometryIdx(geometryIdx);
	}
#endif

	PlayerState(int newId, const Player *p) : PlayerState() {
		id = newId;
		number = p->getPlayerNum();
		const auto location = p->getLocation();
		position.x = location.getX();
		position.y = location.getY();
		position.z = location.getZ();

		setGeometryIdx(static_cast<unsigned>(p->getModelType()));

		moving = 0;
	}

	~PlayerState() {
	}

	void setGeometryIdx(unsigned geomIdx) {
		geometryIdx = geomIdx;

		auto translate = glm::translate(glm::mat4(1.f), position);
		transform = glm::rotate(translate, angle, glm::vec3(0.f, 1.f, 0.f));
	}

	void setTargetAngle(float newTargetAngle) {
		// Try to find an equivalent (+/- 2pi) target that is closer
		float alternateTarget = newTargetAngle - glm::two_pi<float>();
		if (abs(newTargetAngle - angle) > abs(alternateTarget - angle)) {
			newTargetAngle = alternateTarget;
		}
		alternateTarget = newTargetAngle + glm::two_pi<float>();
		if (abs(newTargetAngle - angle) > abs(alternateTarget - angle)) {
			newTargetAngle = alternateTarget;
		}

		targetAngle = newTargetAngle;
	}
};

static std::vector<PlayerState> players;
static Transform * floorTransform = nullptr;
static Transform * envObjsTransform = nullptr;
static Transform * allItemsTransform = nullptr;

// Default camera parameters
static glm::vec3 cam_pos(45.0f, 60.0f, 45.0f);          // e  | Position of camera
static glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);         // d  | This is where the camera looks at
static glm::vec3 cam_up(0.0f, 1.0f, 0.0f);              // up | What orientation "up" is
static const glm::vec3 cam_overhead_angle(-10.f, 50.f, -30.f);   // camera's preferred offset from cam_look_at
static bool thirdPersonView = false;

glm::vec3 ingame_light_center = glm::vec3(0.f);
float ingame_light_radius;

static int cameraClientId = -1;

bool mouseRotation = false;
glm::vec2 prevPos = glm::vec2(0.0f);
float scaleDownMouseOps = 30.0f;
float scaleUpAngle = 4.0f;
float scaleMouseWheel = 1.05f;

glm::vec3 directionBitmaskToVector(int bitmask);
void deallocFloor();
void SendPackets();
void DisplayCallback(GLFWwindow* window);
void UpdateView();
glm::vec3 TrackballMapping(double x, double y, int width, int height);
void TrackballRotation(float rotationAngle, glm::vec3 rotationAxis);

void toggleClient(); // from Tester.cpp

static PlayerState *getMyState()
{
	// linear search is probably faster than binary/tree/whatever for <= 4 elements

	int myId = sharedClient->getMyID();
	for (PlayerState &state : players) {
		if (state.id == myId)
			return &state;
	}

	return nullptr;
}

void InGameGraphicsEngine::ReloadPlayers()
{
	players.clear();

	cout << "[Re]loading players\n";

	if (!sharedClient || !sharedClient->getGameData() || sharedClient->getGameData()->players.size() <= 0) {
		cout << "WARNING: gameData players map is empty\n";
		return;
	}

	const auto &playersMap = sharedClient->getGameData()->players;
	const int myId = sharedClient->getMyID();

	for (const auto &pair : playersMap) {
		const auto id = pair.first;
		const Player *p = pair.second;

		players.emplace_back(id, p);
	}

#ifdef DUMMY_ID
	// add a dummy player
	players.emplace_back(DUMMY_ID, CHEF_IDX);
	const auto state = &players[players.size() - 1];
#endif

	auto myState = getMyState();
	if (myState) {
		InitCamera(myState->position);
	}
}

void resetEnvObjs()
{
	const auto &tileLayout = sharedClient->getGameData()->clientTileLayout;

	if (!tileLayout.size()) {
		cerr << "resetEnvObjs(): map is empty\n";
		return;
	}

	if (envObjsTransform == nullptr) {
		envObjsTransform = new Transform(glm::mat4(1.f));
	}
	else {
		envObjsTransform->removeAllChildren();
	}

	uint8_t objIdx = 0;
	envObjs.resize(floorArray.size());
	for (size_t z = 0; z < envObjs.size(); z++) {
		auto &row = envObjs[z];
		row.resize(floorArray[z].size());
		for (size_t x = 0; x < row.size(); x++) {
			const auto tile = tileLayout[z][x];

#ifdef ENV_OBJS_DEMO
			if (objIdx == 0) objIdx = 1;
			else if (++objIdx >= itemModels.size()) objIdx = 1;
#else
			switch (tile->getTileType())
			{
			case TileType::JAIL:
			{
				objIdx = static_cast<uint8_t>(ItemModelType::jail);
				break;
			}
			case TileType::BOX:
			{
				objIdx = static_cast<uint8_t>(ItemModelType::box);
				break;
			}
			case TileType::GATE:
			{
				GateTile *objTile = (GateTile *)tile;
				objIdx = static_cast<uint8_t>(objTile->getModel());
				break;
			}
			case TileType::KEY_DROP:
			{
				KeyDropTile *objTile = (KeyDropTile *)tile;
				objIdx = static_cast<uint8_t>(objTile->getModel());
				break;
			}
			case TileType::OBJECT:
			{
				ObjectTile *objTile = (ObjectTile *)tile;
				objIdx = static_cast<uint8_t>(objTile->getModel());
				break;
			}
			default:
				objIdx = 0;
				break;
			}

			row[x].textureOverride = 0;
			if (objIdx == 0) {
				bool found = false;
				for (const auto &extra : extraObjects) {
					if (x == extra.x && z == extra.z) {
						row[x].textureOverride = extra.texture ? loadTexture(extra.texture) : 0;
						objIdx = static_cast<uint8_t>(extra.type);
						found = true;
						break;
					}
				}

				if (!found) {
					row[x].transform = nullptr;
					continue;
				}
			}
#endif

			ItemModelType modelType = static_cast<ItemModelType>(objIdx);
			if (modelType == ItemModelType::EMPTY || !itemModels[objIdx].settings) {
				row[x].transform = nullptr;
				continue;
			}

			const auto &settings = *(itemModels[objIdx].settings);

			// Try to turn the object away from the wall
			float angle = 0.f;
			if (settings.wallRotate) {
				auto wall = tile->getWall();
				if (wall & DirectionBitmask::southSide) {
					angle = glm::pi<float>();
				}
				else if (wall & DirectionBitmask::westSide) {
					angle = glm::half_pi<float>();
				}
				else if (wall & DirectionBitmask::eastSide) {
					angle = -glm::half_pi<float>();
				}
			}

			glm::vec3 tileTranslate;
			tileTranslate.x = (x + 0.5f) * TILE_STRIDE * TILE_SCALE;
			tileTranslate.y = (tile->getHeight()) * 0.5f * TILE_LEVEL_OFFSET * TILE_SCALE;
			tileTranslate.z = (z + 0.5f) * TILE_STRIDE * TILE_SCALE;

			auto tileTranslateMat = glm::translate(glm::mat4(1.f), tileTranslate);
			auto rotate = glm::rotate(tileTranslateMat, angle, glm::vec3(0.f, 1.f, 0.f));

			glm::vec3 modelTranslate = settings.translate;
			modelTranslate.x *= TILE_STRIDE * TILE_SCALE;
			modelTranslate.y *= TILE_LEVEL_OFFSET * TILE_SCALE;
			modelTranslate.z *= TILE_STRIDE * TILE_SCALE;

			auto scale = glm::scale(glm::translate(rotate, modelTranslate), settings.scale);

			auto modelAngles = settings.rotation;
			auto modelRotate = glm::rotate(scale, modelAngles.y, glm::vec3(0.f, 1.f, 0.f));
			modelRotate = glm::rotate(modelRotate, modelAngles.x, glm::vec3(1.f, 0.f, 0.f));
			modelRotate = glm::rotate(modelRotate, modelAngles.z, glm::vec3(0.f, 0.f, 1.f));

			row[x].type = modelType;
			row[x].transform = new Transform(modelRotate);
			row[x].transform->addChild(itemModels[objIdx].geometry);
			envObjsTransform->addChild(row[x].transform);
		}
	}
}

void resetItems()
{
	const auto &tileLayout = sharedClient->getGameData()->clientTileLayout;

	if (!tileLayout.size()) {
		cerr << "resetItems(): map is empty\n";
		return;
	}

	if (allItemsTransform == nullptr) {
		allItemsTransform = new Transform(glm::mat4(1.f));
	} 
	else {
		allItemsTransform->removeAllChildren();
	}

	itemTransforms.resize(floorArray.size());
	for (size_t z = 0; z < itemTransforms.size(); z++) {
		auto &row = itemTransforms[z];
		row.resize(floorArray[z].size());
		for (size_t x = 0; x < row.size(); x++) {
			const auto tile = tileLayout[z][x];

			ItemModelType modelType = tile->getItem();
			uint8_t objIdx = static_cast<uint8_t>(modelType);
			
			if (modelType == ItemModelType::EMPTY || !itemModels[objIdx].settings) {
				row[x].transform = nullptr;
				row[x].animating = nullptr;
				row[x].type = modelType;
				continue;
			}

			const auto &settings = *(itemModels[objIdx].settings);

			// Try to turn the object away from the wall
			float angle = 0.f;
			if (settings.wallRotate) {
				auto wall = tile->getWall();
				if (wall & DirectionBitmask::southSide) {
					angle = glm::pi<float>();
				}
				else if (wall & DirectionBitmask::westSide) {
					angle = glm::half_pi<float>();
				}
				else if (wall & DirectionBitmask::eastSide) {
					angle = -glm::half_pi<float>();
				}
			}

			glm::vec3 tileTranslate;
			tileTranslate.x = (x + 0.5f) * TILE_STRIDE * TILE_SCALE;
			tileTranslate.y = (tile->getHeight()) * 0.5f * TILE_LEVEL_OFFSET * TILE_SCALE;
			tileTranslate.z = (z + 0.5f) * TILE_STRIDE * TILE_SCALE;

			// If on a ramp, give a slight height boost
			if (tile->getHeight() & 1) {
				tileTranslate.y += 4.f;
			}

			if (tile->getTileType() == TileType::OBJECT) {
				if (((ObjectTile *)tile)->getModel() == ItemModelType::table) {
					tileTranslate.y += TABLE_HEIGHT;
				}
			}

			auto tileTranslateMat = glm::translate(glm::mat4(1.f), tileTranslate);
			auto rotate = glm::rotate(tileTranslateMat, angle, glm::vec3(0.f, 1.f, 0.f));

			glm::vec3 modelTranslate = settings.translate;
			modelTranslate.x *= TILE_STRIDE * TILE_SCALE;
			modelTranslate.y *= TILE_LEVEL_OFFSET * TILE_SCALE;
			modelTranslate.z *= TILE_STRIDE * TILE_SCALE;

			auto scale = glm::scale(glm::translate(rotate, modelTranslate), settings.scale);

#define ITEM_ROTATION_PERIOD 5.25
			auto t = glfwGetTime();
			float timeAngle = (float)(fmod(t, ITEM_ROTATION_PERIOD) * glm::two_pi<double>() / ITEM_ROTATION_PERIOD);

			auto modelAngles = settings.rotation;
			auto modelRotate = glm::rotate(scale, modelAngles.y + timeAngle, glm::vec3(0.f, 1.f, 0.f));
			modelRotate = glm::rotate(modelRotate, modelAngles.x, glm::vec3(1.f, 0.f, 0.f));
			modelRotate = glm::rotate(modelRotate, modelAngles.z, glm::vec3(0.f, 0.f, 1.f));

			row[x].animating = nullptr;
			row[x].type = modelType;
			row[x].transform = new Transform(modelRotate);
			row[x].transform->addChild(itemModels[objIdx].geometry);
			allItemsTransform->addChild(row[x].transform);
		}
	}
}

void setGoalsFalse() {
	if (!uiCanvas)
		return;

	uiCanvas->setVisible(UICanvas::YELLOW_KEY_GOAL_COMPLETE, false);
	uiCanvas->setVisible(UICanvas::BLUE_KEY_GOAL_COMPLETE, false);
	uiCanvas->setVisible(UICanvas::GREEN_KEY_GOAL_COMPLETE, false);
	uiCanvas->setVisible(UICanvas::YELLOW_SCREWDRIVER_GOAL_COMPLETE, false);
	uiCanvas->setVisible(UICanvas::GREEN_SCREWDRIVER_GOAL_COMPLETE, false);
	uiCanvas->setVisible(UICanvas::RED_SCREWDRIVER_GOAL_COMPLETE, false);
	uiCanvas->setVisible(UICanvas::ROPE_GOAL_COMPLETE, false);
	uiCanvas->setVisible(UICanvas::TOILET_PAPER_GOAL_COMPLETE, false);
	uiCanvas->setVisible(UICanvas::PLUNGER_GOAL_COMPLETE, false);
	uiCanvas->setVisible(UICanvas::YELLOW_KEY_GOAL_INCOMPLETE, true);
	uiCanvas->setVisible(UICanvas::BLUE_KEY_GOAL_INCOMPLETE, true);
	uiCanvas->setVisible(UICanvas::GREEN_KEY_GOAL_INCOMPLETE, true);
	uiCanvas->setVisible(UICanvas::YELLOW_SCREWDRIVER_GOAL_INCOMPLETE, true);
	uiCanvas->setVisible(UICanvas::GREEN_SCREWDRIVER_GOAL_INCOMPLETE, true);
	uiCanvas->setVisible(UICanvas::RED_SCREWDRIVER_GOAL_INCOMPLETE, true);
	uiCanvas->setVisible(UICanvas::ROPE_GOAL_INCOMPLETE, true);
	uiCanvas->setVisible(UICanvas::TOILET_PAPER_GOAL_INCOMPLETE, true);
	uiCanvas->setVisible(UICanvas::PLUNGER_GOAL_INCOMPLETE, true);
}

void setGoalsVisible(int item) {
	if (!uiCanvas) {
		return;
	}

	if (item == (int)ItemModelType::key1) {
		uiCanvas->setVisible(UICanvas::YELLOW_KEY_GOAL_COMPLETE, true);
		uiCanvas->setVisible(UICanvas::YELLOW_KEY_GOAL_INCOMPLETE, false);

	}
	if (item == (int)ItemModelType::key2) {
		uiCanvas->setVisible(UICanvas::BLUE_KEY_GOAL_COMPLETE, true);
		uiCanvas->setVisible(UICanvas::BLUE_KEY_GOAL_INCOMPLETE, false);

	}
	if (item == (int)ItemModelType::key3) {
		uiCanvas->setVisible(UICanvas::GREEN_KEY_GOAL_COMPLETE, true);
		uiCanvas->setVisible(UICanvas::GREEN_KEY_GOAL_INCOMPLETE, false);

	}
	if (item == (int)ItemModelType::screwdriver1) {
		uiCanvas->setVisible(UICanvas::YELLOW_SCREWDRIVER_GOAL_COMPLETE, true);
		uiCanvas->setVisible(UICanvas::YELLOW_SCREWDRIVER_GOAL_INCOMPLETE, false);

	}
	if (item == (int)ItemModelType::screwdriver2) {
		uiCanvas->setVisible(UICanvas::GREEN_SCREWDRIVER_GOAL_COMPLETE, true);
		uiCanvas->setVisible(UICanvas::GREEN_SCREWDRIVER_GOAL_INCOMPLETE, false);

	}
	if (item == (int)ItemModelType::screwdriver3) {
		uiCanvas->setVisible(UICanvas::RED_SCREWDRIVER_GOAL_COMPLETE, true);
		uiCanvas->setVisible(UICanvas::RED_SCREWDRIVER_GOAL_INCOMPLETE, false);
	}
	if (item == (int)ItemModelType::rope) {
		uiCanvas->setVisible(UICanvas::ROPE_GOAL_COMPLETE, true);
		uiCanvas->setVisible(UICanvas::ROPE_GOAL_INCOMPLETE, false);
	}
	if (item == (int)ItemModelType::plunger) {
		uiCanvas->setVisible(UICanvas::PLUNGER_GOAL_COMPLETE, true);
		uiCanvas->setVisible(UICanvas::PLUNGER_GOAL_INCOMPLETE, false);
	}
	if (item == (int)ItemModelType::toiletPaper) {
		uiCanvas->setVisible(UICanvas::TOILET_PAPER_GOAL_COMPLETE, true);
		uiCanvas->setVisible(UICanvas::TOILET_PAPER_GOAL_INCOMPLETE, false);
	}
}

void updateBoxVisibility()
{
	//for goalsUI
	setGoalsFalse();

	if (!sharedClient || !sharedClient->getGameData())
		return;

	const auto &tileLayout = sharedClient->getGameData()->clientTileLayout;

	if (!tileLayout.size()) {
		return;
	}

	auto &allPlayers = sharedClient->getGameData()->getAllPlayers();

	unsigned y = 0;
	for (auto &row : envObjs) {
		unsigned x = 0;
		for (auto &envObj : row) {
			auto tile = envObj.transform;

			if (tile && tileLayout[y][x]->getTileType() == TileType::BOX) {
				tile->hidden = !((BoxTile *)tileLayout[y][x])->hasBox();
			}

			if (tile && tileLayout[y][x]->getTileType() == TileType::GATE) {
				GateTile *gate = (GateTile *)tileLayout[y][x];

				bool foundPlayer = false;

				for (auto pair : allPlayers) {
					if (pair.second->getAction() == Action::CONSTRUCT_GATE) {
						auto loc = pair.second->getLocation();
						int playerX = (int)(loc.getX() / TILE_SIZE_SERVER);
						int playerZ = (int)(loc.getZ() / TILE_SIZE_SERVER);
						if (playerX == x && playerZ == y) {
							foundPlayer = true;
							break;
						}
					}
				}

#ifdef DEBUG_GATE_ANIMATION
				foundPlayer = true;
#endif

				Transform *transformNode = envObjs[y][x].transform;
				if (foundPlayer && gate->getModel() == ItemModelType::window && WINDOW_SCALED_UP) {
					// compensate for weird animation transform
					transformNode->undoScale();
					WINDOW_SCALED_UP = false;
				}
				else if (!foundPlayer &&gate->getModel() == ItemModelType::window && !WINDOW_SCALED_UP) {
					// use regular transform
					transformNode->applyScale(WINDOW_SCALE);
  					WINDOW_SCALED_UP = true;
				}

				glm::vec3 position((x + 0.5f) * TILE_SIZE_SERVER, 0.f, (y + 0.5f) * TILE_SIZE_SERVER);
				if (glm::distance(position, ingame_light_center) < ingame_light_radius + ITEM_MDL_CLIP_RADIUS) {
					auto index = static_cast<unsigned>(gate->getModel());
					itemModels[index].object->Update(foundPlayer);
				}
			}

			//Update UI to express keys already placed
			if (tile && tileLayout[y][x]->getTileType() == TileType::KEY_DROP) {
				std::vector<ItemModelType> depositedItems = ((KeyDropTile *)tileLayout[y][x])->getDepositedKeys();
				for (int i = 0; i < depositedItems.size(); i++) {
					int item = (int)depositedItems[i];
					setGoalsVisible(item);
				}
			}
			++x;
		}
		++y;
	}
}

void reloadMap()
{
	deallocFloor();

	if (!sharedClient || !sharedClient->getGameData())
		return;

	const auto &tileLayout = sharedClient->getGameData()->clientTileLayout;

	if (!tileLayout.size()) {
		cerr << "reloadMap(): map is empty\n";
		return;
	}

	cout << "[Re]loading map\n";

	if (floorTransform == nullptr) {
		const glm::vec3 transAmount(TILE_STRIDE / 2, TILE_HEIGHT_ADJUST, TILE_STRIDE / 2);
		const auto scale = glm::scale(glm::mat4(1.0f), glm::vec3(TILE_SCALE));
		floorTransform = new Transform(glm::translate(scale, transAmount));
	}

	// Floor tiles
	floorArray.resize(tileLayout.size());
	for (size_t z = 0; z < floorArray.size(); z++) {
		auto &row = floorArray[z];

		row.resize(tileLayout[z].size());

		for (size_t x = 0; x < row.size(); x++) {
			const auto &tile = tileLayout[z][x];

			float y = tile->getHeight() * 0.5f * TILE_LEVEL_OFFSET;
			auto skew = glm::mat4(1.0f);

			int rampDirection = 0;
			if (tile->getTileType() == TileType::RAMP) {
				const RampTile *rampTile = (RampTile *)tile;
				rampDirection = static_cast<int>(rampTile->getRampDirection());
			}

			switch (rampDirection) {
			case DirectionBitmask::eastSide:
				skew[0][1] = TILE_LEVEL_OFFSET / TILE_STRIDE;
				break;
			case DirectionBitmask::westSide:
				skew[0][1] = -TILE_LEVEL_OFFSET / TILE_STRIDE;
				break;
			case DirectionBitmask::northSide:
				skew[2][1] = -TILE_LEVEL_OFFSET / TILE_STRIDE;
				break;
			case DirectionBitmask::southSide:
				skew[2][1] = TILE_LEVEL_OFFSET / TILE_STRIDE;
				break;
			default:
				break;
			}
			const auto translate = glm::translate(glm::mat4(1.0f), glm::vec3(x * TILE_STRIDE, y, z * TILE_STRIDE));

			row[x] = new Transform(translate * skew);
			row[x]->addChild(tileGeometry);
			floorTransform->addChild(row[x]);
		}
	}

	// North walls
	northWalls.resize(floorArray.size() + 1);
	for (size_t z = 0; z < northWalls.size(); z++) {
		auto &row = northWalls[z];

		auto clippedZ = (z < floorArray.size()) ? z : (floorArray.size() - 1);
		row.resize(floorArray[clippedZ].size());

		for (size_t x = 0; x < row.size(); x++) {
			if ((z > 0 && (tileLayout[z - 1][x]->getWall() & DirectionBitmask::southSide)) ||
				(z == clippedZ && (tileLayout[z][x]->getWall() & DirectionBitmask::northSide)))
			{
				// Calculate the altitude of the wall
				float height = 0.f;
				float scaleY = 1.f;
				if (z == 0 || z != clippedZ) {
					scaleY = 1.f + 0.25f * tileLayout[clippedZ][x]->getHeight();
				} else {
					scaleY = 1.f + 0.25f * max(tileLayout[z - 1][x]->getHeight(), tileLayout[z][x]->getHeight());
				}
				float y = TILE_STRIDE * 0.9f + (height / 2) * TILE_LEVEL_OFFSET;

				// translate to the edge between tiles
				row[x] = new Transform(glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(x * TILE_STRIDE, y, (z - 0.5f) * TILE_STRIDE)), glm::vec3(1.f, scaleY, 1.f)));
				row[x]->addChild(wallGeometry);
				floorTransform->addChild(row[x]);
			}
		}
	}

	westWalls.resize(floorArray.size());
	for (size_t z = 0; z < westWalls.size(); z++) {
		auto &row = westWalls[z];

		row.resize(floorArray[z].size() + 1);

		for (size_t x = 0; x < row.size(); x++) {
			auto clippedX = (x < row.size() - 1) ? x : (row.size() - 2);

			if ((x > 0 && (tileLayout[z][x - 1]->getWall() & DirectionBitmask::eastSide)) ||
				(x == clippedX && (tileLayout[z][x]->getWall() & DirectionBitmask::westSide)))
			{
				// Calculate the altitude of the wall
				float height = 0.f;
				float scaleY = 1.f;
				if (x == 0 || x != clippedX) {
					scaleY = 1.f + 0.25f * tileLayout[z][clippedX]->getHeight();
				}
				else {
					scaleY = 1.f + 0.25f * max(tileLayout[z][x - 1]->getHeight(), tileLayout[z][x]->getHeight());
				}
				float y = TILE_STRIDE * 0.9f + (height / 2) * TILE_LEVEL_OFFSET;

				// translate to the edge between tiles
				const auto translate = glm::translate(glm::mat4(1.0f), glm::vec3((x - 0.5f) * TILE_STRIDE, y, z * TILE_STRIDE));
				// 90 degree rotation
				const auto rotate = glm::rotate(translate, glm::half_pi<float>(), glm::vec3(0.f, 1.f, 0.f));
				row[x] = new Transform(glm::scale(rotate, glm::vec3(1.f, scaleY, 1.f)));
				row[x]->addChild(wallGeometry);
				floorTransform->addChild(row[x]);
			}
		}
	}

	resetEnvObjs();
	resetItems();
}

void deallocFloor()
{
	DELETE_IF_NEEDED(floorTransform);

	for (size_t z = 0; z < floorArray.size(); z++) {
		auto &row = floorArray[z];

		for (size_t x = 0; x < row.size(); x++) {
			delete row[x];
		}

		row.resize(0);
	}

	floorArray.clear();
}

glm::vec3 directionBitmaskToVector(int bitmask) {
	int dirX = ((bitmask & DirectionBitmask::eastSide) != 0) - ((bitmask & DirectionBitmask::westSide) != 0);
	int dirZ = ((bitmask & DirectionBitmask::northSide) != 0) - ((bitmask & DirectionBitmask::southSide) != 0);

	return glm::vec3((float)dirX, 0.f, (float)dirZ);
}

void SendPackets()
{
	if (!sharedClient)
		return;

	auto dir = directionBitmaskToVector(directions);

	if (dir.z > 0) {
		sharedClient->sendPackets(FORWARD_EVENT);
	}
	else if (dir.z < 0) {
		sharedClient->sendPackets(BACKWARD_EVENT);
	}

	if (dir.x > 0) {
		sharedClient->sendPackets(LEFT_EVENT);
	}
	else if (dir.x < 0) {
		sharedClient->sendPackets(RIGHT_EVENT);
	}
}

void InGameGraphicsEngine::MovePlayers()
{
	// Sanity guard condition
	auto myState = getMyState();
	if (players.size() <= 0 || myState == nullptr) {
		return;
	}

	const int myID = sharedClient->getMyID();
	const auto &playersMap = sharedClient->getGameData()->players;
	for (auto &state : players) {
		const auto oldPos = state.position;

		if (playersMap.count(state.id) > 0) {
			const Player * p = playersMap.at(state.id);
			const auto loc = p->getLocation();

			state.position = glm::vec3(loc.getX(), loc.getY(), loc.getZ());
			if (p->getAction() == Action::SWING_NET && p->isChef()) {
				state.angle = state.angle + 0.0675f * glm::two_pi<float>();
				state.restrictRotation = true;
			}
			else {
				state.restrictRotation = false;
				state.angle = fmod(state.angle, glm::two_pi<float>());
			}
		}
#ifdef DUMMY_ID
		else if (state.id == DUMMY_ID) {
			state.position = myState->position;
			if (floorArray.size()) {
				state.position.x = floorArray[0].size() * 20.f - state.position.x;
			}
		}
#endif

		// if the position changed, tell the object that it is indeed moving
		state.moving <<= 1;
		state.moving |= (char)(state.position != oldPos);


		const glm::mat4 newOffset = glm::translate(glm::mat4(1.0f), state.position);

		// Set targetAngle based on keyboard for me, based on movement for others
		if (!state.restrictRotation) {
			if (state.id != myID) {
				const auto delta = state.position - oldPos;
				if (abs(delta.x) > 0.0001 || abs(delta.z) > 0.0001) {
					state.setTargetAngle(glm::atan(delta.x, delta.z));
				}
			}
			else {
				const auto dir = directionBitmaskToVector(directions);
				if (dir.x != 0.f || dir.z != 0.f) {
					state.setTargetAngle(glm::atan(-dir.x, dir.z));
				}
			}

			// Animate state.angle towards state.targetAngle
			float targetAngle = state.targetAngle;
			if (state.id == myID && state.geometryIdx == static_cast<unsigned>(ModelType::CHEF) && sharedClient->getGameData()->getReverseChef()) {
				targetAngle = fmodf(targetAngle + glm::pi<float>(), glm::two_pi<float>());
			}
			state.angle += (targetAngle - state.angle) * 0.2f;
			if (abs(fmodf(state.targetAngle - state.angle, glm::two_pi<float>())) < 0.01) {
				// state.angle has gotten close enough to state.targetAngle, so make them both between 0 and 2pi
				state.angle = state.targetAngle = fmod(state.targetAngle, glm::two_pi<float>());
			}
		}

		state.transform = glm::rotate(newOffset, state.angle, glm::vec3(0.f, 1.f, 0.f));
	}

	MoveCamera(myState->position);
	UpdateView();
}

void InGameGraphicsEngine::InitCamera(const glm::vec3 &newPlayerPos) {
	cam_look_at.x = newPlayerPos.x;
	cam_look_at.z = newPlayerPos.z;
	cam_look_at.y = TILE_LEVEL_OFFSET * TILE_SCALE + TILE_HEIGHT_ADJUST;

	cam_pos = cam_look_at + cam_overhead_angle;

	UpdateView();
}

ItemModelType ModelTypeForWT(WinType wt) {
	switch (wt) {
	case WinType::DOOR:
		return ItemModelType::door;
	case WinType::TOILET:
		return ItemModelType::window;
	case WinType::VENT:
		return ItemModelType::vent;
	default:
		return ItemModelType::EMPTY;
	}
}

static glm::vec3 LookAtForWT(WinType wt) {
	switch (wt) {
	case WinType::CHEF_WIN:
	{
		for (auto &player : players) {
			if (player.geometryIdx == static_cast<unsigned>(ModelType::CHEF)) {
				return player.position;
			}
		}
		break;
	}
	case WinType::DOOR:
	case WinType::TOILET:
	case WinType::VENT:
	{
		const auto modelType = ModelTypeForWT(wt);
		const auto &tileLayout = sharedClient->getGameData()->getTileLayout();

		for (unsigned z = 0; z < tileLayout.size(); ++z) {
			const auto &row = tileLayout[z];
			for (unsigned x = 0; x < row.size(); ++x) {
				Tile *tile = row[x];
				if (tile->getTileType() != TileType::GATE) {
					continue;
				}

				GateTile *gate = (GateTile *)tile;
				if (gate->getModel() == modelType) {
					auto v = glm::vec3(x * TILE_STRIDE, TILE_LEVEL_OFFSET, z * TILE_STRIDE) * TILE_SCALE;
					return v + glm::vec3(0.5f * TILE_STRIDE * TILE_SCALE, TILE_HEIGHT_ADJUST, 0.5f * TILE_STRIDE * TILE_SCALE);
				}
			}
		}
		break;
	}
	default:
		break;
	}

	return cam_look_at;
}

static glm::vec3 limitLookAt(glm::vec3 lookAt) {
	if (!floorArray.size()) {
		return lookAt;
	}

#define LOOKAT_X_PADDING (TILE_SCALE * TILE_STRIDE * 1.75f)
#define LOOKAT_Z_PADDING (TILE_SCALE * TILE_STRIDE * 1.0f)

	if (lookAt.x < LOOKAT_X_PADDING) {
		lookAt.x = LOOKAT_X_PADDING;
	}
	else if (lookAt.x > floorArray[0].size() * TILE_SCALE * TILE_STRIDE - LOOKAT_X_PADDING) {
		lookAt.x = floorArray[0].size() * TILE_SCALE * TILE_STRIDE - LOOKAT_X_PADDING;
	}

	if (lookAt.z < LOOKAT_Z_PADDING) {
		lookAt.z = LOOKAT_Z_PADDING;
	}
	else if (lookAt.z > floorArray.size() * TILE_SCALE * TILE_STRIDE - LOOKAT_Z_PADDING) {
		lookAt.z = floorArray.size() * TILE_SCALE * TILE_STRIDE - LOOKAT_Z_PADDING;
	}

	return lookAt;
}

static bool isBeingCarriedByChef(int id)
{
	for (auto pair : sharedClient->getGameData()->getAllPlayers()) {
		auto chef = pair.second;
		if (chef->isChef() && chef->getCaughtAnimalId() == id) return true;
	}

	return false;
}

static void toggleJailCam()
{
	auto gameData = sharedClient->getGameData();
	if (gameData) {
		const auto &allPlayers = gameData->getAllPlayers();
		bool cameraClientSeen = false;
		int firstId = -1;
		int nextId = -1;

		for (auto pair : allPlayers) {
			if (pair.first == cameraClientId) {
				cameraClientSeen = true;
			}
			else if (!pair.second->isChef()) {
				if (firstId == -1)
					firstId = pair.first;

				if (cameraClientSeen && nextId == -1)
					nextId = pair.first;
			}
		}

		if (nextId != -1) {
			cameraClientId = nextId;
		}
		else if (firstId != -1) {
			cameraClientId = firstId;
		}
	}
}

static bool iAmJailed()
{
#ifdef DEBUG_CAUGHT
	return true;
#else
	auto gameData = sharedClient->getGameData();
	if (!gameData) return false;

	auto me = gameData->getPlayer(sharedClient->getMyID());
	return me->isCaught() && !isBeingCarriedByChef(sharedClient->getMyID());
#endif
}

void InGameGraphicsEngine::MoveCamera(const glm::vec3 &newPlayerPos) {
	
	ingame_light_center = newPlayerPos;
	
	if (!sharedClient || !sharedClient->getGameData())
		return;

	auto &allPlayers = sharedClient->getGameData()->getAllPlayers();

	glm::vec3 look_target = limitLookAt(newPlayerPos);
	glm::vec3 cam_target = look_target + cam_overhead_angle;
	float look_speed = 0.3f, cam_speed = 0.3f;

#ifdef DEBUG_CAUGHT
	bool watchOthersEnabled = true;
#else
	bool watchOthersEnabled = iAmJailed();
#endif

	if (sharedClient->getGameData()->getWT() != WinType::NONE) {
		ingame_light_center = LookAtForWT(sharedClient->getGameData()->getWT());

		look_target = limitLookAt(ingame_light_center);
		cam_target = look_target + cam_overhead_angle;
		look_speed = 0.3f;
		cam_speed = 0.15f;

		if (glm::distance(cam_target, cam_pos) < 0.001f) {
			quit = true;
		}
	}
	else if (watchOthersEnabled) {
		if (allPlayers.count(cameraClientId) <= 0) {
			cameraClientId = sharedClient->getMyID();
			for (auto pair : allPlayers) {
				if (pair.first != sharedClient->getMyID() && !pair.second->isChef() && !pair.second->isCaught()) {
					cameraClientId = pair.first;
					break;
				}
			}
		}

		Player *networkPlayer = allPlayers.at(cameraClientId);
		auto loc = networkPlayer->getLocation();
		auto position = glm::vec3(loc.getX(), loc.getY(), loc.getZ());
		ingame_light_center = position;
		look_target = limitLookAt(position);
		cam_target = look_target + cam_overhead_angle;
		look_speed = 0.3f;
		cam_speed = 0.3f;
	}
	else if (thirdPersonView) {
		auto myState = getMyState();
		if (myState) {
#define THIRD_PERSON_DIST 15.f
			float camAngle = fmodf(myState->targetAngle + glm::three_over_two_pi<float>(), glm::two_pi<float>());

			cam_target.x = myState->position.x - THIRD_PERSON_DIST * cosf(camAngle);
			cam_target.y = myState->position.y + 25.f;
			cam_target.z = myState->position.z + THIRD_PERSON_DIST * sinf(camAngle);

			look_target.x = myState->position.x;
			look_target.y = myState->position.y + 23.f;
			look_target.z = myState->position.z;
		}
	}

	cam_look_at += (look_target - cam_look_at) * look_speed;
	if (!mouseRotation) {
		cam_pos += (cam_target - cam_pos) * cam_speed;
	}
	UpdateView();
}

// Call glFlush(), but only if we haven't already called it this frame
static bool alreadyFlushed = false;
void idempotentFlush()
{
	if (!alreadyFlushed) {
		alreadyFlushed = true;
		glFlush();
	}
}

void resetIdempotentFlush()
{
	alreadyFlushed = false;
}

void updateUIElements(GameData * gameData) {
	PlayerState * currState = getMyState();

	if (!currState || !uiCanvas) {
		return;
	}

	uiCanvas->setAngerRatio(((float)gameData->getChefAnger())/CHEF_MAX_ANGER);

	std::map<int, Player*> players = gameData->getAllPlayers();
	if (gameData->getChefAnger() >= CHEF_MAX_ANGER){
		uiCanvas->setVisible(UICanvas::ANGRY_METER_1, false);
		uiCanvas->setVisible(UICanvas::ANGRY_METER_2, true);
	}
	else {
		uiCanvas->setVisible(UICanvas::ANGRY_METER_1, true);
		uiCanvas->setVisible(UICanvas::ANGRY_METER_2, false);
	}
	//check if animals are caught
	//activates when animal is first in net.
	for (auto it = players.begin(); it != players.end(); ++it) {
		//std::cerr << "In caught loop" << "\n";
		Player * currPlayer = it->second;
		if (currPlayer->getPlayerNum() == currState->number) {
			uiCanvas->setBoostRatio((float)currPlayer->getDashCooldown() / PLAYER_DASH_COOLDOWN);
		}
		if (currPlayer->isCaught()) {
			if (currPlayer->getModelType() == ModelType::CAT) {
				uiCanvas->setVisible(uiCanvas->CAT_HAPPY, false);
				uiCanvas->setVisible(uiCanvas->CAT_JAIL, true);
			}
			if (currPlayer->getModelType() == ModelType::DOG) {
				uiCanvas->setVisible(uiCanvas->DOG_HAPPY, false);
				uiCanvas->setVisible(uiCanvas->DOG_JAIL, true);
			}
			if (currPlayer->getModelType() == ModelType::RACOON) {
				uiCanvas->setVisible(uiCanvas->RACCOON_HAPPY, false);
				uiCanvas->setVisible(uiCanvas->RACCOON_JAIL, true);
			}
		}
		else {
			if (currPlayer->getModelType() == ModelType::CAT) {
				uiCanvas->setVisible(uiCanvas->CAT_HAPPY, true);
				uiCanvas->setVisible(uiCanvas->CAT_JAIL, false);
			}
			if (currPlayer->getModelType() == ModelType::DOG) {
				uiCanvas->setVisible(uiCanvas->DOG_HAPPY, true);
				uiCanvas->setVisible(uiCanvas->DOG_JAIL, false);
			}
			if (currPlayer->getModelType() == ModelType::RACOON) {
				uiCanvas->setVisible(uiCanvas->RACCOON_HAPPY, true);
				uiCanvas->setVisible(uiCanvas->RACCOON_JAIL, false);
			}
		}
	}

	//check if current user is holding item
	Player * currPlayer = players[currState->id];
	//items held by chef are the icons for animals

	if (currPlayer->getDashCooldown() > 0) {
		uiCanvas->setVisible(UICanvas::PROMPT_BOOST, false);
	}
	if (currPlayer->isChef()) {
		uiCanvas->setBoostVisible(false);
		uiCanvas->setVisible(UICanvas::PROMPT_CHANGE_VIEW, false);
		if (players.find(currPlayer->getCaughtAnimalId()) != players.end()
			&& players[currPlayer->getCaughtAnimalId()]->getModelType() == ModelType::CAT) {
			uiCanvas->setItemChef(UICanvas::CAT_ITEM);
		}
		else if (players.find(currPlayer->getCaughtAnimalId()) != players.end()
			&& players[currPlayer->getCaughtAnimalId()]->getModelType() == ModelType::DOG) {
			uiCanvas->setItemChef(UICanvas::DOG_ITEM);
		}
		else if (players.find(currPlayer->getCaughtAnimalId()) != players.end()
			&& players[currPlayer->getCaughtAnimalId()]->getModelType() == ModelType::RACOON) {
			uiCanvas->setItemChef(UICanvas::RACCOON_ITEM);
		}
		else {

			uiCanvas->removeItems();
		}

		if (currPlayer->isChef() && players.find(currPlayer->getCaughtAnimalId()) == players.end()) {
			uiCanvas->setVisible(UICanvas::PROMPT_SWING_NET, true);
		}
		else {
			uiCanvas->setVisible(UICanvas::PROMPT_SWING_NET, false);
		}
	}
	else {
		uiCanvas->setBoostVisible(true);

		//disable prompts unless item is held
		uiCanvas->setVisible(UICanvas::PROMPT_GREEN_BANANA, false);
		uiCanvas->setVisible(UICanvas::PROMPT_YELLOW_BANANA, false);
		uiCanvas->setVisible(UICanvas::PROMPT_BLACK_BANANA, false);
		uiCanvas->setVisible(UICanvas::PROMPT_APPLE, false);
		uiCanvas->setVisible(UICanvas::PROMPT_ORANGE, false);
		uiCanvas->setVisible(UICanvas::PROMPT_KEY, false);
		bool hasItem = true;

		if (currPlayer->getInventory() == ItemModelType::toiletPaper) {
			uiCanvas->setItem(UICanvas::TOILET_PAPER_ITEM);
			uiCanvas->setVisible(UICanvas::PROMPT_KEY, true);
		}
		else if (currPlayer->getInventory() == ItemModelType::rope) {
			uiCanvas->setItem(UICanvas::ROPE_ITEM);
			uiCanvas->setVisible(UICanvas::PROMPT_KEY, true);
		}
		else if (currPlayer->getInventory() == ItemModelType::plunger) {
			uiCanvas->setItem(UICanvas::PLUNGER_ITEM);
			uiCanvas->setVisible(UICanvas::PROMPT_KEY, true);
		}
		else if (currPlayer->getInventory() == ItemModelType::key1) {
			uiCanvas->setItem(UICanvas::YELLOW_KEY_ITEM);
			uiCanvas->setVisible(UICanvas::PROMPT_KEY, true);
		}
		else if (currPlayer->getInventory() == ItemModelType::key2) {
			uiCanvas->setItem(UICanvas::BLUE_KEY_ITEM);
			uiCanvas->setVisible(UICanvas::PROMPT_KEY, true);
		}
		else if (currPlayer->getInventory() == ItemModelType::key3) {
			uiCanvas->setItem(UICanvas::GREEN_KEY_ITEM);
			uiCanvas->setVisible(UICanvas::PROMPT_KEY, true);
		}
		else if (currPlayer->getInventory() == ItemModelType::screwdriver1) {
			uiCanvas->setItem(UICanvas::YELLOW_SCREWDRIVER_ITEM);
			uiCanvas->setVisible(UICanvas::PROMPT_KEY, true);
		}
		else if (currPlayer->getInventory() == ItemModelType::screwdriver2) {
			uiCanvas->setItem(UICanvas::GREEN_SCREWDRIVER_ITEM);
			uiCanvas->setVisible(UICanvas::PROMPT_KEY, true);
		}
		else if (currPlayer->getInventory() == ItemModelType::screwdriver3) {
			uiCanvas->setItem(UICanvas::RED_SCREWDRIVER_ITEM);
			uiCanvas->setVisible(UICanvas::PROMPT_KEY, true);
		}
		else if (currPlayer->getInventory() == ItemModelType::cake) {
			uiCanvas->setItem(UICanvas::CAKE_ITEM);
		}
		else if (currPlayer->getInventory() == ItemModelType::apple) {
			uiCanvas->setItem(UICanvas::APPLE_ITEM);

			uiCanvas->setVisible(UICanvas::PROMPT_APPLE, true);
		}
		else if (currPlayer->getInventory() == ItemModelType::orange) {
			uiCanvas->setItem(UICanvas::ORANGE_ITEM);
			uiCanvas->setVisible(UICanvas::PROMPT_ORANGE, true);
		}
		else if (currPlayer->getInventory() == ItemModelType::bananaPerfect) {
			uiCanvas->setItem(UICanvas::BANANA_YELLOW_ITEM);
			uiCanvas->setVisible(UICanvas::PROMPT_YELLOW_BANANA, true);
		}
		else if (currPlayer->getInventory() == ItemModelType::bananaGreen) {
			uiCanvas->setItem(UICanvas::BANANA_GREEN_ITEM);
			uiCanvas->setVisible(UICanvas::PROMPT_GREEN_BANANA, true);
		}
		else if (currPlayer->getInventory() == ItemModelType::bananaVeryRipe) {
			uiCanvas->setItem(UICanvas::BANANA_BLACK_ITEM);
			uiCanvas->setVisible(UICanvas::PROMPT_BLACK_BANANA, true);
		}
		else {
			hasItem = false;
			uiCanvas->removeItems();
		}

		//set goals
		Location tempLoc;

		//set prompts
		if (gameData->getAdjacentJailTile(currPlayer->getLocation(),currPlayer->getFacingDirection(),tempLoc) != nullptr &&
			gameData->getAdjacentJailTile(currPlayer->getLocation(), currPlayer->getFacingDirection(), tempLoc)->getCapturedAnimal() != -1 && !currPlayer->isChef()) {
			uiCanvas->setVisible(UICanvas::PROMPT_JAIL_RESCUE, true);
		}
		else {
			uiCanvas->setVisible(UICanvas::PROMPT_JAIL_RESCUE, false);
		}

		if (gameData->getJailTile(currPlayer->getLocation()) != nullptr && !currPlayer->isChef()) {
			uiCanvas->setVisible(UICanvas::PROMPT_CHANGE_VIEW, true);
			uiCanvas->setVisible(UICanvas::PROMPT_JAIL_RESCUE, false);
		}
		else {
			uiCanvas->setVisible(UICanvas::PROMPT_CHANGE_VIEW, false);
		}

		//cake prompt check
		if (gameData->getTile(currPlayer->getLocation())->getTileType() == TileType::GATE &&
			gameData->getGateTile(currPlayer->getLocation())->getKeyProgress() == 3 && 
			gameData->getGateTile(currPlayer->getLocation())->getCurrentConstructTime() >= TIME_TO_CONSTRUCT_GATE){
			uiCanvas->setVisible(UICanvas::PROMPT_GET_CAKE, true);
			uiCanvas->setVisible(UICanvas::PROMPT_BUILD_EXIT, false);
			uiCanvas->setVisible(UICanvas::PROMPT_DROP_ITEM, false);
		}
		else if (gameData->getTile(currPlayer->getLocation())->getTileType() == TileType::GATE &&
			gameData->getGateTile(currPlayer->getLocation())->getKeyProgress() == 3) {
			uiCanvas->setVisible(UICanvas::PROMPT_GET_CAKE, false);
			uiCanvas->setVisible(UICanvas::PROMPT_BUILD_EXIT, true);
			uiCanvas->setVisible(UICanvas::PROMPT_DROP_ITEM, false);
		}
		else {
			uiCanvas->setVisible(UICanvas::PROMPT_GET_CAKE, false);
			uiCanvas->setVisible(UICanvas::PROMPT_BUILD_EXIT, false);
		}

		uiCanvas->setVisible(UICanvas::PROMPT_SWING_NET, false);
		if (gameData->getTile(currPlayer->getLocation())->getTileType() == TileType::BOX && !currPlayer->isChef() &&
			gameData->getBoxTile(currPlayer->getLocation())->hasBox() && !hasItem)
		{
			uiCanvas->setVisible(UICanvas::PROMPT_BOX_SEARCH, true);
		}
		else {
			uiCanvas->setVisible(UICanvas::PROMPT_BOX_SEARCH, false);
		}


	}


}

void InGameGraphicsEngine::IdleCallback()
{
	if (!sharedClient)
		return;

	/* TODO: waiting for server implementation */

	/*if (clock() - elapsedTime > 1000.0 / 60)
	{
		elapsedTime = clock();*/
		resetIdempotentFlush();
		SendPackets();
		sharedClient->update();
		const auto gameData = sharedClient->getGameData();
		if (gameData) {
			updateBoxVisibility();

			// Prevent a jailed animal from watching the a chef
			if (iAmJailed()) {
				// Why does this need fixing?
				auto networkPlayerFollowing = gameData->getPlayer(cameraClientId);
				if (!networkPlayerFollowing || networkPlayerFollowing->isChef()) {
					toggleJailCam();
				}
			}

#ifdef DUMMY_ID
			bool playersChanged = (players.size() != gameData->players.size() + 1);
#else
			bool playersChanged = (players.size() != gameData->players.size());
#endif
			const bool tilesChanged = (floorArray.size() != gameData->clientTileLayout.size());

			const auto &playersMap = sharedClient->getGameData()->players;
			for (auto &state : players) {
#ifdef DUMMY_ID
				if (state.id == DUMMY_ID) {
					continue;
				}
#endif

				if (playersMap.count(state.id) <= 0) {
					playersChanged = true;
					break;
				}

				const auto player = playersMap.at(state.id);
				const unsigned geomIdx = static_cast<unsigned>(player->getModelType());
				if (geomIdx != state.geometryIdx) {
					// instead of setting playersChanged lets just change this one player
					idempotentFlush();
					state.setGeometryIdx(geomIdx);
				}
			}

			if (tilesChanged) {
				idempotentFlush();
				reloadMap();
			}
			else {
				idempotentFlush();
				resetItems();
			}

			if (playersChanged) {
				idempotentFlush();
				ReloadPlayers();
			}
		}
		MovePlayers();
		//DummyMovePlayer();
		//server->update();
		//raccoonModel->Rotate(glm::pi<float>()/1000, 0.0f, 1.0f, 0.0f);

		updateUIElements(gameData);
		Player *networkPlayer = gameData->getPlayer(sharedClient->getMyID());
		if (networkPlayer) {
			ingame_light_radius = networkPlayer->isChef() ? (float)gameData->chefVision : networkPlayer->getVisionRadius();
			fog->setFogDistance(ingame_light_radius);
		}
	//}

}


static void UpdateAndDrawPlayer(PlayerState &state)
{
	auto &model = playerModels[state.geometryIdx];
	auto networkPlayer = sharedClient->getGameData()->getPlayer(state.id);
	if (!networkPlayer) return;

	bool notHidden = true;

	if (networkPlayer->isCaught()) {
		// Search for any chef that is carrying this player
		Player *networkChef = nullptr;
		for (const auto pair : sharedClient->getGameData()->getAllPlayers()) {
			Player *networkChef = pair.second;
			if (!networkChef->isChef()) continue;

			if (networkChef->hasCaughtAnimal() && networkChef->getCaughtAnimalId() == state.id) {
				// We are being carried, so draw nothing
				notHidden = false;
				break;
			}
		}
	}

	Action action = networkPlayer ? networkPlayer->getAction() : Action::NONE;
	auto inventory = networkPlayer->getInventory();
	Geometry *playerGeometry = nullptr;

	PowerUp powerupActive = networkPlayer ? networkPlayer->getPowerUp() : PowerUp::NONE;

#ifdef DEBUG_CARRY
	inventory = static_cast<ItemModelType>(fake_carried_idx);
#endif
	state.building = false;
	switch (action) {
	case Action::OPEN_BOX:
	{
		// hide the un-animated box
		int x = (int)floorf(state.position.x / (TILE_SCALE * TILE_STRIDE));
		int z = (int)floorf(state.position.z / (TILE_SCALE * TILE_STRIDE));
		const auto &envObj = envObjs[z][x];
		auto transform = envObj.transform;
		transform->hidden = true;

		// show the animated box
		auto boxGeometry = itemModels[static_cast<unsigned>(ItemModelType::box)].geometry;
		unsigned boxIdx = (unsigned)(networkPlayer->getPlayerNum() - 1) % MAX_PLAYERS;
		animatedBoxObjects[boxIdx]->Update(true);
		animatedBoxObjects[boxIdx]->Draw(objShaderProgram, &V, &P, transform->getOffset() * boxGeometry->t);

		// Turn towards the box
		float centerX = (x + 0.5f) * TILE_SCALE * TILE_STRIDE;
		float centerZ = (z + 0.5f) * TILE_SCALE * TILE_STRIDE;
		state.setTargetAngle(glm::atan(centerX - state.position.x, centerZ - state.position.z));

		//for particle effects
		state.building = true;
		state.buildPosition = transform->getOffset() * glm::vec4(0, 0, 0, 1);

		// fall through on purpose (missing break is not a mistake)
	}
	case Action::CONSTRUCT_GATE: {
	}
	case Action::UNLOCK_JAIL:
	{
	}
	case Action::SWING_NET:
		model.getActionObject()->Update(true);
		playerGeometry = model.getActionGeometry();
		if (networkPlayer->getModelType() == ModelType::CHEF) {
			inventory = ItemModelType::net;
		}
		break;

	case Action::NONE:
	default:
		if (networkPlayer->getModelType() == ModelType::CHEF && networkPlayer->hasCaughtAnimal()) {
			if (state.moving) {
				model.getCarryObject()->Update(true);
				playerGeometry = model.getCarryGeometry();
			}
			else {
				model.getIdleCarryObject()->Update(true);
				playerGeometry = model.getIdleCarryGeometry();
			}
		}
		else if (inventory != ItemModelType::EMPTY) {
			if (state.moving) {
				model.getCarryObject()->Update(true);
				playerGeometry = model.getCarryGeometry();
			}
			else {
				model.getIdleCarryObject()->Update(true);
				playerGeometry = model.getIdleCarryGeometry();
			}
		}
		else {
			if (state.moving) {
				model.getWalkObject()->Update(true);
				playerGeometry = model.getwalkGeometry();
			}
			else {
				model.getIdleObject()->Update(true);
				playerGeometry = model.getIdleGeometry();
			}
		}
		break;
	}
	state.reversed = false;
	state.instantSearch = false;
	state.isBear = false;
	state.movingSpeed = 0;
	//set states to show auras for powerups
	switch (powerupActive) {
	case PowerUp::GHOST:
		state.movingSpeed = 1;
		break;
	case PowerUp::CHEF_SLOW:
		break;
	case PowerUp::FLASH:
		state.flashedRecently = 6;
		break;
	case PowerUp::CHEF_REVERSE:

		state.movingSpeed = 0;
		break;
	case PowerUp::INSTA_SEARCH:
		state.instantSearch = true;
		break;
	default:
		state.flashedRecently = 0;
		break;
	}
	if (sharedClient->getGameData()->getReverseChef()) {
		std::cerr << "ChefBlind gotten" << std::endl;
	}
	if (sharedClient->getGameData()->getSlowChef()) {
		std::cerr << "ChefSlow gotten" << std::endl;
	}
	if (networkPlayer->getModelType() == ModelType::CHEF && sharedClient->getGameData()->reverseChef) {
		state.reversed = true;
	}
	if (networkPlayer->getModelType() == ModelType::CHEF && sharedClient->getGameData()->slowChef) {
		state.movingSpeed = -1;
	}
	if (networkPlayer->getBearBuff()) {
		state.isBear = true;
	}
	if (notHidden) {
		playerGeometry->draw(V, P, state.transform);
	}


	// Prepare to draw a special copy of an item (either carried or thrown)
	Geometry *inventoryGeometry = nullptr;
	glm::mat4 inventoryMat;

	if (inventory != ItemModelType::EMPTY) {
		// Prepare to draw a carried item

		auto playerSettings = playerModels[state.geometryIdx].settings;
		const auto &itemModel = itemModels[static_cast<unsigned>(inventory)];

		glm::vec3 modelTranslate = itemModel.settings->translate;
		modelTranslate.x *= TILE_STRIDE * TILE_SCALE;
		modelTranslate.y *= TILE_LEVEL_OFFSET * TILE_SCALE;
		modelTranslate.z *= TILE_STRIDE * TILE_SCALE;

		modelTranslate += playerSettings->carryPosition + itemModel.settings->carryTranslate;

		const auto scale = glm::scale(glm::translate(state.transform, modelTranslate), glm::vec3(itemModel.settings->scale));
		const auto modelAngles = itemModel.settings->rotation + itemModel.settings->carryRotate;
		inventoryMat = glm::rotate(scale, modelAngles.y, glm::vec3(0.f, 1.f, 0.f));
		inventoryMat = glm::rotate(inventoryMat, modelAngles.x, glm::vec3(1.f, 0.f, 0.f));
		inventoryMat = glm::rotate(inventoryMat, modelAngles.z, glm::vec3(0.f, 0.f, 1.f));
		inventoryGeometry = itemModel.geometry;

		// The thrown item code will need this later
		state.previousInventory = inventory;
		state.inventoryTransform = inventoryMat;
	}
	else if (state.previousInventory != ItemModelType::EMPTY) {
		// Prepare to draw a thrown item

		// Start the throw animation by setting carryStartTime or otherwise cancel it by unsetting previousInventory
		// depending on if the item actually was just thrown
#define INVENTORY_ANIMATION_DURATION 0.275
#define INVENTORY_GRAVITY 85
		if (!state.animatingInventory) {
			state.carryStopLoc = Location(state.position.x, state.position.y, state.position.z);
			state.carryStopTime = glfwGetTime();
			auto tile = sharedClient->getGameData()->getKeyDropTile(state.carryStopLoc);
			bool isKey = state.previousInventory >= ItemModelType::key1 && state.previousInventory <= ItemModelType::screwdriver3;
			state.animatingInventory = (tile != nullptr && tile->getItem() != state.previousInventory && isKey);
			if (!state.animatingInventory) {
				state.previousInventory = ItemModelType::EMPTY;
			}
		}

		// If the animation wasn't cancelled, calculate projectile position
		if (state.animatingInventory) {
			int x = (int)floorf(state.carryStopLoc.getX() / (TILE_SCALE * TILE_STRIDE));
			int z = (int)floorf(state.carryStopLoc.getZ() / (TILE_SCALE * TILE_STRIDE));
			glm::vec3 targetPosition = glm::vec3(envObjs[z][x].transform->getOffset()[3]);
			targetPosition.y += 6.f;
			auto sourcePosition = glm::vec3(state.inventoryTransform[3]);

			auto animationFraction = (float)((glfwGetTime() - state.carryStopTime) / INVENTORY_ANIMATION_DURATION);
			auto animatedPosition = sourcePosition + (targetPosition - sourcePosition) * animationFraction;
			animatedPosition.y += 0.5f * INVENTORY_GRAVITY * animationFraction - 0.5f * INVENTORY_GRAVITY * animationFraction * animationFraction;

			inventoryMat = state.inventoryTransform;
			inventoryMat[3] = glm::vec4(animatedPosition, inventoryMat[3][3]);

			inventoryGeometry = itemModels[static_cast<unsigned>(state.previousInventory)].geometry;
		}

		// Check if the animation has finished
		if (glfwGetTime() >= state.carryStopTime + INVENTORY_ANIMATION_DURATION) {
			state.animatingInventory = false;
			state.previousInventory = ItemModelType::EMPTY;
		}
	}

	if (notHidden && inventoryGeometry) {
		inventoryGeometry->draw(V, P, inventoryMat);
	}
}

void printPoint(float x, float y, const glm::mat4 &transform)
{
	auto newPoint = transform * glm::vec4(x, y, 0.f, 0.f);
	printf("(%f, %f)", newPoint.x, newPoint.y);
}

static void DrawPings(const glm::mat4 &viewMat)
{
	glUseProgram(colorizeShader);
	auto uAlpha = glGetUniformLocation(colorizeShader, "alpha");
	auto uFillColor = glGetUniformLocation(colorizeShader, "fillColor");

	double now = glfwGetTime();

	for (const auto &pair : allPings) {
		const auto &ping = pair.second;
		if (now > ping.startTime + MINIMAP_PING_DURATION) {
			continue;
		}

		float pingPhase = (float)((now - ping.startTime) / MINIMAP_PING_DURATION);

		glUniform3f(uFillColor, ping.fillColor.r, ping.fillColor.g, ping.fillColor.b);
		glUniform1f(uAlpha, (1.f - pingPhase) * (1.f - pingPhase));

		const float radius = MINIMAP_PING_BEGIN_RADIUS + pingPhase * (MINIMAP_PING_FINAL_RADIUS - MINIMAP_PING_BEGIN_RADIUS);
		const auto transform = glm::scale(glm::translate(identityMat, ping.position), glm::vec3(radius, radius, 1.f));

		mapPingObject->Draw(colorizeShader, &identityMat, &orthoP, viewMat * transform);
	}
}

static void GarbageCollectPings()
{
	auto gameData = sharedClient->getGameData();
	double now = glfwGetTime();

	double angerFraction = gameData->getChefAnger() / (double)CHEF_MAX_ANGER;
	double chefInterval = MINIMAP_PING_INTERVAL_NO_ANGER + angerFraction * (MINIMAP_PING_INTERVAL_FULL_ANGER - MINIMAP_PING_INTERVAL_NO_ANGER);

	vector<int> toRemoveIDs;
	for (const auto &pair : allPings) {
		const auto &ping = pair.second;
		Player *networkPlayer = gameData ? gameData->getPlayer(pair.first) : nullptr;

		double garbageCollectAge = MINIMAP_PING_DURATION;
		if (networkPlayer) {
			garbageCollectAge = networkPlayer->isChef() ? chefInterval : ping.interval;
		}

		if (now > ping.startTime + garbageCollectAge) {
			toRemoveIDs.push_back(pair.first);
		}
	}

	for (auto toRemove : toRemoveIDs) {
		allPings.erase(toRemove);
	}
}

static void DrawMinimap()
{
	GameData *gameData = nullptr;
	if (!sharedClient || !(gameData = sharedClient->getGameData()) || !solidColorObject) {
		return;
	}

	const auto &tileLayout = gameData->getTileLayout();
	int tileHeight = (int)tileLayout.size();
	int tileWidth = 0;
	for (int z = 0; z < tileHeight; z++) {
		if (tileWidth < (int)tileLayout[z].size())
			tileWidth = (int)tileLayout[z].size();
	}

	const float mapWidth = tileWidth * TILE_SIZE_SERVER;
	const float mapHeight = tileHeight * TILE_SIZE_SERVER;

	// First calculate transformation from (x, z, 0) map coords to the minimap rectangle on the screen

	float screenHeight = (windowWidth / (float)windowHeight) * (MINIMAP_SCREEN_WIDTH * tileHeight) / tileWidth;

	const auto position = glm::translate(identityMat, glm::vec3(-1.f + 0.5f * MINIMAP_SCREEN_WIDTH + MINIMAP_MARGIN_LEFT, -1.f + 0.5f * screenHeight + MINIMAP_MARGIN_BOTTOM, 0.f));
	const auto rescale = glm::scale(position, glm::vec3(-MINIMAP_SCREEN_WIDTH / mapWidth, screenHeight / mapHeight, 1.f));
	const auto viewMat = glm::translate(rescale, glm::vec3(mapWidth * -0.5f, mapHeight * -0.5f, 0.f));

	glUseProgram(solidColorShader);
	const auto uFillColor = glGetUniformLocation(solidColorShader, "fillColor");

	// Convenience method to draw a rectangle
	auto fillRect = [&](float centerX, float centerZ, float width, float height, float r, float g, float b, float a) {
		const auto transform = glm::scale(glm::translate(identityMat, glm::vec3(centerX, centerZ, 0.f)), glm::vec3(0.5f * width, 0.5f * height, 1.f));

		glUniform4f(uFillColor, r, g, b, a * MINIMAP_ALPHA);
		solidColorObject->Draw(solidColorShader, &identityMat, &orthoP, viewMat * transform);
	};

	// Draw the background
	fillRect(mapWidth * 0.5f, mapHeight * 0.5f, mapWidth, mapHeight, MINIMAP_BG_COLOR);

	int z;

	// Draw gates and jails
	z = 0;
	for (const auto &row : tileLayout) {
		int x = 0;
		for (auto tile : row) {
			if (tile) {
				const float centerX = (x + 0.5f) * TILE_SIZE_SERVER;
				const float centerZ = (z + 0.5f) * TILE_SIZE_SERVER;

				switch (tile->getTileType())
				{
				case TileType::GATE:
					fillRect(centerX, centerZ, TILE_SIZE_SERVER, TILE_SIZE_SERVER, MINIMAP_GATE_COLOR);
					break;
				case TileType::JAIL:
					fillRect(centerX, centerZ, TILE_SIZE_SERVER, TILE_SIZE_SERVER, MINIMAP_JAIL_COLOR);
					break;
				}
			}
			++x;
		}
		++z;
	}

	// Draw north walls (horizontal along the tops of tiles)
	z = 0;
	for (const auto &row : northWalls) {
		int x = 0;
		for (auto wall : row) {
			if (wall) {
				const float centerX = (x + 0.5f) * TILE_SIZE_SERVER;
				const float centerZ = z * TILE_SIZE_SERVER;
				fillRect(centerX, centerZ, TILE_SIZE_SERVER + MINIMAP_WALL_WIDTH, MINIMAP_WALL_WIDTH, MINIMAP_WALL_COLOR);
			}
			++x;
		}
		++z;
	}

	// Draw west walls (vertical along the left sides of tiles)
	z = 0;
	for (const auto &row : westWalls) {
		int x = 0;
		for (auto wall : row) {
			if (wall) {
				const float centerX = x * TILE_SIZE_SERVER;
				const float centerZ = (z + 0.5f) * TILE_SIZE_SERVER;
				fillRect(centerX, centerZ, MINIMAP_WALL_WIDTH, TILE_SIZE_SERVER + MINIMAP_WALL_WIDTH, MINIMAP_WALL_COLOR);
			}
			++x;
		}
		++z;
	}

	// Draw dots for players you have permission to see the position of
	auto networkPlayerMe = gameData->getPlayer(sharedClient->getMyID());
	for (const auto &state : players) {
		auto networkPlayer = gameData->getPlayer(state.id);
		if (!networkPlayer) {
			continue;
		}
		
		bool isChefState = state.geometryIdx == static_cast<unsigned>(ModelType::CHEF);

		if (state.id != sharedClient->getMyID()) {
			if (networkPlayerMe->isChef()) {
				if (!allPings.count(state.id)) {
					allPings.emplace(state.id, MapPing(state.position, glm::vec3(MINIMAP_ANIMAL_PING_COLOR)));
				}
				// The chef sees only those in jail and not being carried
				if (!networkPlayer->isCaught() || (networkPlayerMe->hasCaughtAnimal() && networkPlayerMe->getCaughtAnimalId() == state.id)) {
					continue;
				}

				if (networkPlayer->getAction() == Action::UNLOCK_JAIL) {
					if (!allPings.count(state.id) || allPings.at(state.id).interval != MINIMAP_JAIL_PING_INTERVAL) {
						allPings[state.id] = MapPing(state.position, glm::vec3(MINIMAP_JAIL_PING_COLOR), MINIMAP_JAIL_PING_INTERVAL);
					}
				}
			}
			else {
				// Animals see only other animals
				if (isChefState) {
					if (!allPings.count(state.id)) {
						allPings.emplace(make_pair(state.id, MapPing(state.position, glm::vec3(MINIMAP_CHEF_PING_COLOR))));
					}
					continue;
				}
				/*else if (networkPlayer->isCaught()) {
					// Animals can't see caught animals
					continue;
				}*/
			}
		}

		if (state.geometryIdx == static_cast<unsigned>(ModelType::CHEF)) {
			fillRect(state.position.x, state.position.z, 10.f, 10.f, MINIMAP_CHEF_DOT_COLOR);
		}
		else if (state.id == sharedClient->getMyID()) {
			fillRect(state.position.x, state.position.z, 10.f, 10.f, MINIMAP_ANIMAL_ME_DOT_COLOR);
		}
		else {
			fillRect(state.position.x, state.position.z, 10.f, 10.f, MINIMAP_ANIMAL_OTHER_DOT_COLOR);
		}
	}

	// Handle pings
	DrawPings(viewMat);
	GarbageCollectPings();
}

static void DrawSandTile(int x, int z)
{
	glm::vec3 position((x + 0.5f) * TILE_SIZE_SERVER, -5.f, (z + 0.5f) * TILE_SIZE_SERVER);

	auto rotate = glm::rotate(glm::translate(identityMat, position), glm::half_pi<float>(), glm::vec3(1.f, 0.f, 0.f));
	glm::mat4 modelTrans = glm::scale(rotate, glm::vec3(TILE_SCALE));
	sandModel->Draw(objShaderProgram, &V, &P, modelTrans);
}

void DisplayCallback(GLFWwindow* window)
{
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glViewport(0, 0, windowWidth, windowHeight);
	//glDepthMask(GL_TRUE);

	skybox->Draw(P, V);

	//glUseProgram(objShaderProgram);
	light->draw(objShaderProgram, &cam_pos, cam_look_at);
	fog->draw(objShaderProgram, P * V * glm::vec4(ingame_light_center, 1.0f));

	if (floorTransform) {
		floorTransform->draw(V, P, glm::mat4(1.0));
	}

	/*if (envObjsTransform) {
		envObjsTransform->draw(V, P, glm::mat4(1.0));
	}*/
	for (int z = 0; z < envObjs.size(); z++) {
		for (int x = 0; x < envObjs[z].size(); x++) {
			const auto &envObj = envObjs[z][x];
			if (envObj.transform && !envObj.transform->hidden) {
				if (envObj.textureOverride) {
					itemModels[static_cast<unsigned>(envObj.type)].object->Draw(objShaderProgram, &V, &P, envObj.transform->getOffset(), envObj.textureOverride);
				}
				else {
					envObj.transform->draw(V, P, identityMat);
				}
			}
		}
	}

	if (allItemsTransform) {
		allItemsTransform->draw(V, P, glm::mat4(1.0));
	}

	for (const auto &state : players) {
		dustSpawner[state.number-1]->draw(particleShaderProgram, &V, &P, cam_pos,
			state.position - glm::vec3(0, 3.0f, 0), (state.moving && state.movingSpeed == 0 && !state.instantSearch && !state.isBear));
		bearSpawner[state.number - 1]->draw(particleShaderProgram, &V, &P, cam_pos,
			state.position - glm::vec3(0, 1.0f, 0), (state.moving && state.movingSpeed == 0 && !state.instantSearch && state.isBear));
		speedSpawner[state.number-1]->draw(particleShaderProgram, &V, &P, cam_pos,
			state.position - glm::vec3(0, 3.0f, 0), (state.moving && state.movingSpeed == 1 && !state.instantSearch));
		slowSpawner[state.number-1]->draw(particleShaderProgram, &V, &P, cam_pos,
			state.position - glm::vec3(0, 3.0f, 0), (state.moving && state.movingSpeed == -1 && !state.instantSearch));
		//buildSpawner->draw(particleShaderProgram, &V, &P, cam_pos,
			//state.buildPosition + ((float)(rand() % 1000 - 1000) / 100.0f) *
			//glm::vec3(1.0f, 0, 0.5f) + glm::vec3(3.5f, 1, 3), state.building);
		flashSpawner[state.number-1]->draw(particleShaderProgram, &V, &P, cam_pos,
			state.position, state.flashedRecently > 0);
		blindSpawner[state.number-1]->draw(particleShaderProgram, &V, &P, cam_pos,
			state.position + glm::vec3(0,25.0f,0), state.reversed);
		searchSpawner[state.number-1]->draw(particleShaderProgram, &V, &P, cam_pos,
			state.position - glm::vec3(0, 3.0f, 0), state.instantSearch);
	}

	glEnable(GL_DEPTH_TEST);
	glUseProgram(objShaderProgram);

	// Draw the players
	if (sharedClient && sharedClient->getGameData()) {
		for (auto &state : players) {
			UpdateAndDrawPlayer(state);
		}
		//particleEffects for powerups

	}

#define SAND_MARGIN_X 6
#define SAND_MARGIN_Z 6

	// Don't fog the sand
	fog->setFogDistance(ingame_light_radius * 2.f);
	fog->draw(objShaderProgram, P * V * glm::vec4(ingame_light_center, 1.0f));

	int mapWidth = 0;
	for (unsigned z = 0; z < (int)floorArray.size(); z++) {
		if (mapWidth < (int)floorArray[z].size())
			mapWidth = (int)floorArray[z].size();
	}

	for (int z = -SAND_MARGIN_Z; z < 0; z++) {
		for (int x = -SAND_MARGIN_X; x < mapWidth + SAND_MARGIN_X; x++) {
			DrawSandTile(x, z);
		}
	}
	for (int z = 0; z < (int)floorArray.size(); z++) {
		for (int x = -SAND_MARGIN_X; x < 0; x++) {
			DrawSandTile(x, z);
		}
		for (int x = (int)floorArray[z].size(); x < mapWidth + SAND_MARGIN_X; x++) {
			DrawSandTile(x, z);
		}
	}
	for (int z = (int)floorArray.size(); z <= (int)floorArray.size() + SAND_MARGIN_Z; z++) {
		for (int x = -SAND_MARGIN_X; x < mapWidth + SAND_MARGIN_X; x++) {
			DrawSandTile(x, z);
		}
	}

	for (const auto &customObj : customPositionedObjects) {
		if (customObj.type == ItemModelType::warrenBear) {
			fog->setFogDistance(1000);
			fog->draw(objShaderProgram, P * V * glm::vec4(ingame_light_center, 1.0f));
			light->toggleNormalShading();
			light->draw(objShaderProgram, &cam_pos, cam_look_at);
		}

		const auto &model = itemModels[static_cast<unsigned>(customObj.type)];
		const auto translate = glm::translate(identityMat, glm::vec3(customObj.x, customObj.y, customObj.z));
		glm::mat4 modelMat = glm::scale(glm::rotate(translate, model.settings->rotation.y, glm::vec3(0.f, 1.f, 0.f)), model.settings->scale);
		model.geometry->draw(V, P, modelMat);

		if (customObj.type == ItemModelType::warrenBear) {
			light->toggleNormalShading();
			light->draw(objShaderProgram, &cam_pos, cam_look_at);
		}
	}

	// Draw the starting prompt
#define START_PROMPT_DURATION 10.0
#define START_PROMPT_FADEOUT 1.0
#define START_PROMPT_PERIOD 0.5f
	double now = glfwGetTime();
	Player *networkPlayer = nullptr;
	if (now >= mainLoopBeginTime && now < mainLoopBeginTime + START_PROMPT_DURATION
		&& twoDeeShader && animalStartingPrompt && chefStartingPrompt
		&& sharedClient && sharedClient->getGameData()
		&& ( networkPlayer = sharedClient->getGameData()->getPlayer(sharedClient->getMyID()) ) ) {
		
		FBXObject *message = networkPlayer->isChef() ? chefStartingPrompt : animalStartingPrompt;

		float scale = 1.f - (sinf((float)now * (glm::two_pi<float>() / START_PROMPT_PERIOD)) + 1.f) * 0.01f;
		auto mat = glm::scale(identityMat, glm::vec3(scale));

		float msgAlpha = 1.f;
		float alphaPhase = (float)(now - (mainLoopBeginTime + START_PROMPT_DURATION - START_PROMPT_FADEOUT) / START_PROMPT_FADEOUT);
		if (alphaPhase >= 0.f && alphaPhase <= 1.f) {
			msgAlpha = 1.f - alphaPhase;
		}

		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		
		glUseProgram(twoDeeShader);
		auto uAlpha = glGetUniformLocation(twoDeeShader, "alpha");
		glUniform1f(uAlpha, msgAlpha * 0.88f);
		message->Draw(twoDeeShader, &identityMat, &orthoP, mat);
	}

	if (uiCanvas) {
		uiCanvas->draw(&V, &P, glm::mat4(1.0));
	}

	DrawMinimap();

	//raccoonModel->Draw(objShaderProgram, &V, &P);
		//playerModels[RACCOON_IDX].geometry->draw(V, P, glm::mat4(1.0));

}

void UpdateView() {
	V = glm::lookAt(cam_pos, cam_look_at, cam_up);
}

// Mapping a 2D cursor position to 3D space!
glm::vec3 TrackballMapping(double x, double y, int width, int height) {
	float trackballX, trackballY, distance;
	glm::vec3 toReturn;
	trackballX = (float)(((2.0f * x) - width) / width);
	trackballY = (float)((height - (2.0f * y)) / height);
	distance = sqrtf((trackballX * trackballX) + (trackballY * trackballY));
	distance = (distance < 1.0f) ? distance : 1.0f;
	toReturn = glm::vec3(trackballX, trackballY, sqrtf(1.001f - (distance*distance)));
	return glm::normalize(toReturn);
}

void TrackballRotation(float rotationAngle, glm::vec3 rotationAxis) {
	glm::vec3 newRotAxis = glm::vec4(rotationAxis, 1.0f) * V;
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotationAngle, newRotAxis);
	glm::vec4 temp = rotationMatrix * glm::vec4(cam_pos - cam_look_at, 1.0f);
	cam_pos = glm::vec3(temp[0], temp[1], temp[2]) + cam_look_at;
	UpdateView();
}

void LoadModels()
{
	// Load models
	cout << "Loading models...\n";

	using namespace std::chrono;
	auto modelLoadingStart = high_resolution_clock::now();

	thread playerLoadingThreads[sizeof(playerModelSettings) / sizeof(playerModelSettings[0])];
	thread actionLoadingThread;

	unsigned threadIdx = 0;
	for (auto &setting : playerModelSettings) {
		playerLoadingThreads[threadIdx] = thread([&]() {
			auto &model = playerModels[static_cast<unsigned>(setting.modelType)];
			glm::mat4 transform = glm::scale(glm::translate(glm::mat4(1.f), setting.translate), glm::vec3(setting.scale));

			cout << "\tloading " << setting.title << " walk" << endl;
			model.settings = &setting;
			model.walkObject = new FBXObject(setting.walkModelPath, setting.walkTexturePath, setting.attachSkel, setting.walkAnimMultiplier, -1, false);
			model.walkGeometry = new Geometry(model.walkObject, objShaderProgram);
			model.walkGeometry->t = transform;
			model.walkObject->SetClipRadius(PLAYER_MDL_CLIP_RADIUS);
			cout << "\tfinished loading " << setting.title << " walk" << endl;

			if (setting.carryModelPath || setting.carryTexturePath) {
				cout << "\tloading " << setting.title << " carry" << endl;
				model.carryObject = new FBXObject(setting.getCarryModelPath(), setting.getCarryTexturePath(), setting.attachSkel, setting.carryAnimMultiplier, -1, false);
				model.carryGeometry = new Geometry(model.carryObject, objShaderProgram);
				model.carryGeometry->t = transform;
				model.carryObject->SetClipRadius(PLAYER_MDL_CLIP_RADIUS);
				cout << "\tfinished loading " << setting.title << " carry" << endl;
			}
			if (setting.idleModelPath || setting.idleTexturePath) {
				cout << "\tloading " << setting.title << " idle" << endl;
				model.idleObject = new FBXObject(setting.getIdleModelPath(), setting.getIdleTexturePath(), setting.attachSkel, setting.idleAnimMultiplier, -1, false);
				model.idleGeometry = new Geometry(model.idleObject, objShaderProgram);
				model.idleGeometry->t = transform;
				model.idleObject->SetClipRadius(PLAYER_MDL_CLIP_RADIUS);
				cout << "\tfinished loading " << setting.title << " idle" << endl;
			}
			if (setting.idleCarryModelPath || setting.idleCarryTexturePath) {
				cout << "\tloading " << setting.title << " idle carry" << endl;
				model.idleCarryObject = new FBXObject(setting.getIdleCarryModelPath(), setting.getIdleCarryTexturePath(), setting.attachSkel, setting.idleCarryAnimMultiplier, -1, false);
				model.idleCarryGeometry = new Geometry(model.idleCarryObject, objShaderProgram);
				model.idleCarryGeometry->t = transform;
				model.idleCarryObject->SetClipRadius(PLAYER_MDL_CLIP_RADIUS);
				cout << "\tfinished loading " << setting.title << " idle carry" << endl;
			}

		});

		++threadIdx;
	}

	actionLoadingThread = thread([&]() {
		for (auto &setting : playerModelSettings) {
			auto &model = playerModels[static_cast<unsigned>(setting.modelType)];
			glm::mat4 transform = glm::scale(glm::translate(glm::mat4(1.f), setting.translate), glm::vec3(setting.scale));
			
			if (setting.actionModelPath || setting.actionTexturePath) {
				cout << "\tloading " << setting.title << " action" << endl;
				model.actionObject = new FBXObject(setting.getActionModelPath(), setting.getActionTexturePath(), setting.attachSkel, setting.actionAnimMultiplier, -1, false);
				model.actionGeometry = new Geometry(model.actionObject, objShaderProgram);
				model.actionGeometry->t = transform;
				model.actionObject->SetClipRadius(PLAYER_MDL_CLIP_RADIUS);
				cout << "\tfinished loading " << setting.title << " action" << endl;
			}
		}
	});

	size_t largestIdx = 0;
	for (auto &setting : itemModelSettings) {
		size_t idx = static_cast<size_t>(setting.id);
		if (largestIdx < idx) {
			largestIdx = idx;
		}
	}

	itemModels.resize(largestIdx + 1);

	thread itemLoadingThread([&]() {
		for (auto &setting : itemModelSettings) {

			cout << "\tloading " << setting.name << endl;

			bool animated = false;
			switch (setting.id) {
			case ItemModelType::door:
			case ItemModelType::vent:
			case ItemModelType::window:
				animated = ENABLE_GATE_ANIMATION;
				break;

			default:
				break;
			}

			auto &m = itemModels[static_cast<size_t>(setting.id)];
			m.settings = &setting;
			m.object = new FBXObject(setting.modelPath, setting.texturePath, animated, 1.0f, 0, false);
			m.geometry = new Geometry(m.object, objShaderProgram);
			m.object->SetClipRadius(ITEM_MDL_CLIP_RADIUS);

			cout << "\tfinished loading " << setting.name << endl;
		}
	});

	cout << "\tloading " << "tile" << endl;
	tileModel = new FBXObject(TILE_MDL_PATH, TILE_TEX_PATH, false, 1.0f, -1, false);
	//tileModel->SetClipRadius(ITEM_MDL_CLIP_RADIUS);

	cout << "\tloading " << "wall" << endl;
	wallModel = new FBXObject(WALL_MDL_PATH, WALL_TEX_PATH, false, 1.0f, -1, false);
	//wallModel->SetClipRadius(ITEM_MDL_CLIP_RADIUS);

	cout << "\tloading " << "sand" << endl;
	sandModel = new FBXObject(CANVAS_MDL_PATH, SAND_TEX_PATH, false, 1.0f, -1, false, GL_LINEAR);
	//sandModel->SetClipRadius(200.f);

	tileGeometry = new Geometry(tileModel, objShaderProgram);
	wallGeometry = new Geometry(wallModel, objShaderProgram);

	cout << "\t" << MAX_PLAYERS << " copies of animated box\n";
	for (unsigned i = 0; i < MAX_PLAYERS; ++i) {
		animatedBoxObjects[i] = new FBXObject(BOX_SEARCH_MDL_PATH, TEXTURES_PATH "box.png", true, 1.0f, -1, false);
		animatedBoxObjects[i]->SetClipRadius(ITEM_MDL_CLIP_RADIUS);
	}

	itemLoadingThread.join();
	for (auto &t : playerLoadingThreads) {
		t.join();
	}
	actionLoadingThread.join();

	auto modelLoadingEnd = high_resolution_clock::now();
	std::chrono::duration<float> modelLoadingDuration = modelLoadingEnd - modelLoadingStart;
	cout << "\tfinished loading models in " << modelLoadingDuration.count() << " seconds\n";
}


InGameGraphicsEngine::InGameGraphicsEngine()
{
	calledMainLoopBegin = false;
	fullyLoaded = false;
	quit = false;
	needsRenderingSetup = true;
	objShaderProgram = 0;
	uiShaderProgram = 0;
	light = nullptr;
	fog = nullptr;
	uiCanvas = nullptr;
}


static void deleteSpawners(ParticleSpawner *array[MAX_PLAYERS]) {
	for (size_t i = 0; i < MAX_PLAYERS; ++i) {
		auto &spawner = array[i];
		if (spawner) {
			delete spawner;
			spawner = nullptr;
		}
	}
}

InGameGraphicsEngine::~InGameGraphicsEngine()
{
	deleteSpawners(dustSpawner);
	deleteSpawners(speedSpawner);
	deleteSpawners(searchSpawner);
	deleteSpawners(slowSpawner);
	deleteSpawners(blindSpawner);
	deleteSpawners(flashSpawner);
	deleteSpawners(bearSpawner);
}

void InGameGraphicsEngine::StartLoading()  // may launch a thread and return immediately
{
	//soundSystem = new SoundSystem();

	/*// If no audio device is plugged in, sound system will refuse to create sounds
	if (!(soundSystem->shouldIgnoreSound())) {
		fprintf(stdout, "createSound before: sound_toilet=%p\n", sound_toilet);
		soundSystem->createSoundEffect(&sound_toilet, SOUNDS_TOILET);
		fprintf(stdout, "createSound after: sound_toilet=%p\n", sound_toilet);
		soundSystem->createSoundEffect(&sound_search_item, SOUNDS_SEARCH_ITEM);
	}*/

	// load the shader program
	if (!objShaderProgram) {
		objShaderProgram = LoadShaders(OBJ_VERT_SHADER_PATH, OBJ_FRAG_SHADER_PATH);
	}
	if (!uiShaderProgram) {
		uiShaderProgram = LoadShaders(UI_VERT_SHADER_PATH, UI_FRAG_SHADER_PATH);
	}
	if (!particleShaderProgram) {
		particleShaderProgram = LoadShaders(PARTICLE_VERT_SHADER_PATH, PARTICLE_FRAG_SHADER_PATH);
	}

	needsRenderingSetup = true;

	auto finish = [](bool *finishedFlag) {
		LoadModels();
		*finishedFlag = true;
	};

	thread t(finish, &fullyLoaded);
	t.detach();
}

void InGameGraphicsEngine::CleanUp()
{
	players.clear();

	if (twoDeeShader) {
		TwoDeeGraphicsEngine::releaseShader();
		twoDeeShader = 0;
	}

	if (animalStartingPrompt) {
		delete animalStartingPrompt;
		animalStartingPrompt = nullptr;
	}

	if (chefStartingPrompt) {
		delete chefStartingPrompt;
		chefStartingPrompt = nullptr;
	}

	for (auto &model : itemModels) {
		if (model.object) delete model.object;
		if (model.geometry) delete model.geometry;
	}

	itemModels.clear();

	for (auto &model : playerModels) {
		model.deallocMembers();
	}

	if (tileModel)        delete tileModel;
	if (wallModel)        delete wallModel;
	if (sandModel)        delete sandModel;
	if (tileGeometry)     delete tileGeometry;
	if (wallGeometry)     delete wallGeometry;
	if (light)            delete light;
	if (solidColorObject) delete solidColorObject;
	if (mapPingObject)    delete mapPingObject;
	if (skybox)           delete skybox;

	deallocFloor();

	if (floorTransform)   delete floorTransform;

	glDeleteProgram(objShaderProgram);
	glDeleteProgram(solidColorShader);
	glDeleteProgram(colorizeShader);
}

void InGameGraphicsEngine::MainLoopBegin()
{
	if (!light) {
		light = new DirLight();
		//light->toggleNormalShading();
	}
	if (!fog) {
		fog = new FogGenerator(CHEF_FOG_DISTANCE);
	}

	if (!skybox) {
		skybox = new Skybox();
	}

	if (!twoDeeShader) {
		twoDeeShader = TwoDeeGraphicsEngine::retainShader();
	}

#ifndef DEBUG_NO_UI
	if (!uiCanvas) {
		cout << "Loading UICanvas... ";
		using namespace std::chrono;
		auto setupStart = high_resolution_clock::now();
		uiCanvas = new UICanvas(uiShaderProgram);
		auto setupEnd = high_resolution_clock::now();
		std::chrono::duration<float> setupDuration = setupEnd - setupStart;
		cout << "done constructing UICanvas in " << setupDuration.count() << " seconds\n";
	}

	if (!animalStartingPrompt) {
		animalStartingPrompt = createObjectForTexture(ANIMAL_START_PROMPT_PATH);
	}
	if (!chefStartingPrompt) {
		chefStartingPrompt = createObjectForTexture(CHEF_START_PROMPT_PATH);
	}
#endif

	if (!solidColorShader) {
		solidColorShader = LoadShaders("./solidcolor.vert", "./solidcolor.frag");
	}

	if (!colorizeShader) {
		colorizeShader = LoadShaders("./passthrough.vert", "./colorize.frag");
	}

	if (!solidColorObject) {
		solidColorObject = new FBXObject(CANVAS_MDL_PATH, nullptr, false, 1.0f, -1);
		solidColorObject->SetDepthTest(false);
	}

	if (!mapPingObject) {
		mapPingObject = createObjectForTexture(CANVAS_PATH "mapPing.png");
	}

	calledMainLoopBegin = true;
	quit = false;

	if (needsRenderingSetup) {
		using namespace std::chrono;
		auto setupStart = high_resolution_clock::now();

		cout << "Calling RenderingSetup() on objects...\n ";

		cout << "\ttile... ";
		tileModel->RenderingSetup();
		cout << "done.\n";

		cout << "\twall... ";
		wallModel->RenderingSetup();
		cout << "done\n";

		cout << "\tsand... ";
		sandModel->RenderingSetup();
		cout << "done\n";

		cout << "\tPlayer models... ";
		for (auto &model : playerModels) {
			if (model.walkObject)
				model.walkObject->RenderingSetup();
			if (model.carryObject)
				model.carryObject->RenderingSetup();
			if (model.actionObject)
				model.actionObject->RenderingSetup();
			if (model.idleObject)
				model.idleObject->RenderingSetup();
			if (model.idleCarryObject)
				model.idleCarryObject->RenderingSetup();
		}
		cout << "done\n";

		cout << "\tanimated boxes... ";
		for (auto box : animatedBoxObjects) {
			if (box)
				box->RenderingSetup();
		}
		cout << "done\n";

		for (auto &model : itemModels) {
			if (model.object) {
				cout << "\t" << model.settings->name << "... ";
				model.object->RenderingSetup();
				cout << "done\n";
			}
		}

		//particle setup
		for (int i = 0; i < MAX_PLAYERS; i++) {
			dustSpawner[i] = new ParticleSpawner(DUST_PARTICLE_TEX, glm::vec3(0, 1.0f, 0), 1.0, 125,3.3f);
			flashSpawner[i] = new ParticleSpawner(FLASH_PARTICLE_TEX, glm::vec3(0, 10.0f, 0), 3.0f);
			speedSpawner[i] = new ParticleSpawner(SPEED_PARTICLE_TEX, glm::vec3(0, 2.5f, 0), 1.0, 125, 3.3f);
			slowSpawner[i] = new ParticleSpawner(SLOW_PARTICLE_TEX, glm::vec3(0, 0.0f, 0), 1.0, 125, 3.3f);
			//buildSpawner = new ParticleSpawner(BUILD_PARTICLE_TEX, glm::vec3(0, 10.0f, 2.0f), 0.7f);
			blindSpawner[i] = new ParticleSpawner(BLIND_PARTICLE_TEX, glm::vec3(0, 0.0f, 0), 0.5f, 255);
			searchSpawner[i] = new ParticleSpawner(SEARCH_PARTICLE_TEX, glm::vec3(0, -4.0f, 0), 2.0f);
			bearSpawner[i] = new ParticleSpawner(BEAR_PARTICLE_TEX, glm::vec3(0, 1.0f, 0), 1.75f, 255, 5.0f);
		}

		auto setupEnd = high_resolution_clock::now();
		std::chrono::duration<float> setupDuration = setupEnd - setupStart;
		cout << "\tfinished RenderingSetup() in " << setupDuration.count() << " seconds\n";

		needsRenderingSetup = false;
	}

	InitCamera(glm::vec3(0.f));

	// Set clear color
	glClearColor(0.05f, 0.8f, 0.85f, 1.0f);

	const auto gameData = sharedClient->getGameData();
	gameData->startGameClock();

	mainLoopBeginTime = glfwGetTime();

	// Reset everything
	deallocFloor();
	players.clear();
	allPings.clear();
}

void InGameGraphicsEngine::MainLoopEnd()
{
	calledMainLoopBegin = false;
	quit = false;
}

void InGameGraphicsEngine::ResizeCallback(GLFWwindow* window, int newWidth, int newHeight)
{
	// Set the viewport size. This is the only matrix that OpenGL maintains for us in modern OpenGL!
	glViewport(0, 0, windowWidth, windowHeight);

	if (windowHeight > 0)
	{
		P = glm::perspective(45.0f, (float)windowWidth / (float)windowHeight, 0.1f, 4000.0f);

		orthoP = glm::ortho(-1.f, 1.f, -1.f, 1.f);

		UpdateView();
	}
}

void InGameGraphicsEngine::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Check for a key press
	if (action == GLFW_PRESS)
	{
		// TODO: Remove the following if block; meant just to test sound
		// and show how to use the soundSystem
		/*if (!(soundSystem->shouldIgnoreSound())) {
			fprintf(stdout, "before playSound: %d\n", sound_toilet);
			// soundSystem->playSound(sound_toilet);
			soundSystem->playSoundNoOverlap(sound_toilet);
		}*/

		// Check if escape was pressed
		if (key == GLFW_KEY_ESCAPE)
		{
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		if (key == GLFW_KEY_F10) {
			sharedClient->sendPackets(INCREMENT_ANGER_EVENT);
		}
		if (key == GLFW_KEY_F11) {
			sharedClient->sendPackets(OPEN_ALL_BOXES_EVENT);
		}
		if (key == GLFW_KEY_F9) {
			sharedClient->sendPackets(UNLOCK_ALL_GATES_EVENT);
		}
		if (key == GLFW_KEY_F5) {
			sharedClient->sendPackets(REMOVE_WALLS_EVENT);
		}

		if (key == GLFW_KEY_UP) {
			directions |= DirectionBitmask::northSide;
			thirdPersonView = false;
		}

		if (key == GLFW_KEY_DOWN) {
			directions |= DirectionBitmask::southSide;
			thirdPersonView = false;
		}

		if (key == GLFW_KEY_LEFT) {
			directions |= DirectionBitmask::westSide;
			thirdPersonView = false;
		}

		if (key == GLFW_KEY_RIGHT) {
			directions |= DirectionBitmask::eastSide;
			thirdPersonView = false;
		}

		if (key == GLFW_KEY_SPACE) {
			if (iAmJailed()) {
				toggleJailCam();
			}
			else {
				// interact key press
				sharedClient->sendPackets(INTERACT_EVENT);
			}
		}

		if (key == GLFW_KEY_D) {
			sharedClient->sendPackets(DROP_EVENT);
		}
		if (key == GLFW_KEY_F) {
			sharedClient->sendPackets(POWERUP_EVENT);
		}
		if (key == GLFW_KEY_H) {
			// interact key press
			sharedClient->sendPackets(HIDE_EVENT);
		}
		if (key == GLFW_KEY_0) {
			sharedClient->sendPackets(SELECT0_EVENT);
		}
		if (key == GLFW_KEY_1) {
			sharedClient->sendPackets(SELECT1_EVENT);
		}
		if (key == GLFW_KEY_2) {
			sharedClient->sendPackets(SELECT2_EVENT);
		}
		if (key == GLFW_KEY_3) {
			sharedClient->sendPackets(SELECT3_EVENT);
		}
		if (key == GLFW_KEY_4) {
			sharedClient->sendPackets(SELECT4_EVENT);
		}
		if (key == GLFW_KEY_P) {
			sharedClient->sendPackets(RESET_EVENT);
		}
		if (key == GLFW_KEY_I) {
#ifdef DEBUG_CARRY
			fake_carried_idx++;
			if (fake_carried_idx >= itemModels.size()) {
				fake_carried_idx = 0;
			}
			if (!itemModels[fake_carried_idx].settings) {
				fake_carried_idx++;
			}
#endif
		}
		if (key == GLFW_KEY_TAB) {
			(void)toggleClient();
		}
		if (key == GLFW_KEY_V) {
			thirdPersonView = !thirdPersonView;
		}
		if (key == GLFW_KEY_S) {
			sharedClient->sendPackets(PLAYER_DASH_EVENT);
		}

	}
	else if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_UP) {
			directions &= ~DirectionBitmask::northSide;
		}

		if (key == GLFW_KEY_DOWN) {
			directions &= ~DirectionBitmask::southSide;
		}

		if (key == GLFW_KEY_LEFT) {
			directions &= ~DirectionBitmask::westSide;
		}

		if (key == GLFW_KEY_RIGHT) {
			directions &= ~DirectionBitmask::eastSide;
		}

		if (key == GLFW_KEY_SPACE) {
			// interact key release
			sharedClient->sendPackets(RELEASE_EVENT);
		}

		if (key == GLFW_KEY_F) {
			// interact key release
			sharedClient->sendPackets(RELEASE_EVENT);
		}
	}
}

void InGameGraphicsEngine::CursorPosCallback(GLFWwindow * window, double xpos, double ypos)
{
	if (mouseRotation) {
		float rotationAngle, dotProduct;
		glm::vec3 prevTrackballPos, currTrackballPos, rotationAxis;
		prevTrackballPos = TrackballMapping(prevPos[0], prevPos[1], windowWidth, windowHeight);
		currTrackballPos = TrackballMapping(xpos, ypos, windowWidth, windowHeight);
		rotationAxis = glm::cross(prevTrackballPos, currTrackballPos);
		dotProduct = glm::dot(prevTrackballPos, currTrackballPos);
		rotationAngle = glm::acos(dotProduct);
		if (!isnan(rotationAngle)) {
			TrackballRotation(scaleUpAngle*rotationAngle, rotationAxis);
		}
		prevPos = glm::vec2(xpos, ypos);
	}
}

void InGameGraphicsEngine::MouseWheelCallback(GLFWwindow * window, double xoffset, double yoffset)
{
	glm::vec3 new_cam_pos;
	float distance;
	if (yoffset < 0) {
		new_cam_pos = (cam_pos - cam_look_at) * scaleMouseWheel + cam_look_at;
	}
	else {
		new_cam_pos = (cam_pos - cam_look_at) / scaleMouseWheel + cam_look_at;
	}
	distance = glm::distance(new_cam_pos, glm::vec3(0.0f, 0.0f, 0.0f));
	if (distance < 1400.0f && distance > 2.0f) {
		cam_pos = glm::vec3(new_cam_pos);
		UpdateView();
	}
}

void InGameGraphicsEngine::MouseButtonCallback(GLFWwindow * window, int button, int action, int mods)
{
	double x, y;
	// Rotate the model about the center of the graphics window when the left mouse button is pressed and the mouse is moved
	if ((button == GLFW_MOUSE_BUTTON_RIGHT) && (action == GLFW_PRESS)) {
		glfwGetCursorPos(window, &x, &y);
		prevPos = glm::vec2(x, y);
		mouseRotation = true;
	}
	else if ((button == GLFW_MOUSE_BUTTON_RIGHT) && (action == GLFW_RELEASE)) {
		mouseRotation = false;
	}
}

void InGameGraphicsEngine::MainLoopCallback(GLFWwindow * window)
{
	// Main render display callback. Rendering of objects is done here.
	DisplayCallback(window);
	// Idle callback. Updating objects, etc. can be done here.
	IdleCallback();
}

bool InGameGraphicsEngine::ShouldFadeout()
{
	return quit;
}
