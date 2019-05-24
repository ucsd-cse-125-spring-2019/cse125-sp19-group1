#include "Player.h"

Player::Player() : playerID(-1) { std::cout << "player default constructor called\n"; addDecodeFunctions(); }

Player::Player(int anID, Location aLoc) : playerID(anID), location(aLoc), inventory(ItemModelType::EMPTY),
	modelType(ModelType::RACOON), visionRadius(VISION_RADIUS)
{
	addEncodeFunctions();
	addDecodeFunctions();
	if (anID == 1) {
		modelType = ModelType::CHEF;
	}
}


 Location Player::getLocation() const { return location; }
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

void Player::setInteracting(bool interact) {
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


ModelType Player::getModelType() const {
	return modelType;
}

bool Player::isChef() const {
	return modelType == ModelType::CHEF;
}

void Player::setCaughtAnimal(bool caught) {
	caughtAnimal = caught;
}
void Player::setCaughtAnimalType(ModelType type) {
	caughtAnimalType = type;
}

void Player::setCaughtStatus(bool caught) {
	caughtStatus = caught;
}

bool Player::hasCaughtAnimal() const {
	return caughtAnimal;
}

int Player::getCaughtAnimalId() const {
	return caughtAnimalId;
}
ModelType Player::getCaughtAnimalType() const {
	return caughtAnimalType;
}

void Player::setCaughtAnimalId(int id) {
	caughtAnimalId = id;
}

bool Player::isCaught() const {
	return caughtStatus;
}

bool Player::inRange(Location & myLoc, Location & theirLoc) {
	double dist = sqrt(pow(myLoc.getX() - theirLoc.getX(), 2) +
		pow(myLoc.getY() - theirLoc.getY(), 2) +
		pow(myLoc.getZ() - theirLoc.getZ(), 2) * 1.0);
	if (dist < catchRadius) {
		return true;
	}
	return false;
}

void Player::setActionStartTime() {
	actionStartTime = std::chrono::system_clock::now();
}

void Player::setUnlockJailStartTime() {
	unlockJailStartTime = std::chrono::system_clock::now();
}

double Player::getInteractingTime(int opt) {
	auto now = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = now - actionStartTime;
	if (opt == 1) 
	{
		elapsed_seconds = now - unlockJailStartTime;
	}
	return elapsed_seconds.count();
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
	encodingFunctions["location"] = &Player::encodeLocation;
	encodingFunctions["model"] = &Player::encodeModelType;
	encodingFunctions["inventory"] = &Player::encodeInventory;
	encodingFunctions["hidden"] = &Player::encodeHidden;
	encodingFunctions["interactAction"] = &Player::encodeInteractAction;
	encodingFunctions["visionRadius"] = &Player::encodeVisionRadius;
	encodingFunctions["caughtStatus"] = &Player::encodeCaughtStatus;
	encodingFunctions["caughtAnimal"] = &Player::encodeCaughtAnimal;
	encodingFunctions["caughtAnimalType"] = &Player::encodeCaughtAnimalType;


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
	encodedData << "caughtStatus: " << caughtStatus << std::endl;

	return encodedData.str();
}
std::string Player::encodeCaughtAnimal() {
	std::stringstream encodedData;
	encodedData << "caughtAnimal: " << caughtAnimal << std::endl;

	return encodedData.str();
}

std::string Player::encodeCaughtAnimalType() {
	std::stringstream encodedData;
	encodedData << "caughtAnimalType: " << static_cast<int>(caughtAnimalType) << std::endl;

	return encodedData.str();
}
