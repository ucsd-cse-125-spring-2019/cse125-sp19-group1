#include "Window.h"
#include "ClientGame.h"

const char* window_title = "GLFW Starter Project";


//Cube cube(5.0f);
Mesh mesh;
ClientGame * client;


int Window::width;
int Window::height;

glm::mat4 P; // P for projection
glm::mat4 V; // V for view

glm::vec3 cam_pos(0.0f, 0.0f, 20.0f);    // e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);  // d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);      // up | What orientation "up" is

Vector3f playerPos(0.0f, 0.0f, 0.0f);
Vector3f lastPlayerPos(0.0f, 0.0f, 0.0f);
float playerSpeed = 0.001f;
int playerDir = 0;


bool mouseRotation = false;
glm::vec2 prevPos = glm::vec2(0.0f, 0.0f);
float scaleDownMouseOps = 30.0f;
float scaleUpAngle = 4.0f;
float scaleMouseWheel = 1.05f;

void Window::initialize_objects()
{
	mesh = Mesh();
	mesh.LoadMesh("Door.obj");
}

void Window::clean_up()
{
}

GLFWwindow* Window::create_window(int width, int height)
{
	// Initialize GLFW.
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return NULL;
	}

	// 4x antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4);

	client = new ClientGame();

	// Create the GLFW window
	GLFWwindow * window = glfwCreateWindow(width, height, window_title, NULL, NULL);

	// Check if the window could not be created
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window
	glfwMakeContextCurrent(window);

	// Set swap interval to 1
	glfwSwapInterval(1);

	// Call the resize callback to make sure things get drawn immediately
	Window::resize_callback(window, width, height);

	return window;
}

void Window::resize_callback(GLFWwindow* window, int width, int height)
{
#ifdef __APPLE__
	glfwGetFramebufferSize(window, &width, &height); // In case your Mac has a retina display
#endif
	Window::width = width;
	Window::height = height;
	// Set the viewport size
	glViewport(0, 0, width, height);
	// Set the matrix mode to GL_PROJECTION to determine the proper camera properties
	glMatrixMode(GL_PROJECTION);
	// Load the identity matrix
	glLoadIdentity();
	// Set the perspective of the projection viewing frustum
	gluPerspective(60.0, double(width) / (double)height, 1.0, 1000.0);
	// Move camera back 20 units so that it looks at the origin (or else it's in the origin)
	glTranslatef(0, 0, -20);
}

void Window::idle_callback()
{
	// Perform any updates as necessary. Here, we will spin the cube slightly.
	//cube.update();
	//mesh.spin();
	client->update();

}

void Window::display_callback(GLFWwindow* window)
{
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Set the matrix mode to GL_MODELVIEW
	glMatrixMode(GL_MODELVIEW);
	// Load the identity matrix
	glLoadIdentity();
	
	// Render objects
	//cube.draw();
	if (playerDir == 1) {
		client->sendForwardPackets();
		//playerPos += Vector3f(0, playerSpeed, 0);
	}
	else if (playerDir == 2) {
		//playerPos -= Vector3f(0, playerSpeed, 0);
	}
	else if (playerDir == 3) {
		//playerPos -= Vector3f(playerSpeed, 0, 0);
	}
	else if (playerDir == 4) {
		//playerPos += Vector3f(playerSpeed, 0, 0);
	}
	if (!client->clients2.empty()) {
		Vector3f location = Vector3f(client->clients2["client_0"][0] * 0.1f,
			client->clients2["client_0"][1] * 0.1f,
			client->clients2["client_0"][2] * 0.1f);
		playerPos = location;
	}
	Vector3f translation = playerPos - lastPlayerPos;
	glTranslatef(translation.x, translation.y, translation.z);
	//lastPlayerPos = playerPos;
	mesh.Render(V, P, playerPos);

	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
	// Swap buffers
	glfwSwapBuffers(window);
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
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
			playerDir = 1;
		}
		if (key == GLFW_KEY_DOWN) {
			playerDir = 2;
		}
		if (key == GLFW_KEY_LEFT) {
			playerDir = 3;
		}
		if (key == GLFW_KEY_RIGHT) {
			playerDir = 4;
		}
	}
	else if (action == GLFW_RELEASE) {
		playerDir = 0;
	}
}

glm::vec3 Window::TrackballMapping(double x, double y, int width, int height) {
	float trackballX, trackballY, distance;
	glm::vec3 toReturn;
	trackballX = ((2.0f * x) - width) / width;
	trackballY = (height - (2.0f * y)) / height;
	distance = sqrtf((trackballX * trackballX) + (trackballY * trackballY));
	distance = (distance < 1.0f) ? distance : 1.0f;
	toReturn = glm::vec3(trackballX, trackballY, sqrtf(1.001f - (distance*distance)));
	return glm::normalize(toReturn);
}

void Window::TrackballRotation(float rotationAngle, glm::vec3 rotationAxis) {
	glm::vec3 newRotAxis = glm::vec4(rotationAxis, 1.0f) * V;
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotationAngle, newRotAxis);
	glm::vec4 temp = rotationMatrix * glm::vec4(cam_pos, 1.0f);
	cam_pos = glm::vec3(temp[0], temp[1], temp[2]);
	UpdateView();
}

void Window::UpdateView() {
	V = glm::lookAt(cam_pos, cam_look_at, cam_up);
}

void Window::CursorPosCallback(GLFWwindow * window, double xpos, double ypos) {
	if (mouseRotation) {
		float rotationAngle, dotProduct;
		glm::vec3 prevTrackballPos, currTrackballPos, rotationAxis;
		prevTrackballPos = TrackballMapping(prevPos[0], prevPos[1], Window::width, Window::height);
		currTrackballPos = TrackballMapping(xpos, ypos, Window::width, Window::height);
		rotationAxis = glm::cross(prevTrackballPos, currTrackballPos);
		dotProduct = glm::dot(prevTrackballPos, currTrackballPos);
		rotationAngle = glm::acos(dotProduct);
		if (!isnan(rotationAngle)) {
			TrackballRotation(scaleUpAngle*rotationAngle, rotationAxis);
		}
		prevPos = glm::vec2(xpos, ypos);
	}
}

void Window::MouseWheelCallback(GLFWwindow * window, double xoffset, double yoffset) {
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

void Window::MouseButtonCallback(GLFWwindow * window, int button, int action, int mods) {
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

	if ((button == GLFW_MOUSE_BUTTON_LEFT) && (action == GLFW_PRESS)) {
		glm::vec3 new_cam_pos;
		float distance; 
		new_cam_pos = cam_pos * scaleMouseWheel;
		distance = glm::distance(new_cam_pos, glm::vec3(0.0f, 0.0f, 0.0f));
		if (distance < 1400.0f && distance > 2.0f) {
			cam_pos = glm::vec3(new_cam_pos);
			UpdateView();
		}
	}
}


