////////////////////////////////////////
// Tester.cpp
////////////////////////////////////////

#include "Tester.h"

GLFWwindow * window;
int windowWidth;
int windowHeight;
const char* window_title = "TESTER";

glm::mat4 P; // P for projection
glm::mat4 V; // V for view
DirLight * light;
FBXObject * fbx;
GLuint objShaderProgram;

Transform * root;

// Default camera parameters
glm::vec3 cam_pos(45.0f, 60.0f, 45.0f);    // e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);  // d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);      // up | What orientation "up" is

glm::vec3 playerPos;

bool mouseRotation = false;
glm::vec2 prevPos = glm::vec2(0.0f, 0.0f);
float scaleDownMouseOps = 30.0f;
float scaleUpAngle = 4.0f;
float scaleMouseWheel = 1.05f;

ServerGame * server;
ClientGame * client;

bool directions[4] = { false, false, false, false };

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

void Init()
{
	server = new ServerGame();
	client = new ClientGame();
	_beginthread(serverLoop, 0, (void*)12);

	// load the shader program
	objShaderProgram = LoadShaders(OBJ_VERT_SHADER_PATH, OBJ_FRAG_SHADER_PATH);
	light = new DirLight();
	fbx = new FBXObject(RACCOON_DAE_PATH, RACCOON_TEX_PATH, true);
	root = new Transform(glm::mat4(1.0));
	Transform * player = new Transform(glm::rotate(glm::mat4(1.0), glm::pi<float>(), glm::vec3(0, 1, 0)));
	Geometry * playerModel = new Geometry(fbx, objShaderProgram);
	root->addChild(player);
	player->addChild(playerModel);
	Transform * player2Translate = new Transform(glm::translate(glm::mat4(1.0), glm::vec3(20.0f, 0, 0)));
	Transform * player2Rotate = new Transform(glm::rotate(glm::mat4(1.0), glm::pi<float>(), glm::vec3(0, 1, 0)));
	Geometry * playerModel2 = new Geometry(fbx, objShaderProgram);
	root->addChild(player2Translate);
	player2Translate->addChild(player2Rotate);
	player2Rotate->addChild(playerModel2);
	//fbx->Rotate(glm::pi<float>(), 0.0f, 1.0f, 0.0f);
}

void serverLoop(void * args) {
	while (true) {
		server->update();
	}
}

// Treat this as a destructor function. Delete dynamically allocated memory here.
void CleanUp() {
	delete(fbx);
	delete(light);
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

void SendPackets() 
{
	if (directions[0]) {
		client->sendMovementPackets(FORWARD_EVENT);
	}
	if (directions[1]) {
		client->sendMovementPackets(BACKWARD_EVENT);
	}
	if (directions[2]) {
		client->sendMovementPackets(LEFT_EVENT);
	}
	if (directions[3]) {
		client->sendMovementPackets(RIGHT_EVENT);
	}
}

void MovePlayer()
{
	if (!client->clients2.empty() && (directions[0] || directions[1] || directions[2] || directions[3])) {
		glm::vec3 prevPos = fbx->GetPosition();
		Location location = client->allClients["client_0"].getLocation();
		glm::vec3 newPos = glm::vec3(location.x * 0.1f, location.y * 0.1f, location.z * 0.1f);
		fbx->MoveTo(newPos[0], newPos[1], newPos[2]);
		MoveCamera(&newPos);
		UpdateView();
	}
}

void MoveCamera(glm::vec3 * newPlayerPos) {
	if (fbx->WithinBounds(-20.0f, 20.0f, -20.0f, 20.0f)) {
		cam_look_at[0] = (*newPlayerPos)[0];
		cam_pos[0] = (*newPlayerPos)[0] + 45.0f;
		cam_look_at[2] = (*newPlayerPos)[2];
		cam_pos[2] = (*newPlayerPos)[2] + 45.0f;
		UpdateView();
	}
}

void IdleCallback()
{
	/* TODO: waiting for server implementation */
	SendPackets();
	client->update();
	MovePlayer();
	//DummyMovePlayer();
}

void DisplayCallback(GLFWwindow* window)
{
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, windowWidth, windowHeight);
	glDepthMask(GL_TRUE);

	glUseProgram(objShaderProgram);
	root->draw(V, P);
	light->draw(objShaderProgram, &cam_pos);
	//fbx->Draw(objShaderProgram, &V, &P);

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

		if (key == GLFW_KEY_UP) {
			directions[0] = true;
		}

		if (key == GLFW_KEY_DOWN) {
			directions[1] = true;
		}

		if (key == GLFW_KEY_LEFT) {
			directions[2] = true;
		}

		if (key == GLFW_KEY_RIGHT) {
			directions[3] = true;
		}
	}
	else if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_UP) {
			directions[0] = false;
		}

		if (key == GLFW_KEY_DOWN) {
			directions[1] = false;
		}

		if (key == GLFW_KEY_LEFT) {
			directions[2] = false;
		}

		if (key == GLFW_KEY_RIGHT) {
			directions[3] = false;
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
	trackballX = ((2.0f * x) - width) / width;
	trackballY = (height - (2.0f * y)) / height;
	distance = sqrtf((trackballX * trackballX) + (trackballY * trackballY));
	distance = (distance < 1.0f) ? distance : 1.0f;
	toReturn = glm::vec3(trackballX, trackballY, sqrtf(1.001f - (distance*distance)));
	return glm::normalize(toReturn);
}

void TrackballRotation(float rotationAngle, glm::vec3 rotationAxis) {
	glm::vec3 newRotAxis = glm::vec4(rotationAxis, 1.0f) * V;
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotationAngle, newRotAxis);
	glm::vec4 temp = rotationMatrix * glm::vec4(cam_pos, 1.0f);
	cam_pos = glm::vec3(temp[0], temp[1], temp[2]);
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
