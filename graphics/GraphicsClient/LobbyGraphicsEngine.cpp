#include "LobbyGraphicsEngine.h"

#include "FBXObject.h"
#include "../../network/ClientGame.h"
#include "../../network/ClientNetwork.h"

#define TEXTURE_PATH "../2D Elements/"
#define BG_TEX_PATH (TEXTURE_PATH "lobbyscreen.png")
#define WARNING_TEX_PATH (TEXTURE_PATH "lobbyscreen_warning.png")
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


LobbyGraphicsEngine::LobbyGraphicsEngine() : TwoDeeGraphicsEngine()
{
	backgroundFilename = BG_TEX_PATH;
	warningObject = nullptr;
}


LobbyGraphicsEngine::~LobbyGraphicsEngine()
{
}


void LobbyGraphicsEngine::MainLoopBegin()
{
	TwoDeeGraphicsEngine::MainLoopBegin();

	loadSprites(youArePlayerMsg, LOBBY_MAX_PLAYERS);
	loadSprites(playerMsg, LOBBY_MAX_PLAYERS);
	loadSprites(startMsg, 2);
	loadSprites(&switchMsg, 1);

	if (!warningObject) {
		warningObject = createObjectForTexture(WARNING_TEX_PATH);
	}

	warningAlpha = 0.f;
}

void LobbyGraphicsEngine::MainLoopEnd()
{
	TwoDeeGraphicsEngine::MainLoopEnd();

	freeSprites(youArePlayerMsg, LOBBY_MAX_PLAYERS);
	freeSprites(playerMsg, LOBBY_MAX_PLAYERS);
	freeSprites(startMsg, 2);
	freeSprites(&switchMsg, 1);

	if (warningObject) {
		delete warningObject;
		warningObject = nullptr;
	}
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
		sharedClient->sendPackets(SELECT_EVENT);
		cout << "Lobby got SPACE" << endl;
		break;
	}
	case GLFW_KEY_ENTER:
	{
		// Only send the start event if we have the right mix of players
		// (or for debugging allow it if we are connected to localhost)
		if (startEnabled || !strcmp(SERVER_IP_ADDRESS, LOCAL_HOST)) {
			sharedClient->sendPackets(START_EVENT);
			quit = true;
		}

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

void LobbyGraphicsEngine::DrawSprite(LobbySprite &sprite, float designX, float designY, float alpha)
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

	glUniform1f(uAlpha, alpha * screenAlpha);
	sprite.object->Draw(passthroughShaderProgram, &identityMat, &orthoProj, transform);
}

void LobbyGraphicsEngine::MainLoopCallback(GLFWwindow * window)
{
	TwoDeeGraphicsEngine::MainLoopCallback(window);

	int myPlayerNum = 1;

	auto gameData = sharedClient->getGameData();
	sharedClient->update();

	if (sharedClient->getGameData()->getGameState() != GameState::IN_LOBBY)
		quit = true;
	std::map < int, Player * > &players = gameData->getAllPlayers();
	if(gameData->getPlayer(sharedClient->getMyID()))
		myPlayerNum = gameData->getPlayer(sharedClient->getMyID())->getPlayerNum();

	bool playerIsOnline[LOBBY_MAX_PLAYERS] = { false };
	bool playerIsAnimal[LOBBY_MAX_PLAYERS] = { false };

	// update playerIsAnimal using server values
	for (auto iter = players.begin(); iter != players.end(); iter++)
	{
		Player * player = iter->second;
		auto idx = (player->getPlayerNum() - 1) % LOBBY_MAX_PLAYERS;
		playerIsOnline[idx] = true;
		playerIsAnimal[idx] = player->hasSelectedAnimal();
	}

	// Now draw the players

#define PLAYER_MARGIN_Y 3.25f
#define PLAYER_TOP_Y 339.f
	static const float columnX[2] = {221.f, 572.f};

	int chefCount = 0;
	int animalCount = 0;
	float columnY[2] = { PLAYER_TOP_Y, PLAYER_TOP_Y };
	for (int i = 0; i < LOBBY_MAX_PLAYERS; ++i) {
		targetY[i] = columnY[playerIsAnimal[i]];
		if (playerY[i] > 1.f) {
			playerY[i] += (targetY[i] - playerY[i]) * 0.3f;

			if (abs(targetY[i] - playerY[i]) < 0.1) {
				playerY[i] = targetY[i];
			}
		}
		else {
			playerY[i] = targetY[i];
		}

		float targetX = playerIsOnline[i] ? columnX[playerIsAnimal[i]] : 0.5f * (columnX[0] + columnX[1]);

		if (playerX[i] < 1.f || abs(targetX - playerX[i]) < 0.1) {
			playerX[i] = targetX;
		}
		else {
			playerX[i] += (targetX - playerX[i]) * 0.3f;
		}
		
		float playerAlpha = 1.f - sinf(((playerX[i] - columnX[0]) / (columnX[1] - columnX[0])) * glm::pi<float>());

		DrawSprite(playerMsg[i], playerX[i], playerY[i], playerAlpha);

		if (playerIsOnline[i]) {
			columnY[playerIsAnimal[i]] += playerMsg[i].designHeight + PLAYER_MARGIN_Y;
			chefCount += !playerIsAnimal[i];
			animalCount += playerIsAnimal[i];
		}
	}

	// Draw the rest

	double now = glfwGetTime();

#define START_PERIOD 0.8
	float startAlpha = sinf((now / START_PERIOD) * glm::pi<double>()) * 0.5f + 0.5f;
	startEnabled = (chefCount == 1 && animalCount == 3);

	DrawSprite(youArePlayerMsg[myPlayerNum - 1], 792, 10);
	DrawSprite(switchMsg, 340, 472);
	DrawSprite(startMsg[startEnabled], 342, 504, startEnabled ? startAlpha : 1.f);

	warningAlpha += ((1 - startEnabled) - warningAlpha) * 0.3f;

	if (warningObject) {
		glUniform1f(uAlpha, warningAlpha * screenAlpha);
		warningObject->Draw(passthroughShaderProgram, &identityMat, &orthoProj, identityMat);
	}
}
