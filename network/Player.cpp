#include "Player.h"

Player::Player() : playerID(-1) { std::cout << "player default constructor called\n"; addDecodeFunctions(); }

Player::Player(int anID, int aPlayerNum, Location aLoc) : playerID(anID), playerNum(aPlayerNum), location(aLoc), inventory(ItemModelType::EMPTY),
	modelType(ModelType::RACOON), visionRadius(VISION_RADIUS), speedMultiplier(1.0), selectedAnimal(true)
{
	addEncodeFunctions();
	addDecodeFunctions();
	//if (anID == 1) {
	//	modelType = ModelType::CHEF;
	//}
}


Location Player::getLocation() const { return location; }
int Player::getPlayerNum() const { return playerNum; }
bool Player::hasSelectedAnimal() const { return selectedAnimal; }
void Player::toggleAnimalSelection() { selectedAnimal = !selectedAnimal; dirtyVariablesMap["animalSelection"] = true; }
bool Player::isReady() const
{
	return ready;
}

void Player::toggleReady()
{
	ready = !ready;
}
ItemModelType Player::getInventory() const { return inventory; }
bool Player::isInteracting() const { return interacting; }
//bool Player::getOpenJail() const { return openingJail; }
//bool Player::getOpeningGate() const { return openingGate; }
bool Player::getHidden() { return hidden; }
Action Player::getAction() const { return action; }

Direction Player::getFacingDirection() const { return facingDirection; }
void Player::setFacingDirection(Direction dir) { facingDirection = dir; }

void Player::setPlayerID(int id)
{
	playerID = id;
}
void Player::setModelType(ModelType type)
{
	modelType = type;
	dirtyVariablesMap["model"] = true;
}
void Player::setAction(Action anAction)
{
	action = anAction;
	dirtyVariablesMap["interactAction"] = true;
}
void Player::setLocation(float argX, float argY, float argZ)
{
	location.update(argX, argY, argZ);
	dirtyVariablesMap["location"] = true;

}

void Player::setLocation(Location aLoc)
{
	location.update(aLoc.getX(), aLoc.getY(), aLoc.getZ());
	dirtyVariablesMap["location"] = true;
}

void Player::setInventory(ItemModelType anItem)
{
	inventory = anItem;
	dirtyVariablesMap["inventory"] = true;

}

void Player::setHidden(bool hide)
{
	hidden = hide;
}

void Player::setInteracting(bool interact) 
{
	interacting = interact;
}

//void Player::setOpenJail(bool interact) {
//	openingJail = interact;
//}
//
//
//void Player::setOpeningGate(bool status) {
//	openingGate = status;
//}

ModelType Player::getModelType() const 
{
	return modelType;
}

bool Player::isChef() const 
{
	return modelType == ModelType::CHEF;
}

void Player::setCaughtAnimal(bool caught) 
{
	caughtAnimal = caught;
	dirtyVariablesMap["caughtAnimal"] = true;
}
void Player::setCaughtAnimalType(ModelType type) {
	caughtAnimalType = type;
	dirtyVariablesMap["caughtAnimalType"] = true;
}

//<<<<<<< HEAD
void Player::setCaughtStatus(bool caught) {
	caughtStatus = caught;
	dirtyVariablesMap["caughtStatus"] = true;
}

bool Player::hasCaughtAnimal() const {
//=======
//void Player::setIsCaught(bool caught) 
//{
//	isCaught = caught;
//}

//bool Player::getCaughtAnimal() const 
//{
//>>>>>>> server
	return caughtAnimal;
}

int Player::getCaughtAnimalId() const 
{
	return caughtAnimalId;
}
ModelType Player::getCaughtAnimalType() const {
	return caughtAnimalType;
}

void Player::setCaughtAnimalId(int id) 
{
	caughtAnimalId = id;
}

//<<<<<<< HEAD
bool Player::isCaught() const {
	return caughtStatus;
//=======
////bool Player::getIsCaught() {
//bool Player::getIsCaught() const 
//{
//	return isCaught;
//>>>>>>> server
}

bool Player::inRange(Location & myLoc, Location & theirLoc) 
{
	double dist = sqrt(pow(myLoc.getX() - theirLoc.getX(), 2) +
		pow(myLoc.getY() - theirLoc.getY(), 2) +
		pow(myLoc.getZ() - theirLoc.getZ(), 2) * 1.0);
	if (dist < catchRadius) {
		return true;
	}
	return false;
}

void Player::setActionStartTime() 
{
	actionStartTime = std::chrono::system_clock::now();
}

//<<<<<<< HEAD
void Player::setUnlockJailStartTime() {
	unlockJailStartTime = std::chrono::system_clock::now();
}
//=======
void Player::setSpeedStartTime()
{
	speedStartTime = std::chrono::system_clock::now();
}
void Player::setSearchStartTime()
{
	searchStartTime = std::chrono::system_clock::now();
}
void Player::setVisionStartTime()
{
	visionStartTime = std::chrono::system_clock::now();
}
void Player::setSlowStartTime()
{
	slowStartTime = std::chrono::system_clock::now();
}

void Player::setStartJailTime() 
{
	startJail = std::chrono::system_clock::now();
//>>>>>>> server
}
void Player::setSpeedMultiplier(double multiplier)
{
	speedMultiplier = multiplier;
}

double Player::getSpeedMultiplier() const
{
	return speedMultiplier;
}

double Player::getInteractingTime(int opt) 
{
	auto now = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = now - actionStartTime;
	if (opt == 1) 
	{
		elapsed_seconds = now - unlockJailStartTime;
	}
	return elapsed_seconds.count();
}

double Player::getSpeedTime()
{
	auto now = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = now - speedStartTime;
	return elapsed_seconds.count();
}

double Player::getSearchTime()
{
	auto now = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = now - searchStartTime;
	return elapsed_seconds.count();
}

double Player::getVisionTime()
{
	auto now = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = now - visionStartTime;
	return elapsed_seconds.count();
}

double Player::getSlowTime()
{
	auto now = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = now - slowStartTime;
	return elapsed_seconds.count();
}


//chef interaction/power-up methods
void Player::updateChefMultiplier(int anger) 
{
	if (anger < 12) 
	{
		chefSpeedMultiplier = 1;
		catchRadius = 10;
	}
	else if (anger < 24) 
	{
		chefSpeedMultiplier = 1;
		catchRadius = 12;
	}
	else if (anger < 36) 
	{
		chefSpeedMultiplier = 1.15;
		catchRadius = 14;
	}
	else if (anger < 48) 
	{
		chefSpeedMultiplier = 1.25;
		catchRadius = 16;
	}
	else if (anger < 59) 
	{
		chefSpeedMultiplier = 1.3;
		catchRadius = 18;
	}
	else 
	{
		chefSpeedMultiplier = 1.35;
		catchRadius = 20;
	}

	if (getSlowChef())
	{
		chefSpeedMultiplier = slowedSpeed;
	}
}


std::string Player::encodePlayerData(bool newPlayerInit)
//std::string Player::encodePlayerData() const
{
	std::stringstream encodedData;
	encodedData << "client: " << playerID << std::endl;

	for (auto p : dirtyVariablesMap)
	{
		std::string key = p.first;
		bool dirty = p.second;
		if (dirty || newPlayerInit)
		{
			if (encodingFunctions.count(key) > 0)
			{
				encodedData << (this->*encodingFunctions.at(key))();
				dirtyVariablesMap[key] = false;
			}
			else
				std::cout << "Missing encoding function for key " << key << std::endl;
		}
	}

	return encodedData.str();
}

void Player::decodePlayerData(std::string key, std::string value)
{
	// Safety check in case decoding function for the key doesn't exist
	if (decodingFunctions.count(key) > 0)
		(this->*decodingFunctions[key])(value); // Format for calling the functions from the map
	else
		std::cout << "No decoding function for key: " << key << std::endl;
}

void Player::decodePlayerNum(std::string value)
{
	playerNum = std::stoi(value);
}

void Player::decodeAnimalSelection(std::string value)
{
	selectedAnimal = value == "1";
}
void Player::decodeLocation(std::string value)
{
	std::stringstream valueStream(value);
	std::string x, y, z;
	valueStream >> x >> y >> z;
	setLocation(std::stof(x), std::stof(y), std::stof(z));
}

void Player::decodeInventory(std::string value)
{
	inventory = static_cast<ItemModelType>(std::stoi(value));
}

void Player::decodeModelType(std::string value)
{
	modelType = static_cast<ModelType>(std::stoi(value));
}
void Player::decodeHidden(std::string value)
{
	hidden = std::stoi(value) == 1;
}

void Player::decodeInteractAction(std::string value)
{
	action = static_cast<Action>(std::stoi(value));
}

void Player::decodeVisionRadius(std::string value)
{
	visionRadius = std::stof(value);
}

void Player::decodeCaughtStatus(std::string value)
{
	std::cout << "decoding caught status " << value << std::endl;
	caughtStatus = value == "1";
}
void Player::decodeCaughtAnimal(std::string value)
{
	caughtAnimal = value == "1";
}
void Player::decodeCaughtAnimalType(std::string value)
{
	caughtAnimalType = static_cast<ModelType>(std::stoi(value));
}

void Player::addDecodeFunctions()
{
	decodingFunctions["playerNum"] = &Player::decodePlayerNum;
	decodingFunctions["animalSelection"] = &Player::decodeAnimalSelection;
	decodingFunctions["location"] = &Player::decodeLocation;
	decodingFunctions["model"] = &Player::decodeModelType;
	decodingFunctions["inventory"] = &Player::decodeInventory;
	decodingFunctions["hidden"] = &Player::decodeHidden;
	decodingFunctions["interactAction"] = &Player::decodeInteractAction;
	decodingFunctions["visionRadius"] = &Player::decodeVisionRadius;
	decodingFunctions["caughtStatus"] = &Player::decodeCaughtStatus;
	decodingFunctions["caughtAnimal"] = &Player::decodeCaughtAnimal;
	decodingFunctions["caughtAnimalType"] = &Player::decodeCaughtAnimalType;

}

void Player::addEncodeFunctions()
{
	encodingFunctions["playerNum"] = &Player::encodePlayerNum;
	encodingFunctions["animalSelection"] = &Player::encodeAnimalSelection;
	encodingFunctions["location"] = &Player::encodeLocation;
	encodingFunctions["model"] = &Player::encodeModelType;
	encodingFunctions["inventory"] = &Player::encodeInventory;
	encodingFunctions["hidden"] = &Player::encodeHidden;
	encodingFunctions["interactAction"] = &Player::encodeInteractAction;
	encodingFunctions["visionRadius"] = &Player::encodeVisionRadius;
	encodingFunctions["caughtStatus"] = &Player::encodeCaughtStatus;
	encodingFunctions["caughtAnimal"] = &Player::encodeCaughtAnimal;
	encodingFunctions["caughtAnimalType"] = &Player::encodeCaughtAnimalType;


	dirtyVariablesMap["playerNum"] = true;
	dirtyVariablesMap["animalSelection"] = true;
	dirtyVariablesMap["location"] = true;
	dirtyVariablesMap["inventory"] = true;
	dirtyVariablesMap["model"] = true;
	dirtyVariablesMap["hidden"] = true;
	dirtyVariablesMap["interactAction"] = true;
	dirtyVariablesMap["visionRadius"] = true;
	dirtyVariablesMap["caughtStatus"] = true;
	dirtyVariablesMap["caughtAnimal"] = true;
	dirtyVariablesMap["caughtAnimalType"] = true;

}
std::string Player::encodePlayerNum() {
	std::stringstream encodedData;
	encodedData << "playerNum: " << playerNum << std::endl;

	return encodedData.str();
}

std::string Player::encodeAnimalSelection() {
	std::stringstream encodedData;
	encodedData << "animalSelection: " << selectedAnimal << std::endl;

	return encodedData.str();
}
std::string Player::encodeLocation() {
	std::stringstream encodedData;
	encodedData << "location: " << location.getX() << " " << location.getY() << " " << location.getZ() << std::endl;

	return encodedData.str();
}
std::string Player::encodeInventory() {
	std::stringstream encodedData;
	encodedData << "inventory: " << static_cast<int>(inventory) << std::endl;

	return encodedData.str();
}

std::string Player::encodeModelType() {
	std::stringstream encodedData;
	encodedData << "model: " << static_cast<int>(modelType) << std::endl;

	return encodedData.str();
}

std::string Player::encodeHidden() {
	std::stringstream encodedData;
	encodedData << "hidden: " << hidden << std::endl;

	return encodedData.str();
}

std::string Player::encodeInteractAction() {
	std::stringstream encodedData;
	encodedData << "interactAction: " << static_cast<int>(action) << std::endl;

	return encodedData.str();
}

std::string Player::encodeVisionRadius() {
	std::stringstream encodedData;
	encodedData << "visionRadius: " << visionRadius << std::endl;

	return encodedData.str();
}
std::string Player::encodeCaughtStatus() {
	std::stringstream encodedData;
	encodedData << "caughtStatus: " << (int)caughtStatus << std::endl;

	return encodedData.str();
}
std::string Player::encodeCaughtAnimal() {
	std::stringstream encodedData;
	encodedData << "caughtAnimal: " << (int)caughtAnimal << std::endl;

	return encodedData.str();
}

std::string Player::encodeCaughtAnimalType() {
	std::stringstream encodedData;
	encodedData << "caughtAnimalType: " << static_cast<int>(caughtAnimalType) << std::endl;

	return encodedData.str();
}
