#include "InGameGraphicsEngine.h"

#include "../../network/ClientGame.h"

#include "ItemModelType.h"

#include "Transform.h"
#include "Geometry.h"
#include "SoundSystem.h"
#include "DirLight.h"
#include "FogGenerator.h"
#include "FBXObject.h"
#include "UIObject.h"

#include <thread>

#define RACCOON_WALK_MDL_PATH   (ANIMATIONS_PATH "raccoonWalk.dae")
#define RACCOON_CARRY_MDL_PATH  (ANIMATIONS_PATH "raccoonWalkWithItem.dae")
#define RACCOON_SEARCH_MDL_PATH (ANIMATIONS_PATH "raccoonSearch.dae")
#define RACCOON_MDL_PATH        (MODELS_PATH "raccoon.fbx")
#define RACCOON_TEX_PATH        (TEXTURES_PATH "raccoon.ppm")

#define CAT_WALK_MDL_PATH       (ANIMATIONS_PATH "catWalk.dae")
#define CAT_CARRY_MDL_PATH      (ANIMATIONS_PATH "catWalkWithItem.dae")
#define CAT_SEARCH_MDL_PATH     (ANIMATIONS_PATH "catSearch.dae")
#define CAT_MDL_PATH            (MODELS_PATH "cat.fbx")
#define CAT_TEX_PATH            (TEXTURES_PATH "cat.ppm")

#define DOG_WALK_MDL_PATH       (ANIMATIONS_PATH "dogWalk.dae")
#define DOG_CARRY_MDL_PATH      (ANIMATIONS_PATH "dogWalkWithItem.dae")
#define DOG_SEARCH_MDL_PATH     (ANIMATIONS_PATH "dogSearch.dae")
#define DOG_MDL_PATH            (MODELS_PATH "doggo.fbx")
#define DOG_TEX_PATH            (TEXTURES_PATH "doggo.ppm")

#define CHEF_WALK_PATH    (ANIMATIONS_PATH "chefWalk.dae")
#define CHEF_MDL_PATH     (MODELS_PATH "chef.fbx")
#define CHEF_TEX_PATH     (TEXTURES_PATH "chef.ppm")

#define TILE_MDL_PATH     (MODELS_PATH "tile.fbx")
#define TILE_TEX_PATH     (TEXTURES_PATH "tile.png")

#define CHEF_FOG_DISTANCE 85.0f
#define RACCOON_FOG_DISTANCE 160
#define WALL_MDL_PATH     (MODELS_PATH "wall.fbx")
#define WALL_TEX_PATH     (TEXTURES_PATH "wall.png")

//#define CANVAS_MDL_PATH   (MODELS_PATH "canvas.fbx")
//#define CANVAS_TEX_PATH   (TEXTURES_PATH "canvas.ppm");

#define OBJ_VERT_SHADER_PATH "./obj_shader.vert"
#define OBJ_FRAG_SHADER_PATH "./obj_shader.frag"

#define UI_VERT_SHADER_PATH "./ui_shader.vert"
#define UI_FRAG_SHADER_PATH "./ui_shader.frag"

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


// Uncomment to render a repeating pattern of all environment objects
// This is good for debugging scale/positioning/rendering
//#define ENV_OBJS_DEMO

#define TILE_HEIGHT_ADJUST -2.f
#define TILE_SCALE 10.f          /* overall scale of the entire floor. (TILE_SCALE * TILE_STRIDE) should match server tile size, which is currently 20 */
#define TILE_LEVEL_OFFSET 1.0f   /* from first floor to second */
#define TILE_STRIDE 2.0f         /* difference in position from one tile to the next */

// Uncomment to render an additional dummy Chef that mirrors the player's movements
// #define DUMMY_ID -4000

// Uncomment to force the camera to any other player
// #define DEBUG_CAUGHT

#define DELETE_IF_NEEDED(x) \
	if (x) {\
		delete x;\
		x = nullptr;\
	}

#define CHEF_IDX     (static_cast<unsigned>(ModelType::CHEF))
#define RACCOON_IDX  (static_cast<unsigned>(ModelType::RACOON))
#define CAT_IDX      (static_cast<unsigned>(ModelType::CAT))
#define DOG_IDX      (static_cast<unsigned>(ModelType::DOG))

static const struct PlayerModelSettings {
	const char *walkModelPath;     // filesystem path to a model geometry file
	const char *walkTexturePath;   // filesystem path to a texture file
	int walkAnimIndex;             // index of the preferred animation (optional specification for weird cases)
	const char *carryModelPath;    // filesystem path to a model geometry file
	const char *carryTexturePath;  // filesystem path to a texture file
	int carryAnimIndex;            // index of the preferred animation (optional specification for weird cases)
	const char *actionModelPath;   // filesystem path to a model geometry file
	const char *actionTexturePath; // filesystem path to a texture file
	int actionAnimIndex;           // index of the preferred animation (optional specification for weird cases)
	const char *title;             // name for use in debug messages, maybe user-visible too
	const char *name;              // name for use in debug messages, maybe user-visible too
	ModelType modelType;           // A unique ID, like ModelType::CHEF
	bool attachSkel;               // true if animated with a skeleton
	float scale;                   // scale adjustment
	glm::vec3 translate;           // position adjustment

	inline const char * getWalkModelPath() const {
		return walkModelPath;
	}

	inline const char * getCarryModelPath() const {
		return carryModelPath ? carryModelPath : walkModelPath;
	}

	inline const char * getActionModelPath() const {
		return actionModelPath ? actionModelPath : walkModelPath;
	}

	inline const char * getwalkTexturePath() const {
		return walkTexturePath;
	}

	inline const char * getCarryTexturePath() const {
		return carryTexturePath ? carryTexturePath : walkTexturePath;
	}

	inline const char * getActionTexturePath() const {
		return actionTexturePath ? actionTexturePath : walkTexturePath;
	}
} playerModelSettings[] = {
	// walkModelPath          walkTexturePath    walkAnimIndex  carryModelPath           carryTexturePath  carryAnimIndex  actionModelPath           actionTexturePath  actionAnimIndex  title       name          modelType        attachSkel scale   translate
	{ CHEF_WALK_PATH,         CHEF_TEX_PATH,     -1,            nullptr,                 nullptr,          -1,             nullptr,                  nullptr,           -1,              "Chef",     "Cheoffrey",  ModelType::CHEF,    true,   1.f,    glm::vec3(0.f) },
	{ RACCOON_WALK_MDL_PATH,  RACCOON_TEX_PATH,  -1,            RACCOON_CARRY_MDL_PATH,  nullptr,          -1,             RACCOON_SEARCH_MDL_PATH,  nullptr,           -1,              "Raccoon",  "Hung",       ModelType::RACOON,  true,  0.5f,   glm::vec3(0.f, 4.0f, -1.2f) },
	{ CAT_WALK_MDL_PATH,      CAT_TEX_PATH,      -1,            CAT_CARRY_MDL_PATH,      nullptr,          -1,             CAT_SEARCH_MDL_PATH,      nullptr,           -1,              "Cat",      "Kate",       ModelType::CAT,     true,   1.f,    glm::vec3(0.f) },
	{ DOG_WALK_MDL_PATH,      DOG_TEX_PATH,      -1,            DOG_CARRY_MDL_PATH,      nullptr,          -1,             DOG_SEARCH_MDL_PATH,      nullptr,           -1,              "Dog",      "Richard",    ModelType::DOG,     true,   1.f,    glm::vec3(0.f) },
};

#define MDL_AND_TEX(m, t) MODELS_PATH m ".fbx", TEXTURES_PATH t ".png"
#define MDL_SAME_TEX(x) MDL_AND_TEX(x, x)
#define GLM_H_PI glm::half_pi<float>()
#define GLM_PI glm::pi<float>()

static const struct ItemModelSettings {
	const char *modelPath;       // filesystem path to a model geometry file
	const char *texturePath;     // filesystem path to a texture file
	const char *name;            // name for use in debug messages, maybe user-visible too
	ItemModelType id;            // A unique ID, like ItemModelType::apple
	float scale;                 // scale adjustment
	glm::vec3 translate;         // position adjustment
	glm::vec3 rotation;          // rotation
	bool wallRotate;             // auto-rotate away from any wall on this tile
} itemModelSettings[] = {
	// model and texture paths                      name                    id                               scale  translate                      rotate                            wallRotate
	{ MDL_SAME_TEX("apple"),                        "apple",                ItemModelType::apple,             1.f,  glm::vec3(0.f),                glm::vec3(0.f),                   false },
	{ MDL_AND_TEX("banana", "bananagreen"),         "green banana",         ItemModelType::bananaGreen,       1.f,  glm::vec3(0.f),                glm::vec3(0.f),                   false },
	{ MDL_AND_TEX("banana", "bananaperfect"),       "perfect banana",       ItemModelType::bananaPerfect,     1.f,  glm::vec3(0.f),                glm::vec3(0.f),                   false },
	{ MDL_AND_TEX("banana", "bananaveryveryripe"),  "very ripe banana",     ItemModelType::bananaVeryRipe,    1.f,  glm::vec3(0.f),                glm::vec3(0.f),                   false },
	{ MDL_SAME_TEX("box"),                          "box",                  ItemModelType::box,              1.5f,  glm::vec3(0.f),                glm::vec3(0.f),                   false },
	{ MDL_SAME_TEX("cake"),                         "cake",                 ItemModelType::cake,             0.6f,  glm::vec3(0.f),                glm::vec3(0.f, GLM_H_PI, 0.f),    false },
	{ MDL_SAME_TEX("cookingpot"),                   "cooking pot",          ItemModelType::cookingPot,        1.f,  glm::vec3(0.f),                glm::vec3(0.f),                   true  },
	{ MDL_SAME_TEX("door"),                         "door",                 ItemModelType::door,              1.f,  glm::vec3(0.f, 0.0f, -0.45f),  glm::vec3(0.f),                   true },
	{ MDL_SAME_TEX("fork"),                         "fork",                 ItemModelType::fork,              1.f,  glm::vec3(0.f),                glm::vec3(0.f),                   true },
	{ MDL_SAME_TEX("garbagebag"),                   "garbage bag",          ItemModelType::garbageBag,        1.f,  glm::vec3(0.f),                glm::vec3(0.f),                   true },
	{ MDL_SAME_TEX("jail"),                         "jail",                 ItemModelType::jail,             0.3f,  glm::vec3(0.f),                glm::vec3(0.f),                   false },
	{ MDL_AND_TEX("key", "key1"),                   "key #1",               ItemModelType::key1,             0.5f,  glm::vec3(0.f),                glm::vec3(GLM_H_PI, 0.f, 0.f),    false },
	{ MDL_AND_TEX("key", "key2"),                   "key #2",               ItemModelType::key2,             0.5f,  glm::vec3(0.f),                glm::vec3(GLM_H_PI, 0.f, 0.f),    false },
	{ MDL_AND_TEX("key", "key3"),                   "key #3",               ItemModelType::key3,             0.5f,  glm::vec3(0.f),                glm::vec3(GLM_H_PI, 0.f, 0.f),    false },
	{ MDL_AND_TEX("keydrop", "keydrop_bathroom"),   "bathroom key drop",    ItemModelType::keyDropBathroom,  2.5f,  glm::vec3(0.f, 0.0f, -0.375f), glm::vec3(0.f, -GLM_H_PI, 0.f),   true },
	{ MDL_AND_TEX("keydrop", "keydrop_frontexit"),  "front exit key drop",  ItemModelType::keyDropFrontExit, 2.5f,  glm::vec3(0.f, 0.0f, -0.375f), glm::vec3(0.f, -GLM_H_PI, 0.f),   true },
	{ MDL_AND_TEX("keydrop", "keydrop_vent"),       "vent key drop",        ItemModelType::keyDropVent,      2.5f,  glm::vec3(0.f, 0.0f, -0.375f), glm::vec3(0.f, -GLM_H_PI, 0.f),   true },
	{ MDL_SAME_TEX("knife"),                        "knife",                ItemModelType::knife,             1.f,  glm::vec3(0.f),                glm::vec3(0.f),                   true },
	{ MDL_SAME_TEX("orange"),                       "orange fruit",         ItemModelType::orange,            1.f,  glm::vec3(0.f),                glm::vec3(0.f),                   false },
	{ MDL_SAME_TEX("painting"),                     "wall painting",        ItemModelType::painting,         1.8f,  glm::vec3(0.f, 0.5f, -0.4f),   glm::vec3(0.f),                   true },
	{ MDL_SAME_TEX("pear"),                         "pear",                 ItemModelType::pear,              1.f,  glm::vec3(0.f),                glm::vec3(0.f),                   false },
	{ MDL_SAME_TEX("plate"),                        "plate",                ItemModelType::plate,             1.f,  glm::vec3(0.f),                glm::vec3(0.f),                   true },
	{ MDL_SAME_TEX("plunger"),                      "plunger",              ItemModelType::plunger,          0.7f,  glm::vec3(0.f),                glm::vec3(0.f),                   false },
	{ MDL_SAME_TEX("restaurantchair"),              "restaurant chair",     ItemModelType::restaurantChair,  0.7f,  glm::vec3(0.f),                glm::vec3(0.f),                   true },
	{ MDL_SAME_TEX("rope"),                         "rope",                 ItemModelType::rope,             1.5f,  glm::vec3(0.f),                glm::vec3(0.f),                   true },
	{ MDL_AND_TEX("screwdriver", "screwdriver1"),   "screwdriver #1",       ItemModelType::screwdriver1,   0.225f,  glm::vec3(0.f),                glm::vec3(0.f),                   false },
	{ MDL_AND_TEX("screwdriver", "screwdriver2"),   "screwdriver #2",       ItemModelType::screwdriver2,   0.225f,  glm::vec3(0.f),                glm::vec3(0.f),                   false },
	{ MDL_AND_TEX("screwdriver", "screwdriver3"),   "screwdriver #3",       ItemModelType::screwdriver3,   0.225f,  glm::vec3(0.f),                glm::vec3(0.f),                   false },
	{ MDL_SAME_TEX("stove"),                        "stove",                ItemModelType::stove,           1.45f,  glm::vec3(0.f, 0.f, -0.225f),  glm::vec3(0.f),                   true },
	{ MDL_SAME_TEX("toilet"),                       "toilet",               ItemModelType::toilet,           0.6f,  glm::vec3(0.f),                glm::vec3(0.f),                   true },
	{ MDL_SAME_TEX("toiletpaper"),                  "toilet paper",         ItemModelType::toiletPaper,      0.9f,  glm::vec3(0.f),                glm::vec3(0.f),                   false },
	{ MDL_SAME_TEX("vent"),                         "vent",                 ItemModelType::vent,             2.5f,  glm::vec3(0.f, 0.1f, -0.455f), glm::vec3(0.f),                   true },
	{ MDL_SAME_TEX("window"),                       "window",               ItemModelType::window,            1.f,  glm::vec3(0.f, 0.65f, -0.4f),  glm::vec3(0.f),                   true },
	{ MDL_SAME_TEX("table"),                        "table",                ItemModelType::table,            1.5f,  glm::vec3(0.f),                glm::vec3(0.f),                   true },
};

struct ItemModel {
	const ItemModelSettings *settings;
	FBXObject *object;
	Geometry *geometry;
};

static vector<ItemModel> itemModels;

static glm::mat4 P; // P for projection
static glm::mat4 orthoP; // P for 2d elements;
static glm::mat4 V; // V for view
static DirLight * light = nullptr;
static FogGenerator * fog = nullptr;

struct PlayerModel {
	const PlayerModelSettings *settings;
	FBXObject *walkObject;
	FBXObject *carryObject;
	FBXObject *actionObject;
	Geometry *walkGeometry;
	Geometry *carryGeometry;
	Geometry *actionGeometry;
	Transform *transform;

	inline FBXObject * getWalkObject() {
		return walkObject;
	}
	inline FBXObject * getCarryObject() {
		return carryObject ? carryObject : walkObject;
	}
	inline FBXObject * getActionObject() {
		return actionObject ? actionObject : walkObject;
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

	void deallocMembers() {
		DELETE_IF_NEEDED(walkObject);
		DELETE_IF_NEEDED(carryObject);
		DELETE_IF_NEEDED(actionObject);
		DELETE_IF_NEEDED(walkGeometry);
		DELETE_IF_NEEDED(carryGeometry);
		DELETE_IF_NEEDED(actionGeometry);
		DELETE_IF_NEEDED(transform);
	}
};

struct AnimatedItem {
	Transform *transform = nullptr;
	Geometry *animating = nullptr;
	ItemModelType type = ItemModelType::EMPTY;
};

static PlayerModel playerModels[NUM_PLAYER_MODEL_TYPES] = { nullptr };

static FBXObject * tileModel = nullptr;
static FBXObject * wallModel = nullptr;

static UICanvas * uiCanvas = nullptr;
static GLuint objShaderProgram;
static GLuint uiShaderProgram;

static GLuint uiTexture;

static Geometry * tileGeometry;
static Geometry * wallGeometry;
static vector<vector<Transform *>> floorArray;
static vector<vector<Transform *>> northWalls;
static vector<vector<Transform *>> westWalls;
static vector<vector<Transform *>> envObjs;
static vector<vector<AnimatedItem>> itemTransforms;

static SoundSystem * soundSystem;
static Sound * sound_door;
static Sound * sound_door_unlock;
static Sound * sound_found_item;
static Sound * sound_net;
static Sound * sound_raccoon_up;
static Sound * sound_raccoon_down;
static Sound * sound_search_item;
static Sound * sound_toilet;
static Sound * sound_vent_screw;
static Sound * sound_window;
static Sound * sound_yay;


extern ClientGame * sharedClient;

static int elapsedTime = 0;
static int directions = 0;

struct PlayerState {
	Transform *transform;       // for position and rotation
	float targetAngle;          // the angle it's trying to animate to
	float angle;                // angle it's currently facing (should be approaching targetAngle)
	glm::vec3 position;         // position, important for faking targetAngle above
	int id;                     // the player ID from the server
	unsigned geometryIdx;       // index into playerGeometry
	char moving;				// bool indicating whether or not the model should animate

	PlayerState() : position(0.f) {
		angle = targetAngle = 0.f;
		geometryIdx = 0;
		transform = nullptr;
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

		const auto location = p->getLocation();
		position.x = location.getX();
		position.y = location.getY();
		position.z = location.getZ();

		setGeometryIdx(static_cast<unsigned>(p->getModelType()));

		moving = 0;
	}

	~PlayerState() {
		if (transform && transform->decrementRefCount()) {
			delete transform;
		}
	}

	PlayerState(const PlayerState &other) {
		transform = other.transform;
		if (transform) {
			transform->incrementRefCount();
		}

		targetAngle = other.targetAngle;
		angle = other.angle;
		position = other.position;
		id = other.id;
		geometryIdx = other.geometryIdx;
		moving = other.moving;
	}

	PlayerState &operator=(const PlayerState &other) {
		if (transform && transform->decrementRefCount()) {
			delete transform;
		}

		transform = other.transform;
		if (transform) {
			transform->incrementRefCount();
		}

		targetAngle = other.targetAngle;
		angle = other.angle;
		position = other.position;
		id = other.id;
		geometryIdx = other.geometryIdx;

		return *this;
	}

	void setGeometryIdx(unsigned geomIdx) {
		geometryIdx = geomIdx;

		auto translate = glm::translate(glm::mat4(1.f), position);
		auto transformMat = glm::rotate(translate, angle, glm::vec3(0.f, 1.f, 0.f));
		if (transform) {
			transform->setOffset(transformMat);
			transform->removeAllChildren();
		}
		else {
			transform = new Transform(transformMat);
		}
		transform->addChild(playerModels[geometryIdx].transform);
	}
};

Transform * root = nullptr;
Transform * allPlayersNode = nullptr;
std::vector<PlayerState> players;
Transform * floorTransform = nullptr;
Transform * envObjsTransform = nullptr;
Transform * allItemsTransform = nullptr;

// Default camera parameters
glm::vec3 cam_pos(45.0f, 60.0f, 45.0f);          // e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);         // d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);              // up | What orientation "up" is
const glm::vec3 cam_angle(-10.f, 50.f, -30.f);  // camera's preferred offset from cam_look_at

glm::vec3 light_center = glm::vec3(0.f);

bool mouseRotation = false;
glm::vec2 prevPos = glm::vec2(0.0f, 0.0f);
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
bool iAmCaught();


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

	allPlayersNode->removeAllChildren();

	const auto &playersMap = sharedClient->getGameData()->players;
	const int myId = sharedClient->getMyID();

	for (const auto &pair : playersMap) {
		const auto id = pair.first;
		const Player *p = pair.second;

		players.emplace_back(id, p);

		const auto state = &players[players.size() - 1];
		allPlayersNode->addChild(state->transform);
	}

#ifdef DUMMY_ID
	// add a dummy player
	players.emplace_back(DUMMY_ID, CHEF_IDX);
	const auto state = &players[players.size() - 1];
	allPlayersNode->addChild(state->transform);
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
		root->addChild(envObjsTransform);
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

			if (objIdx == 0) {
				row[x] = nullptr;
				continue;
			}
#endif

			ItemModelType modelType = static_cast<ItemModelType>(objIdx);
			if (modelType == ItemModelType::EMPTY || !itemModels[objIdx].settings) {
				row[x] = nullptr;
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

			auto scale = glm::scale(glm::translate(rotate, modelTranslate), glm::vec3(settings.scale));

			auto modelAngles = settings.rotation;
			auto modelRotate = glm::rotate(scale, modelAngles.y, glm::vec3(0.f, 1.f, 0.f));
			modelRotate = glm::rotate(modelRotate, modelAngles.x, glm::vec3(1.f, 0.f, 0.f));
			modelRotate = glm::rotate(modelRotate, modelAngles.z, glm::vec3(0.f, 0.f, 1.f));

			row[x] = new Transform(modelRotate);
			row[x]->addChild(itemModels[objIdx].geometry);
			envObjsTransform->addChild(row[x]);
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
		root->addChild(allItemsTransform);
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

			if (tile->getTileType() == TileType::OBJECT) {
				if (((ObjectTile *)tile)->getModel() == ItemModelType::table) {
					tileTranslate.y += 9.25f;
				}
			}

			auto tileTranslateMat = glm::translate(glm::mat4(1.f), tileTranslate);
			auto rotate = glm::rotate(tileTranslateMat, angle, glm::vec3(0.f, 1.f, 0.f));

			glm::vec3 modelTranslate = settings.translate;
			modelTranslate.x *= TILE_STRIDE * TILE_SCALE;
			modelTranslate.y *= TILE_LEVEL_OFFSET * TILE_SCALE;
			modelTranslate.z *= TILE_STRIDE * TILE_SCALE;

			auto scale = glm::scale(glm::translate(rotate, modelTranslate), glm::vec3(settings.scale));

#define ITEM_ROTATION_PERIOD 5.25
			auto t = glfwGetTime();
			float timeAngle = fmod(t, ITEM_ROTATION_PERIOD) * glm::two_pi<double>() / ITEM_ROTATION_PERIOD;

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

void updateBoxVisibility()
{
	if (!sharedClient || !sharedClient->getGameData())
		return;

	const auto &tileLayout = sharedClient->getGameData()->clientTileLayout;

	if (!tileLayout.size()) {
		return;
	}

	unsigned y = 0;
	for (auto &row : envObjs) {
		unsigned x = 0;
		for (auto tile : row) {
			if (tile && tileLayout[y][x]->getTileType() == TileType::BOX) {
				tile->hidden = !((BoxTile *)tileLayout[y][x])->hasBox();
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
		root->addChild(floorTransform);
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
				int height = 1;
				/*if (z == 0 || z != clippedZ) {
					height = sharedClient->heights[clippedZ][x];
				} else {
					height = (sharedClient->heights[z - 1][x] + sharedClient->heights[z][x]) / 2;
				}*/
				float y = TILE_STRIDE * 0.9f + (height / 2) * TILE_LEVEL_OFFSET;

				// translate to the edge between tiles
				row[x] = new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(x * TILE_STRIDE, y, (z - 0.5f) * TILE_STRIDE)));
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
				int height = 1;
				/*if (x == 0 || x != clippedX) {
					height = sharedClient->heights[z][clippedX];
				}
				else {
					height = (sharedClient->heights[z][x - 1] + sharedClient->heights[z][x]) / 2;
				}*/
				float y = TILE_STRIDE * 0.9f + (height / 2) * TILE_LEVEL_OFFSET;

				// translate to the edge between tiles
				const auto translate = glm::translate(glm::mat4(1.0f), glm::vec3((x - 0.5f) * TILE_STRIDE, y, z * TILE_STRIDE));
				// 90 degree rotation
				const auto rotate = glm::rotate(translate, glm::half_pi<float>(), glm::vec3(0.f, 1.f, 0.f));
				row[x] = new Transform(rotate);
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
		bool changedTargetAngle = false;
		if (state.id != myID) {
			const auto delta = state.position - oldPos;

			if (abs(delta.x) > 0.0001 || abs(delta.z) > 0.0001) {
				state.targetAngle = glm::atan(delta.x, delta.z);
				changedTargetAngle = true;
			}
		}
		else {
			auto dir = directionBitmaskToVector(directions);
			if (dir.x != 0.f || dir.z != 0.f) {
				state.targetAngle = glm::atan(-dir.x, dir.z);
				changedTargetAngle = true;
			}
		}

		// Try to find an equivalent (+/- 2pi) target that is closer
		if (changedTargetAngle) {
			float alternateTarget = state.targetAngle - glm::two_pi<float>();
			if (abs(state.targetAngle - state.angle) > abs(alternateTarget - state.angle)) {
				state.targetAngle = alternateTarget;
			}
			alternateTarget = state.targetAngle + glm::two_pi<float>();
			if (abs(state.targetAngle - state.angle) > abs(alternateTarget - state.angle)) {
				state.targetAngle = alternateTarget;
			}
		}

		// Animate state.angle towards state.targetAngle
		state.angle += (state.targetAngle - state.angle) * 0.375f;
		if (abs(state.targetAngle - state.angle) < 0.01) {
			// state.angle has gotten close enough to state.targetAngle, so make them both between 0 and 2pi
			state.angle = state.targetAngle = fmod(state.targetAngle, glm::two_pi<float>());
		}

		auto transformMat = glm::rotate(newOffset, state.angle, glm::vec3(0.f, 1.f, 0.f));
		state.transform->setOffset(transformMat);
	}

	MoveCamera(myState->position);
	UpdateView();
}

void InGameGraphicsEngine::InitCamera(const glm::vec3 &newPlayerPos) {
	cam_look_at.x = newPlayerPos.x;
	cam_look_at.z = newPlayerPos.z;
	cam_look_at.y = TILE_LEVEL_OFFSET * TILE_SCALE + TILE_HEIGHT_ADJUST;

	cam_pos = cam_look_at + cam_angle;

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
		return cam_look_at;
	}
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

static bool iAmCaught()
{
#ifdef DEBUG_CAUGHT
	return true;
#else
	auto &allPlayers = sharedClient->getGameData()->getAllPlayers();
	return allPlayers.count(sharedClient->getMyID()) && allPlayers.at(sharedClient->getMyID())->isCaught();
#endif
}

void InGameGraphicsEngine::MoveCamera(const glm::vec3 &newPlayerPos) {
	
	light_center = newPlayerPos;
	
	if (!sharedClient || !sharedClient->getGameData())
		return;

	auto &allPlayers = sharedClient->getGameData()->getAllPlayers();

	glm::vec3 look_target = limitLookAt(newPlayerPos);
	glm::vec3 cam_target = look_target + cam_angle;
	float look_speed = 0.3f, cam_speed = 0.3f;

	if (sharedClient->getGameData()->getWT() != WinType::NONE) {
		light_center = LookAtForWT(sharedClient->getGameData()->getWT());

		look_target = limitLookAt(light_center);
		cam_target = look_target + cam_angle;
		look_speed = 0.3f;
		cam_speed = 0.15f;

		if (glm::distance(cam_target, cam_pos) < 0.001f) {
			quit = true;
		}
	}
	else if (iAmCaught()) {
		for (auto &player : players) {
#ifdef DEBUG_CAUGHT
			if (player.id != sharedClient->getMyID()) {

				light_center = player.position;
				look_target = limitLookAt(player.position);
				cam_target = look_target + cam_angle;
				look_speed = 0.3f;
				cam_speed = 0.15f;
				break;
			}
#else
			if (player.geometryIdx != static_cast<unsigned>(ModelType::CHEF) 
				&& allPlayers.count(player.id)
				&& !allPlayers.at(player.id)->isCaught()) {

				light_center = player.position;
				look_target = limitLookAt(player.position);
				cam_target = look_target + cam_angle;
				look_speed = 0.3f;
				cam_speed = 0.15f;
				break;
			}
#endif
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

	uiCanvas->setAngerRatio(((float)gameData->getChefAnger())/60.0f);
	std::map<int, Player*> players = gameData->getAllPlayers();
	//check if animals are caught
	//activates when animal is first in net.
	for (auto it = players.begin(); it != players.end(); ++it) {
		//std::cerr << "In caught loop" << "\n";
		Player * currPlayer = it->second;
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
	PlayerState * currState = getMyState();
	Player * currPlayer = players[currState->id];
	//items held by chef are the icons for animals
	if (currPlayer->isChef()) {
		if (players.find(currPlayer->getCaughtAnimalId()) != players.end()
			&& players[currPlayer->getCaughtAnimalId()]->getModelType() == ModelType::CAT) {
			uiCanvas->setItem(UICanvas::CAT_ITEM);
		}
		else if (players.find(currPlayer->getCaughtAnimalId()) != players.end()
			&& players[currPlayer->getCaughtAnimalId()]->getModelType() == ModelType::DOG) {
			uiCanvas->setItem(UICanvas::DOG_ITEM);
		}
		else if (players.find(currPlayer->getCaughtAnimalId()) != players.end()
			&& players[currPlayer->getCaughtAnimalId()]->getModelType() == ModelType::RACOON) {
			uiCanvas->setItem(UICanvas::RACCOON_ITEM);
		}

		if (currPlayer->isChef()) {
			uiCanvas->setVisible(UICanvas::PROMPT_SWING_NET, true);
		}
	}
	else {
		//disable prompts unless item is held
		uiCanvas->setVisible(UICanvas::PROMPT_GREEN_BANANA, false);
		uiCanvas->setVisible(UICanvas::PROMPT_YELLOW_BANANA, false);
		uiCanvas->setVisible(UICanvas::PROMPT_BLACK_BANANA, false);
		uiCanvas->setVisible(UICanvas::PROMPT_APPLE, false);
		uiCanvas->setVisible(UICanvas::PROMPT_ORANGE, false);


		if (currPlayer->getInventory() == ItemModelType::toiletPaper) {
			uiCanvas->setItem(UICanvas::TOILET_PAPER_ITEM);
		}
		else if (currPlayer->getInventory() == ItemModelType::rope) {
			uiCanvas->setItem(UICanvas::ROPE_ITEM);
		}
		else if (currPlayer->getInventory() == ItemModelType::plunger) {
			uiCanvas->setItem(UICanvas::PLUNGER_ITEM);
		}
		else if (currPlayer->getInventory() == ItemModelType::key1) {
			uiCanvas->setItem(UICanvas::YELLOW_KEY_ITEM);
		}
		else if (currPlayer->getInventory() == ItemModelType::key2) {
			uiCanvas->setItem(UICanvas::BLUE_KEY_ITEM);
		}
		else if (currPlayer->getInventory() == ItemModelType::key3) {
			uiCanvas->setItem(UICanvas::GREEN_KEY_ITEM);
		}
		else if (currPlayer->getInventory() == ItemModelType::screwdriver1) {
			uiCanvas->setItem(UICanvas::YELLOW_SCREWDRIVER_ITEM);
		}
		else if (currPlayer->getInventory() == ItemModelType::screwdriver2) {
			uiCanvas->setItem(UICanvas::GREEN_SCREWDRIVER_ITEM);
		}
		else if (currPlayer->getInventory() == ItemModelType::screwdriver3) {
			uiCanvas->setItem(UICanvas::RED_SCREWDRIVER_ITEM);
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
			uiCanvas->removeItems();
		}

		//set goals

		//set prompts
		if (gameData->getTile(currPlayer->getLocation())->getTileType() == TileType::JAIL &&
			gameData->getJailTile(currPlayer->getLocation())->getCapturedAnimal() != -1 && !currPlayer->isChef()) {
			uiCanvas->setVisible(UICanvas::PROMPT_JAIL_RESCUE, true);
		}
		else {
			uiCanvas->setVisible(UICanvas::PROMPT_JAIL_RESCUE, false);
		}
		uiCanvas->setVisible(UICanvas::PROMPT_SWING_NET, false);
		if (gameData->getTile(currPlayer->getLocation())->getTileType() == TileType::BOX && !currPlayer->isChef() &&
			gameData->getBoxTile(currPlayer->getLocation())->hasBox())
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

	if (clock() - elapsedTime > 1000.0 / 60)
	{
		elapsedTime = clock();
		resetIdempotentFlush();
		SendPackets();
		sharedClient->update();
		const auto gameData = sharedClient->getGameData();
		if (gameData) {
			updateBoxVisibility();

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

		for (auto &playerState : players) {
			playerModels[playerState.geometryIdx].walkObject->Update(playerState.moving != 0);
		}

		updateUIElements(gameData);
		fog->setFogDistance(gameData->chefVision);

	}

}



void DisplayCallback(GLFWwindow* window)
{
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glViewport(0, 0, windowWidth, windowHeight);
	//glDepthMask(GL_TRUE);

	//glUseProgram(objShaderProgram);
	light->draw(objShaderProgram, &cam_pos, cam_look_at);
	fog->draw(objShaderProgram, P * V * glm::vec4(light_center, 1.0f));
	root->draw(V, P, glm::mat4(1.0));

	uiCanvas->draw(&V, &P, glm::mat4(1.0));


	//raccoonModel->Draw(objShaderProgram, &V, &P);
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
	cout << "Loading models..." << endl;

	using namespace std::chrono;
	auto modelLoadingStart = high_resolution_clock::now();

	for (auto &setting : playerModelSettings) {
		cout << "\tloading " << setting.name << endl;

		auto &model = playerModels[static_cast<unsigned>(setting.modelType)];

		glm::mat4 transform = glm::scale(glm::translate(glm::mat4(1.f), setting.translate), glm::vec3(setting.scale));

		model.settings = &setting;
		model.walkObject = new FBXObject(setting.walkModelPath, setting.walkTexturePath, setting.attachSkel, setting.walkAnimIndex, false);
		model.walkGeometry = new Geometry(model.walkObject, objShaderProgram);
		if (setting.carryModelPath || setting.carryTexturePath) {
			model.carryObject = new FBXObject(setting.getCarryModelPath(), setting.getCarryTexturePath(), setting.attachSkel, setting.carryAnimIndex, false);
			model.carryGeometry = new Geometry(model.carryObject, objShaderProgram);
		}
		if (setting.actionModelPath || setting.actionTexturePath) {
			model.actionObject = new FBXObject(setting.getActionModelPath(), setting.getActionTexturePath(), setting.attachSkel, setting.actionAnimIndex, false);
			model.actionGeometry = new Geometry(model.actionObject, objShaderProgram);
		}
		model.transform = new Transform(transform);

		model.transform->addChild(model.walkGeometry);
	}

	cout << "\tloading " << "tile" << endl;
	tileModel = new FBXObject(TILE_MDL_PATH, TILE_TEX_PATH, false, false);

	cout << "\tloading " << "wall" << endl;
	wallModel = new FBXObject(WALL_MDL_PATH, WALL_TEX_PATH, false, false);

	tileGeometry = new Geometry(tileModel, objShaderProgram);
	wallGeometry = new Geometry(wallModel, objShaderProgram);

	size_t largestIdx = 0;
	for (auto &setting : itemModelSettings) {
		size_t idx = static_cast<size_t>(setting.id);
		if (largestIdx < idx) {
			largestIdx = idx;
		}
	}

	itemModels.resize(largestIdx + 1);

	for (auto &setting : itemModelSettings) {

		cout << "\tloading " << setting.name << endl;

		auto &m = itemModels[static_cast<size_t>(setting.id)];
		m.settings = &setting;
		m.object = new FBXObject(setting.modelPath, setting.texturePath, false, false);
		m.geometry = new Geometry(m.object, objShaderProgram);
	}

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
}


InGameGraphicsEngine::~InGameGraphicsEngine()
{
}

void InGameGraphicsEngine::StartLoading()  // may launch a thread and return immediately
{
	soundSystem = new SoundSystem();

	// If no audio device is plugged in, sound system will refuse to create sounds
	if (!(soundSystem->shouldIgnoreSound())) {
		fprintf(stdout, "createSound before: sound_toilet=%p\n", sound_toilet);
		soundSystem->createSoundEffect(&sound_toilet, SOUNDS_TOILET);
		fprintf(stdout, "createSound after: sound_toilet=%p\n", sound_toilet);
		soundSystem->createSoundEffect(&sound_search_item, SOUNDS_SEARCH_ITEM);
	}

	// load the shader program
	objShaderProgram = LoadShaders(OBJ_VERT_SHADER_PATH, OBJ_FRAG_SHADER_PATH);
	uiShaderProgram = LoadShaders(UI_VERT_SHADER_PATH, UI_FRAG_SHADER_PATH);

	light = new DirLight();
	fog = new FogGenerator(CHEF_FOG_DISTANCE);
	//light->toggleNormalShading();

	root = new Transform(glm::mat4(1.0));
	allPlayersNode = new Transform(glm::mat4(1.0));
	root->addChild(allPlayersNode);

	uiCanvas = new UICanvas(uiShaderProgram);

	needsRenderingSetup = true;

	auto finish = [](bool *finishedFlag) {
		LoadModels();
		*finishedFlag = true;
	};

	thread t(finish, &fullyLoaded);
	//finish(fullyLoaded);
	t.detach();
}

void InGameGraphicsEngine::CleanUp()
{

	if (allPlayersNode) {
		allPlayersNode->removeAllChildren();
		delete allPlayersNode;
		allPlayersNode = nullptr;
	}

	players.clear();

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
	if (tileGeometry)     delete tileGeometry;
	if (wallGeometry)     delete wallGeometry;
	if (light)            delete light;

	deallocFloor();

	if (floorTransform)   delete floorTransform;
	if (root)             delete root;

	glDeleteProgram(objShaderProgram);
}

void InGameGraphicsEngine::MainLoopBegin()
{
	calledMainLoopBegin = true;
	quit = false;

	if (needsRenderingSetup) {
		using namespace std::chrono;
		auto setupStart = high_resolution_clock::now();

		cout << "Calling RenderingSetup() on objects...\n ";

		tileModel->RenderingSetup();
		wallModel->RenderingSetup();
		for (auto &model : playerModels) {
			if (model.walkObject)
				model.walkObject->RenderingSetup();
			if (model.carryObject)
				model.carryObject->RenderingSetup();
			if (model.actionObject)
				model.actionObject->RenderingSetup();
		}
		for (auto &model : itemModels) {
			if (model.object)
				model.object->RenderingSetup();
		}

		auto setupEnd = high_resolution_clock::now();
		std::chrono::duration<float> setupDuration = setupEnd - setupStart;
		cout << "\tfinished RenderingSetup() in " << setupDuration.count() << " seconds\n";

		needsRenderingSetup = false;
	}

	InitCamera(glm::vec3(0.f));

	const auto gameData = sharedClient->getGameData();
	gameData->startGameClock();

	// Set clear color
	glClearColor(0.05f, 0.8f, 0.85f, 1.0f);
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
		//orthoP = glm::ortho(0.0f, (float)windowWidth, (float)windowHeight, 0.0f, -1.0f, 1.0f);
		orthoP = glm::ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);
		V = glm::lookAt(cam_pos, cam_look_at, cam_up);
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


		if (key == GLFW_KEY_UP) {
			directions |= DirectionBitmask::northSide;
		}

		if (key == GLFW_KEY_DOWN) {
			directions |= DirectionBitmask::southSide;
		}

		if (key == GLFW_KEY_LEFT) {
			directions |= DirectionBitmask::westSide;
		}

		if (key == GLFW_KEY_RIGHT) {
			directions |= DirectionBitmask::eastSide;
		}

		if (key == GLFW_KEY_SPACE) {
			// interact key press
			sharedClient->sendPackets(INTERACT_EVENT);
		}

		if (key == GLFW_KEY_ENTER) {
			// interact key press
			sharedClient->sendPackets(READY_EVENT);
		}
		if (key == GLFW_KEY_O) {
			// interact key press
			sharedClient->sendPackets(START_EVENT);
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
