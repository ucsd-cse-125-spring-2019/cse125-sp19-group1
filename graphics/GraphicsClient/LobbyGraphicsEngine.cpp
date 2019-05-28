#include "LobbyGraphicsEngine.h"

#include "FBXObject.h"
#include "../../network/ClientGame.h"

#define TEXTURE_PATH "../2D Elements/"
#define BG_TEX_PATH (TEXTURE_PATH "lobbyscreen.png")
#define LOBBY_PNG(x) (TEXTURE_PATH "lobby_" x ".png" )

#define DESIGN_WIDTH 964
#define DESIGN_HEIGHT 542

extern ClientGame * sharedClient;

static const glm::mat4 identityMat(1.f);

struct LobbySprite {
	const char * filename;
	float designWidth, designHeight;
	FBXObject * object;
};

LobbySprite youArePlayerMsg[LOBBY_MAX_PLAYERS] = {
	{ LOBBY_PNG("youareplayer1"), 169, 100 },
	{ LOBBY_PNG("youareplayer2"), 169, 100 },
	{ LOBBY_PNG("youareplayer3"), 169, 100 },
	{ LOBBY_PNG("youareplayer4"), 169, 100 },
};

LobbySprite playerMsg[LOBBY_MAX_PLAYERS] = {
	{ LOBBY_PNG("player1small"), 94,  0 },
	{ LOBBY_PNG("player2small"), 99,  0 },
	{ LOBBY_PNG("player3small"), 99,  0 },
	{ LOBBY_PNG("player4small"), 102, 0 },
};

LobbySprite startMsg[2] = {
	{ LOBBY_PNG("startdisable"), 316, 0 },
	{ LOBBY_PNG("start"),        316, 0 },
};

LobbySprite switchMsg = { LOBBY_PNG("switch"), 318, 0 };

static void loadSprites(LobbySprite *sprites, size_t count)
{
	int texWidth = 0, texHeight = 0;

	for (unsigned i = 0; i < count; ++i) {
		auto obj = createObjectForTexture(sprites[i].filename);
		obj->GetTextureSize(texWidth, texHeight);

		float w = sprites[i].designWidth, h = sprites[i].designHeight;
		if (abs(w) < 1.f) {
			w = (h * texWidth) / texHeight;
		}
		else if (abs(h) < 1.f) {
			h = (w * texHeight) / texWidth;
		}

		sprites[i].object = obj;
		sprites[i].designWidth = w;
		sprites[i].designHeight = h;
	}
}

static void freeSprites(LobbySprite *sprites, size_t count)
{
	for (unsigned i = 0; i < count; ++i) {
		if (sprites[i].object) {
			delete sprites[i].object;
			sprites[i].object = nullptr;
		}
	}
}


LobbyGraphicsEngine::LobbyGraphicsEngine()
{
	backgroundFilename = BG_TEX_PATH;
}


LobbyGraphicsEngine::~LobbyGraphicsEngine()
{
}


void LobbyGraphicsEngine::StartLoading()
{
	TwoDeeGraphicsEngine::StartLoading();

	loadSprites(youArePlayerMsg, LOBBY_MAX_PLAYERS);
	loadSprites(playerMsg, LOBBY_MAX_PLAYERS);
	loadSprites(startMsg, 2);
	loadSprites(&switchMsg, 1);
}

void LobbyGraphicsEngine::CleanUp()
{
	TwoDeeGraphicsEngine::CleanUp();

	freeSprites(youArePlayerMsg, LOBBY_MAX_PLAYERS);
	freeSprites(playerMsg, LOBBY_MAX_PLAYERS);
	freeSprites(startMsg, 2);
	freeSprites(&switchMsg, 1);
}

void LobbyGraphicsEngine::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// We are only interested in key presses
	if (action != GLFW_PRESS) {
		return;
	}

	switch (key) {
	case GLFW_KEY_SPACE:
	{
		// TODO: send a packet to the server
		playerIsAnimal[myPlayerNum - 1] = !playerIsAnimal[myPlayerNum - 1];

		cout << "Lobby got SPACE" << endl;
		break;
	}
	case GLFW_KEY_ENTER:
	{
		if (startEnabled) {
			// TODO: send a packet to the server
		}

		//TODO: only do this if the server says it's ok
		quit = true;

		cout << "Lobby got ENTER" << endl;
		break;
	}
	case GLFW_KEY_ESCAPE:
	{
		// Close the window. This causes the program to also terminate.
		glfwSetWindowShouldClose(window, GL_TRUE);
		break;
	}
	default:
		break;
	}
}

void LobbyGraphicsEngine::DrawSprite(LobbySprite &sprite, int designX, int designY, float alpha)
{
	glm::vec3 center = glm::vec3(0.f);
	center.x = ((designX + sprite.designWidth / 2) / DESIGN_WIDTH) * 2.f - 1.f;
	center.y = ((designY + sprite.designHeight / 2) / DESIGN_HEIGHT) * -2.f + 1.f;
	glm::mat4 transform = glm::translate(glm::mat4(1.f), center);

	glm::vec3 scale;
	scale.x = sprite.designWidth / DESIGN_WIDTH;
	scale.y = sprite.designHeight / DESIGN_HEIGHT;
	scale.z = 1.f;
	transform = glm::scale(transform, scale);

	glUniform1f(uAlpha, alpha);
	sprite.object->Draw(passthroughShaderProgram, &identityMat, &orthoProj, transform);
}

void LobbyGraphicsEngine::MainLoopCallback(GLFWwindow * window)
{
	TwoDeeGraphicsEngine::MainLoopCallback(window);

	double now = glfwGetTime();

#define START_PERIOD 0.8
	float startAlpha = sinf((now / START_PERIOD) * glm::pi<double>()) * 0.5f + 0.5f;

	// For now, randomize playerIsAnimal for the other 3 players
	// TODO: get this from the server instead
	int blah = (int)(now / 2.75);
	static int lastBlah = -1;
	if (blah != lastBlah) {
		lastBlah = blah;
		for (int i = 0; i < LOBBY_MAX_PLAYERS; ++i) {
			if (i + 1 != myPlayerNum) {
				playerIsAnimal[i] = rand() & 1;
			}
		}
	}

	int chefCount = 0;
	float playerY[2] = { 339, 339 };
	for (int i = 0; i < LOBBY_MAX_PLAYERS; ++i) {
		DrawSprite(playerMsg[i], playerIsAnimal[i] ? 572 : 221, playerY[playerIsAnimal[i]]);
		playerY[playerIsAnimal[i]] += playerMsg[i].designHeight + 3.25f;
		chefCount += !playerIsAnimal[i];
	}

	startEnabled = (chefCount == 1);

	DrawSprite(youArePlayerMsg[myPlayerNum - 1], 792, 10);
	DrawSprite(switchMsg, 340, 472);
	DrawSprite(startMsg[startEnabled], 342, 504, startEnabled ? startAlpha : 1.f);
}
