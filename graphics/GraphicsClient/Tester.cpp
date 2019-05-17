////////////////////////////////////////
// Tester.cpp
////////////////////////////////////////

#include "Tester.h"
#include "ItemModelType.h"

#include <ctime>
#include <algorithm>

//#define ENV_OBJS_DEMO

#define TILE_HEIGHT_ADJUST -2.f
#define TILE_SCALE 10.f          /* overall scale of the entire floor. (TILE_SCALE * TILE_STRIDE) should match server tile size, which is currently 20 */
#define TILE_LEVEL_OFFSET 1.0f   /* from first floor to second */
#define TILE_STRIDE 2.0f         /* difference in position from one tile to the next */

// Uncomment to render an additional dummy Chef that mirrors the player's movements
// #define DUMMY_ID -4000

int elapsedTime = 0;
GLFWwindow * window = nullptr;
int windowWidth;
int windowHeight;
const char* window_title = GAME_NAME_SHORT;

#define CHEF_IDX     (static_cast<unsigned>(ModelType::CHEF))
#define RACCOON_IDX  (static_cast<unsigned>(ModelType::RACOON))
#define CAT_IDX      (static_cast<unsigned>(ModelType::CAT))
#define DOG_IDX      (static_cast<unsigned>(ModelType::DOG))

static const struct {
	const char *modelPath;
	const char *texturePath;
	const char *name;
	ItemModelType id;
} itemModelSettings[] = {
	{ MODELS_PATH "apple.fbx", TEXTURES_PATH "apple.ppm", "apple", ItemModelType::apple },
	{ MODELS_PATH "banana.fbx", TEXTURES_PATH "bananagreen.ppm", "green banana", ItemModelType::bananaGreen },
	{ MODELS_PATH "banana.fbx", TEXTURES_PATH "bananaperfect.ppm", "perfect banana", ItemModelType::bananaPerfect },
	{ MODELS_PATH "banana.fbx", TEXTURES_PATH "bananaveryveryripe.ppm", "very ripe banana", ItemModelType::bananaVeryRipe },
	{ MODELS_PATH "box.fbx", TEXTURES_PATH "box.ppm", "box", ItemModelType::box },
	{ MODELS_PATH "cake.fbx", TEXTURES_PATH "cake.ppm", "cake", ItemModelType::cake },
	{ MODELS_PATH "canvas.fbx", TEXTURES_PATH "canvas.ppm", "canvas", ItemModelType::canvas },
	{ MODELS_PATH "cookingpot.fbx", TEXTURES_PATH "cookingpot.ppm", "cooking pot", ItemModelType::cookingPot },
	{ MODELS_PATH "door.fbx", TEXTURES_PATH "door.ppm", "door", ItemModelType::door },
	{ MODELS_PATH "fork.fbx", TEXTURES_PATH "fork.ppm", "fork", ItemModelType::fork },
	{ MODELS_PATH "garbagebag.fbx", TEXTURES_PATH "garbagebag.ppm", "garbage bag", ItemModelType::garbageBag },
	{ MODELS_PATH "jail.fbx", TEXTURES_PATH "jail.ppm", "jail", ItemModelType::jail },
	{ MODELS_PATH "key.fbx", TEXTURES_PATH "key1.ppm", "key #1", ItemModelType::key1 },
	{ MODELS_PATH "key.fbx", TEXTURES_PATH "key2.ppm", "key #2", ItemModelType::key2 },
	{ MODELS_PATH "key.fbx", TEXTURES_PATH "key3.ppm", "key #3", ItemModelType::key3 },
	{ MODELS_PATH "keydrop.fbx", TEXTURES_PATH "keydrop.ppm", "key drop", ItemModelType::keyDrop },
	{ MODELS_PATH "keydrop.fbx", TEXTURES_PATH "keydrop_bathroom.ppm", "bathroom key drop", ItemModelType::keyDropBathroom },
	{ MODELS_PATH "keydrop.fbx", TEXTURES_PATH "keydrop_frontexit.ppm", "front exit key drop", ItemModelType::keyDropFrontExit },
	{ MODELS_PATH "keydrop.fbx", TEXTURES_PATH "keydrop_vent.ppm", "vent key drop", ItemModelType::keyDropVent },
	{ MODELS_PATH "knife.fbx", TEXTURES_PATH "knife.ppm", "knife", ItemModelType::knife },
	{ MODELS_PATH "orange.fbx", TEXTURES_PATH "orange.ppm", "orange fruit", ItemModelType::orange },
	{ MODELS_PATH "painting.fbx", TEXTURES_PATH "painting.ppm", "wall painting", ItemModelType::painting },
	{ MODELS_PATH "pear.fbx", TEXTURES_PATH "pear.ppm", "pear", ItemModelType::pear },
	{ MODELS_PATH "plate.fbx", TEXTURES_PATH "plate.ppm", "plate", ItemModelType::plate },
	{ MODELS_PATH "plunger.fbx", TEXTURES_PATH "plunger.ppm", "plunger", ItemModelType::plunger },
	{ MODELS_PATH "restaurantchair.fbx", TEXTURES_PATH "restaurantchair.ppm", "restaurant chair", ItemModelType::restaurantChair },
	{ MODELS_PATH "rope.fbx", TEXTURES_PATH "rope.ppm", "rope", ItemModelType::rope },
	{ MODELS_PATH "screwdriver.fbx", TEXTURES_PATH "screwdriver1.ppm", "screwdriver #1", ItemModelType::screwdriver1 },
	{ MODELS_PATH "screwdriver.fbx", TEXTURES_PATH "screwdriver2.ppm", "screwdriver #2", ItemModelType::screwdriver2 },
	{ MODELS_PATH "screwdriver.fbx", TEXTURES_PATH "screwdriver3.ppm", "screwdriver #3", ItemModelType::screwdriver3 },
	{ MODELS_PATH "stove.fbx", TEXTURES_PATH "stove.ppm", "stove", ItemModelType::stove },
	{ MODELS_PATH "toilet.fbx", TEXTURES_PATH "toilet.ppm", "toilet", ItemModelType::toilet },
	{ MODELS_PATH "toiletpaper.fbx", TEXTURES_PATH "toiletpaper.ppm", "toilet paper", ItemModelType::toiletPaper },
	{ MODELS_PATH "vent.fbx", TEXTURES_PATH "vent.ppm", "vent", ItemModelType::vent },
	{ MODELS_PATH "window.fbx", TEXTURES_PATH "window.ppm", "window", ItemModelType::window },
};

struct ItemModel {
	ItemModelType id;
	const char *name;
	FBXObject *object;
	Geometry *geometry;
};

vector<ItemModel> itemModels;

glm::mat4 P; // P for projection
glm::mat4 orthoP; // P for 2d elements;
glm::mat4 V; // V for view
DirLight * light = nullptr;
FogGenerator * fog = nullptr;

FBXObject * playerModels[NUM_PLAYER_MODEL_TYPES] = { nullptr };
Geometry * playerGeometry[NUM_PLAYER_MODEL_TYPES] = { nullptr };

FBXObject * tileModel = nullptr;
FBXObject * wallModel = nullptr;

UICanvas * uiCanvas = nullptr;
GLuint objShaderProgram;
GLuint uiShaderProgram;

GLuint uiTexture;

Geometry * tileGeometry;
Geometry * wallGeometry;
vector<vector<Transform *>> floorArray;
vector<vector<Transform *>> northWalls;
vector<vector<Transform *>> westWalls;
vector<vector<Transform *>> envObjs;

vector<vector<uint8_t>> envObjsMap;

struct PlayerState {
	Transform *transform;       // for position and rotation
	float targetAngle;          // the angle it's trying to animate to
	float angle;                // angle it's currently facing (should be approaching targetAngle)
	glm::vec3 position;         // position, important for faking targetAngle above
	int id;                     // the player ID from the server
	unsigned geometryIdx;       // index into playerGeometry

	PlayerState() : position(0.f) {
		angle = targetAngle = 0.f;
		geometryIdx = 0;
		transform = nullptr;
	}

#ifdef DUMMY_ID
	// Warning: this constructor is only for debugging use
	PlayerState(int newId, int geomIdx) {
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
		transform->addChild(playerGeometry[geometryIdx]);
	}
};

Transform * root = nullptr;
Transform * allPlayersNode = nullptr;
std::vector<PlayerState> players;
Transform * floorTransform = nullptr;
Transform * envObjsTransform = nullptr;

// Default camera parameters
glm::vec3 cam_pos(45.0f, 60.0f, 45.0f);          // e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);         // d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);              // up | What orientation "up" is
const glm::vec3 cam_angle(-15.f, 100.f, -45.f);  // camera's preferred offset from cam_look_at

bool mouseRotation = false;
glm::vec2 prevPos = glm::vec2(0.0f, 0.0f);
float scaleDownMouseOps = 30.0f;
float scaleUpAngle = 4.0f;
float scaleMouseWheel = 1.05f;

ServerGame * server = nullptr;
ClientGame * client = nullptr;

int directions = 0;   // use DirectionBitmask

void ErrorCallback(int error, const char* description)
{
	// Print error
	fputs(description, stderr);
}

void SetupGlew()
{
	// Initialize GLEW. Not needed on OSX systems.
#ifndef __APPLE__
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		glfwTerminate();
	}
	fprintf(stdout, "Current GLEW version: %s\n", glewGetString(GLEW_VERSION));
#endif
}

void SetupOpenGLSettings()
{
#ifndef __APPLE__
	// Setup GLEW. Don't do this on OSX systems.
	SetupGlew();
#endif
	// Enable depth buffering
	glEnable(GL_DEPTH_TEST);
	// Related to shaders and z value comparisons for the depth buffer
	glDepthFunc(GL_LEQUAL);
	// Set polygon drawing mode to fill front and back of each polygon
	// You can also use the paramter of GL_LINE instead of GL_FILL to see wireframes
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// Disable backface culling to render both sides of polygons
	glDisable(GL_CULL_FACE);
	/* Enable culling of faces to speed up rendering
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK); */
	// Set clear color
	glClearColor(0.05f, 0.8f, 0.85f, 1.0f);
	// Set shading to smooth
	glShadeModel(GL_SMOOTH);
	// Auto normalize surface normals
	glEnable(GL_NORMALIZE);
}

void PrintVersions()
{
	// Get info of GPU and supported OpenGL version
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("OpenGL version supported %s\n", glGetString(GL_VERSION));

	//If the shading language symbol is defined
#ifdef GL_SHADING_LANGUAGE_VERSION
	std::printf("Supported GLSL version is %s.\n", (char *)glGetString(GL_SHADING_LANGUAGE_VERSION));
#endif
}

PlayerState *getMyState()
{
	// linear search is probably faster than binary/tree/whatever for <= 4 elements

	int myId = client->getMyID();
	for (PlayerState &state : players) {
		if (state.id == myId)
			return &state;
	}

	return nullptr;
}

void reloadPlayers()
{
	players.clear();

	cout << "[Re]loading players\n";

	if (!client || !client->getGameData() || client->getGameData()->players.size() <= 0) {
		cout << "WARNING: gameData players map is empty\n";
		return;
	}

	allPlayersNode->removeAllChildren();

	const auto &playersMap = client->getGameData()->players;
	const int myId = client->getMyID();

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
	const auto &state = players[players.size() - 1];
	allPlayersNode->addChild(state.transform);
#endif
}

void reloadMap()
{
	deallocFloor();

	const auto &tileLayout = client->getGameData()->clientTileLayout;
	
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

	if (envObjsTransform == nullptr) {
		envObjsTransform = new Transform(glm::mat4(1.f));
		root->addChild(envObjsTransform);
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
					height = client->heights[clippedZ][x];
				} else {
					height = (client->heights[z - 1][x] + client->heights[z][x]) / 2;
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
					height = client->heights[z][clippedX];
				}
				else {
					height = (client->heights[z][x - 1] + client->heights[z][x]) / 2;
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

	uint8_t objIdx = 0;
	envObjs.resize(floorArray.size());
	for (size_t z = 0; z < envObjs.size(); z++) {
		auto &row = envObjs[z];
		row.resize(floorArray[z].size());
		for (size_t x = 0; x < row.size(); x++) {
#ifdef ENV_OBJS_DEMO
			if (objIdx == 0) objIdx = 1;
			else if (++objIdx >= itemModels.size()) objIdx = 1;
#else
			const auto &tile = tileLayout[z][x];
			
			uint8_t objIdx = envObjsMap[z][x];
			if (tile->getTileType() == TileType::JAIL) {
				objIdx = static_cast<uint8_t>(ItemModelType::jail);
			}

			if (objIdx == 0) {
				row[x] = nullptr;
				continue;
		}
#endif
			ItemModelType modelType = static_cast<ItemModelType>(objIdx);
			

			// Try to turn the object away from the wall
			float angle = 0.f;
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

			glm::vec3 translate;
			translate.x = (x + 0.5f) * TILE_STRIDE * TILE_SCALE;
			translate.y = tile->getHeight() * 0.5f * TILE_LEVEL_OFFSET * TILE_SCALE;
			translate.z = (z + 0.5f) * TILE_STRIDE * TILE_SCALE;
			auto rotate = glm::rotate(glm::translate(glm::mat4(1.f), translate), angle, glm::vec3(0.f, 1.f, 0.f));
			row[x] = new Transform(rotate);
			row[x]->addChild(itemModels[objIdx].geometry);
			envObjsTransform->addChild(row[x]);
		}
	}
}

void deallocFloor()
{
	if (floorTransform) {
		delete floorTransform;
		floorTransform = nullptr;
	}

	for (size_t z = 0; z < floorArray.size(); z++) {
		auto &row = floorArray[z];

		for (size_t x = 0; x < row.size(); x++) {
			delete row[x];
		}

		row.resize(0);
	}
}

void Init()
{
	server = new ServerGame();
	client = new ClientGame();
	//_beginthread(serverLoop, 0, (void*)12);

	// load the shader program
	objShaderProgram = LoadShaders(OBJ_VERT_SHADER_PATH, OBJ_FRAG_SHADER_PATH);
	uiShaderProgram = LoadShaders(UI_VERT_SHADER_PATH, UI_FRAG_SHADER_PATH);
	
	light = new DirLight();
	fog = new FogGenerator(CHEF_FOG_DISTANCE);
	//light->toggleNormalShading();
	
	loadMapArray(envObjsMap, "../../maps/tinytinymap/env_objs.txt");

	// Load models
	cout << "Loading models..." << endl;

	using namespace std::chrono;
	auto modelLoadingStart = high_resolution_clock::now();
	
	cout << "\tloading " << "chef" << endl;
	playerModels[CHEF_IDX    ] = new FBXObject(CHEF_DAE_PATH, CHEF_TEX_PATH, false);
	
	cout << "\tloading " << "raccoon" << endl;
	playerModels[RACCOON_IDX ] = new FBXObject(RACCOON_DAE_PATH, RACCOON_TEX_PATH, true);
	
	cout << "\tloading " << "cat" << endl;
	playerModels[CAT_IDX     ] = new FBXObject(CAT_DAE_PATH, CAT_TEX_PATH, true);
	
	cout << "\tloading " << "dog" << endl;
	playerModels[DOG_IDX     ] = new FBXObject(DOG_MDL_PATH, DOG_TEX_PATH, false);

	cout << "\tloading " << "tile" << endl;
	tileModel = new FBXObject(TILE_MDL_PATH, TILE_TEX_PATH, false);
	
	cout << "\tloading " << "wall" << endl;
	wallModel = new FBXObject(WALL_MDL_PATH, WALL_TEX_PATH, false);	
	for (unsigned i = 0; i < NUM_PLAYER_MODEL_TYPES; i++) {
		playerGeometry[i] = new Geometry(playerModels[i], objShaderProgram);
	}

	uiCanvas = new UICanvas(uiShaderProgram);

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
		m.id = setting.id;
		m.name = setting.name;
		m.object = new FBXObject(setting.modelPath, setting.texturePath, false);
		m.geometry = new Geometry(m.object, objShaderProgram);
	}

	auto modelLoadingEnd = high_resolution_clock::now();
	std::chrono::duration<float> modelLoadingDuration = modelLoadingEnd - modelLoadingStart;
	cout << "\tfinished loading models in " << modelLoadingDuration.count() << " seconds\n";

	root = new Transform(glm::mat4(1.0));
	allPlayersNode = new Transform(glm::mat4(1.0));
	root->addChild(allPlayersNode);

	UpdateView();
	MoveCamera(glm::vec3(0.f));
}

void serverLoop(void * args) {
	while (true) {
		server->update();
	}
}

// Treat this as a destructor function. Delete dynamically allocated memory here.
void CleanUp() {
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
		if (model) {
			delete model;
			model = nullptr;
		}
	}

	for (auto &geom : playerGeometry) {
		if (geom) {
			delete geom;

		}
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

void ResizeCallback(GLFWwindow* window, int newWidth, int newHeight)
{
#ifdef __APPLE__
	glfwGetFramebufferSize(window, &newWidth, &newHeight); // In case your Mac has a retina display
#endif
	windowWidth = newWidth;
	windowHeight = newHeight;
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

GLFWwindow* CreateWindowFrame(int width, int height)
{
	// Initialize GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return NULL;
	}

	// 4x antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ // Because Apple hates comforming to standards
	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window
	GLFWwindow* window = glfwCreateWindow(width, height, window_title, NULL, NULL);

	// Check if the window could not be created
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		fprintf(stderr, "Either GLFW is not installed or your graphics card does not support modern OpenGL.\n");
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window
	glfwMakeContextCurrent(window);

	// Set swap interval to 1
	glfwSwapInterval(1);

	// Get the width and height of the framebuffer to properly resize the window
	glfwGetFramebufferSize(window, &width, &height);
	// Call the resize callback to make sure things get drawn immediately
	ResizeCallback(window, width, height);

	return window;
}

glm::vec3 directionBitmaskToVector(int bitmask) {
	int dirX = ((directions & DirectionBitmask::eastSide) != 0) - ((directions & DirectionBitmask::westSide) != 0);
	int dirZ = ((directions & DirectionBitmask::northSide) != 0) - ((directions & DirectionBitmask::southSide) != 0);

	return glm::vec3((float)dirX, 0.f, (float)dirZ);
}

void SendPackets() 
{
	auto dir = directionBitmaskToVector(directions);

	if (dir.z > 0) {
		client->sendPackets(FORWARD_EVENT);
	}
	else if (dir.z < 0) {
		client->sendPackets(BACKWARD_EVENT);
	}

	if (dir.x > 0) {
		client->sendPackets(LEFT_EVENT);
	} 
	else if (dir.x < 0) {
		client->sendPackets(RIGHT_EVENT);
	}
}

void MovePlayers()
{
	// Sanity guard condition
	auto myState = getMyState();
	if (players.size() <= 0 || myState == nullptr) {
		return;
	}

	const auto myOldPos = myState->position;

	const int myID = client->getMyID();
	const auto &playersMap = client->getGameData()->players;
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

	MoveCamera(myState->position, myOldPos);
	UpdateView();
}

void MoveCamera(const glm::vec3 &newPlayerPos) {
	cam_look_at.x = newPlayerPos.x;
	cam_look_at.z = newPlayerPos.z;
	cam_look_at.y = TILE_LEVEL_OFFSET * TILE_SCALE + TILE_HEIGHT_ADJUST;
	
	cam_pos = cam_look_at + cam_angle;

	UpdateView();
}

void MoveCamera(const glm::vec3 &newPlayerPos, const glm::vec3 &oldPlayerPos) {
	auto screenPos = glm::project(newPlayerPos, glm::mat4(1.f), P * V, glm::vec4(0.f, 0.f, 1.f, 1.f));
	
	// Make the coordinates range from -1 to 1
	screenPos = (screenPos - glm::vec3(0.5f, 0.5f, 0.f)) * 2.f;

	// If the player is outside a circle of radius 0.3, then move camera
	if (screenPos.x * screenPos.x + screenPos.y * screenPos.y > 0.3f * 0.3f) {
		const auto offsetX = newPlayerPos.x - oldPlayerPos.x;
		cam_look_at.x += offsetX;
		cam_pos.x += offsetX;

		const auto offsetZ = newPlayerPos.z - oldPlayerPos.z;
		cam_look_at.z += offsetZ;
		cam_pos.z += offsetZ;
	}
	
	cam_look_at.y = TILE_LEVEL_OFFSET * TILE_SCALE + TILE_HEIGHT_ADJUST;

	// Smoothly move camera to default angle, but only if keyboard is active and mouse rotation isn't
	if (directions && !mouseRotation) {
		const auto desired = cam_look_at + cam_angle;
		cam_pos += (desired - cam_pos) * 0.225f;
	}

	UpdateView();
}

// Call glFlush(), but only if we haven't already called it this frame
bool alreadyFlushed = false;
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

void IdleCallback()
{
	/* TODO: waiting for server implementation */

	if (clock() - elapsedTime > 1000.0 / 60)
	{
		elapsedTime = clock();
		resetIdempotentFlush();
		SendPackets();
		client->update();
		const auto gameData = client->getGameData();
		if (gameData) {
#ifdef DUMMY_ID
			bool playersChanged = (players.size() != gameData->players.size() + 1);
#else
			bool playersChanged = (players.size() != gameData->players.size());
#endif
			const bool tilesChanged = (floorArray.size() != gameData->clientTileLayout.size());

			const auto &playersMap = client->getGameData()->players;
			for (auto &state : players) {
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
				
			if (playersChanged) {
				idempotentFlush();
				reloadPlayers();
			}
		}
		MovePlayers();
		//DummyMovePlayer();
		server->update();
		//raccoonModel->Rotate(glm::pi<float>()/1000, 0.0f, 1.0f, 0.0f);
		
		for (auto model : playerModels) {
			model->Update();
		}
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
	auto myState = getMyState();
	if (myState) {
		fog->draw(objShaderProgram, P * V * glm::vec4(myState->position, 1.0f));
	}
	root->draw(V, P, glm::mat4(1.0));

	uiCanvas->draw(&V, &P, glm::mat4(1.0));


	//raccoonModel->Draw(objShaderProgram, &V, &P);

	// Swap buffers
	glfwSwapBuffers(window);
	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Check for a key press
	if (action == GLFW_PRESS)
	{
		// Check if escape was pressed
		if (key == GLFW_KEY_ESCAPE)
		{
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		if (key == GLFW_KEY_M) {
			uiCanvas->setAngerRatio(1.0f);
			uiCanvas->setVisible(uiCanvas->RACCOON_HAPPY, false);
			uiCanvas->setVisible(uiCanvas->RACCOON_JAIL, true);
		}


		if (key == GLFW_KEY_A) {
			for (auto model : playerModels) {
				model->ToNextKeyframe();
			}
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
			client->sendPackets(INTERACT_EVENT);
		}

		if (key == GLFW_KEY_ENTER) {
			// interact key press
			client->sendPackets(READY_EVENT);
		}
		if (key == GLFW_KEY_O) {
			// interact key press
			client->sendPackets(START_EVENT);
		}
		if (key == GLFW_KEY_D) {
			client->sendPackets(DROP_EVENT);
		}
		if (key == GLFW_KEY_H) {
			// interact key press
			client->sendPackets(HIDE_EVENT);
		}
		if (key == GLFW_KEY_0) {
			client->sendPackets(SELECT0_EVENT);
		}
		if (key == GLFW_KEY_1) {
			client->sendPackets(SELECT1_EVENT);
		}
		if (key == GLFW_KEY_2) {
			client->sendPackets(SELECT2_EVENT);
		}
		if (key == GLFW_KEY_3) {
			client->sendPackets(SELECT3_EVENT);
		}
		if (key == GLFW_KEY_4) {
			client->sendPackets(SELECT4_EVENT);
		}
		if (key == GLFW_KEY_P) {
			client->sendPackets(RESET_EVENT);
		}

	}
	else if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_UP) {
			directions &= ~ DirectionBitmask::northSide;
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
			client->sendPackets(RELEASE_EVENT);
		}
	}
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

void CursorPosCallback(GLFWwindow * window, double xpos, double ypos) {
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

void MouseWheelCallback(GLFWwindow * window, double xoffset, double yoffset) {
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

void MouseButtonCallback(GLFWwindow * window, int button, int action, int mods) {
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

void SetupCallbacks()
{
	// Set the error callback
	glfwSetErrorCallback(ErrorCallback);
	// Set the key callback
	glfwSetKeyCallback(window, KeyCallback);
	// Set the window resize callback
	glfwSetFramebufferSizeCallback(window, ResizeCallback);
	// Set the mouse input callback
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	// Set the cursor position callback
	glfwSetCursorPosCallback(window, CursorPosCallback);
	// Set the mouse wheel callback
	glfwSetScrollCallback(window, MouseWheelCallback);
}

int main(void)
{
	// Create the GLFW window
	window = CreateWindowFrame(640, 480);
	// Print OpenGL and GLSL versions
	PrintVersions();
	// Setup callbacks
	SetupCallbacks();
	// Setup OpenGL settings, including lighting, materials, etc.
	SetupOpenGLSettings();
	// Initialize objects/pointers for rendering
	Init();

	// Loop while GLFW window should stay open
	while (!glfwWindowShouldClose(window))
	{
		using namespace std::chrono;

		auto start = high_resolution_clock::now();

		// Main render display callback. Rendering of objects is done here.
		DisplayCallback(window);
		// Idle callback. Updating objects, etc. can be done here.
		IdleCallback();

		auto finish = high_resolution_clock::now();

		// Limit to no more than 100fps
		auto frameTime = duration_cast<microseconds>(finish - start);
		if (frameTime < chrono::microseconds(10000)) {
			this_thread::sleep_for(chrono::microseconds(10000) - frameTime);
		}
		
		//cout << frameTime.count() / 1000.0 << endl;   // print # of milliseconds it took to process this frame
	}

	CleanUp();
	// Destroy the window
	glfwDestroyWindow(window);
	// Terminate GLFW
	glfwTerminate();

	return EXIT_SUCCESS;
}
