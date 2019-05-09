////////////////////////////////////////
// Tester.cpp
////////////////////////////////////////

#include "Tester.h"
#include <ctime>
#include <algorithm>

#define TILE_HEIGHT_ADJUST -2.f
#define TILE_SCALE 10.f          /* overall scale of the entire floor. (TILE_SCALE * TILE_STRIDE) should match server tile size, which is currently 20 */
#define TILE_LEVEL_OFFSET 1.0f   /* from first floor to second */
#define TILE_STRIDE 2.0f         /* difference in position from one tile to the next */

int elapsedTime = 0;
GLFWwindow * window = nullptr;
int windowWidth;
int windowHeight;
const char* window_title = "TESTER";

#define CHEF_IDX     (static_cast<unsigned>(ModelType::CHEF))
#define RACCOON_IDX  (static_cast<unsigned>(ModelType::RACOON))
#define CAT_IDX      (static_cast<unsigned>(ModelType::CAT))
#define DOG_IDX      (static_cast<unsigned>(ModelType::DOG))

glm::mat4 P; // P for projection
glm::mat4 orthoP; // P for 2d elements;
glm::mat4 V; // V for view
DirLight * light = nullptr;
FogGenerator * fog = nullptr;

FBXObject * playerModels[NUM_PLAYER_MODEL_TYPES] = { nullptr };
Geometry * playerGeometry[NUM_PLAYER_MODEL_TYPES] = { nullptr };

FBXObject * tileModel = nullptr;
FBXObject * wallModel = nullptr;

FBXObject * uiCanvas = nullptr;
GLuint objShaderProgram;
GLuint uiShaderProgram;

GLuint uiTexture;

Geometry * tileGeometry;
Geometry * wallGeometry;
std::vector<std::vector<Transform *>> floorArray;
std::vector<std::vector<Transform *>> northWalls;
std::vector<std::vector<Transform *>> westWalls;

Transform * root = nullptr;
Transform * player = nullptr;
Transform * floorTransform = nullptr;

// Default camera parameters
glm::vec3 cam_pos(45.0f, 60.0f, 45.0f);    // e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);   // d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);        // up | What orientation "up" is
glm::vec3 cam_angle(-15.f, 100.f, -45.f);  // camera's preferred offset from cam_look_at

glm::vec3 playerPos = glm::vec3(0.f);
float playerTargetAngle = 0.f;
float playerAngle = 0.f;

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

void reloadMap()
{
	deallocFloor();

	const auto &tileLayout = client->getGameData()->clientTileLayout;
	
	if (!tileLayout.size()) {
		cerr << "reloadMap(): map is empty\n";
		return;
	}

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

			float y = tileLayout[z][x].getHeight() * 0.5f * TILE_LEVEL_OFFSET;
			auto skew = glm::mat4(1.0f);

			//TODO: add ramp direction to the Tile class
			// For now, randomize as a reminder
			int rampDirection = tileLayout[z][x].getTileType() == TileType::RAMP ? (1 << (rand() % 4)) : 0;

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
			if ((z > 0 && (tileLayout[z - 1][x].getWall() & DirectionBitmask::southSide)) ||
				(z == clippedZ && (tileLayout[z][x].getWall() & DirectionBitmask::northSide)))
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

			if ((x > 0 && (tileLayout[z][x - 1].getWall() & DirectionBitmask::eastSide)) ||
				(x == clippedX && (tileLayout[z][x].getWall() & DirectionBitmask::westSide)))
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

	root->addChild(floorTransform);
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
	
	// Load models
	playerModels[CHEF_IDX    ] = new FBXObject(CHEF_DAE_PATH, CHEF_TEX_PATH, false);
	playerModels[RACCOON_IDX ] = new FBXObject(RACCOON_DAE_PATH, RACCOON_TEX_PATH, false);
	playerModels[CAT_IDX     ] = new FBXObject(CAT_MDL_PATH, CAT_TEX_PATH, false);
	playerModels[DOG_IDX     ] = new FBXObject(DOG_MDL_PATH, DOG_TEX_PATH, false);

	for (unsigned i = 0; i < NUM_PLAYER_MODEL_TYPES; i++) {
		playerGeometry[i] = new Geometry(playerModels[i], objShaderProgram);
	}

	tileModel = new FBXObject(TILE_MDL_PATH, TILE_TEX_PATH, false);
	wallModel = new FBXObject(WALL_MDL_PATH, WALL_TEX_PATH, false);
	uiCanvas = new FBXObject(CANVAS_MDL_PATH, DOG_TEX_PATH, false);
	uiCanvas->SetDepthTest(false);


	tileGeometry = new Geometry(tileModel, objShaderProgram);
	wallGeometry = new Geometry(wallModel, objShaderProgram);

	root = new Transform(glm::mat4(1.0));
	player = new Transform(glm::mat4(1.0));
	Geometry * playerModel = new Geometry(playerModels[CHEF_IDX], objShaderProgram);
	root->addChild(player);
	player->addChild(playerModel);
	Transform * player2Translate = new Transform(glm::translate(glm::mat4(1.0), glm::vec3(20.0f, 0, 0)));
	Transform * player2Rotate = new Transform(glm::mat4(1.0));
	Geometry * playerModel2 = new Geometry(playerModels[DOG_IDX], objShaderProgram);
	root->addChild(player2Translate);
	player2Translate->addChild(player2Rotate);
	player2Rotate->addChild(playerModel2);
    //raccoonModel->Rotate(glm::pi<float>(), 0.0f, 1.0f, 0.0f);
	UpdateView();

	//raccoonModel->Rotate(glm::pi<float>(), 0.0f, 1.0f, 0.0f);

/*	// load the map
	// TODO: wait to get it from the server instead, display a loading screen
#define MAP_PATH "../../maps/tinytinymap/"
	loadMapArray(client->heights, MAP_PATH "heights.txt");
	loadMapArray(client->ramps, MAP_PATH "ramps.txt");
	loadMapArray(client->walls, MAP_PATH "walls.txt");

	mapFinishedLoading();*/
	MoveCamera(playerPos);

	//raccoonModel->Rotate(glm::pi<float>(), 0.0f, 1.0f, 0.0f);
}

void serverLoop(void * args) {
	while (true) {
		server->update();
	}
}

// Treat this as a destructor function. Delete dynamically allocated memory here.
void CleanUp() {
	for (auto &model : playerModels) {
		if (model) {
			delete model;
			model = nullptr;
		}
	}

	if (tileModel)        delete tileModel;
	if (wallModel)        delete wallModel;
	if (tileGeometry)     delete tileGeometry;
	if (wallGeometry)     delete wallGeometry;
	if (light)            delete light;

	deallocFloor();

	if (floorTransform)   delete floorTransform;
	if (player)           delete player;
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

void MovePlayer()
{
	Player * p = client->getGameData()->getPlayer(client->getMyID());
	if (p)
	{
		glm::vec3 location = glm::vec3(
			p->getLocation().getX(),
			p->getLocation().getY(),
			p->getLocation().getZ());

	//if (!client->clients2.empty() && (directions[0] || directions[1] || directions[2] || directions[3])) {
		//glm::vec3 prevPos = raccoonModel->GetPosition();
		const auto oldPos = playerPos;
		playerPos = location;
		glm::mat4 newOffset = glm::translate(glm::mat4(1.0f), playerPos);
		auto dir = directionBitmaskToVector(directions);
		if (dir.x != 0.f || dir.z != 0.f) {
			playerTargetAngle = glm::atan(-dir.x, dir.z);
			float alternateTarget = playerTargetAngle - glm::two_pi<float>();
			if (abs(playerTargetAngle - playerAngle) > abs(alternateTarget - playerAngle)) {
				playerTargetAngle = alternateTarget;
			}
			alternateTarget = playerTargetAngle + glm::two_pi<float>();
			if (abs(playerTargetAngle - playerAngle) > abs(alternateTarget - playerAngle)) {
				playerTargetAngle = alternateTarget;
			}
		}
		
		playerAngle += (playerTargetAngle - playerAngle) * 0.375f;
		if (abs(playerTargetAngle - playerAngle) < 0.01) {
			playerAngle = playerTargetAngle = fmod(playerTargetAngle, glm::two_pi<float>());
		}
		
		auto playerRotation = glm::rotate(newOffset, playerAngle, glm::vec3(0.f, 1.f, 0.f));
		player->setOffset(playerRotation);
		//raccoonModel->MoveTo(newPos[0], newPos[1], newPos[2]);
		MoveCamera(playerPos, oldPos);
		UpdateView();
	}

}

void MoveCamera(const glm::vec3 &newPlayerPos) {
	cam_look_at.x = newPlayerPos.x;
	cam_look_at.z = newPlayerPos.z;
	cam_look_at.y = 0.f;
	
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
	
	cam_look_at.y = 0.f;

	// Smoothly move camera to default angle, but only if keyboard is active and mouse rotation isn't
	if (directions && !mouseRotation) {
		const auto desired = cam_look_at + cam_angle;
		cam_pos += (desired - cam_pos) * 0.225f;
	}

	UpdateView();
}

void IdleCallback()
{
	/* TODO: waiting for server implementation */

	if (clock() - elapsedTime > 1000.0 / 60)
	{
		elapsedTime = clock();
		SendPackets();
		client->update();
		if (!floorArray.size())
			reloadMap();
		MovePlayer();
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
	fog->draw(objShaderProgram, P * V * glm::vec4(playerPos, 1.0f));
	root->draw(V, P, glm::mat4(1.0));
	//uiCanvas->Draw(uiShaderProgram, &V, &P, glm::mat4(1.0));


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
		if (key == GLFW_KEY_D) {
			// interact key press
			client->sendPackets(DROP_EVENT);
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
		new_cam_pos = cam_pos * scaleMouseWheel;
	}
	else {
		new_cam_pos = cam_pos / scaleMouseWheel;
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

	exit(EXIT_SUCCESS);
}
