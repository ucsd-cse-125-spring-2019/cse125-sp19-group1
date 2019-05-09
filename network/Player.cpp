#include "Player.h"

Player::Player() : playerID(-1) { std::cout << "player default constructor called\n"; addDecodeFunctions(); }
Player::Player(int anID) : playerID(anID), inventory(ItemName::EMPTY), hasCake(false), modelType(ModelType::RACOON)
{
	location = Location();
	addEncodeFunctions();
	addDecodeFunctions();
	if (anID % 2 == 1) {
		modelType = ModelType::CHEF;
	}
}

Player::Player(int anID, Location aLoc) : playerID(anID), location(aLoc), inventory(ItemName::EMPTY), hasCake(false), modelType(ModelType::RACOON)
{
	addEncodeFunctions();
	addDecodeFunctions();
	if (anID % 2 == 1) {
		modelType = ModelType::CHEF;
	}
}
void Player::setModelType(ModelType type)
{
	modelType = type;
	dirtyVariablesMap["model"] = true;
}

 Location Player::getLocation() const { return location; }
ItemName Player::getInventory() const { return inventory; }
bool Player::getInteracting() { return interacting; }
bool Player::getOpenJail() const { return openingJail; }
bool Player::getOpeningGate() const { return openingGate; }
bool Player::getHidden() { return hidden; }


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

void Player::setInventory(ItemName anItem)
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


void Player::setOpenJail(bool interact) {
	openingJail = interact;
}


void Player::setOpeningGate(bool status) {
	openingGate = status;
}


ModelType Player::getModelType() const {
	return modelType;
}

bool Player::isChef() const {
	return modelType == ModelType::CHEF;
}

void Player::setCaughtAnimal(bool caught) {
	caughtAnimal = caught;
}

void Player::setIsCaught(bool caught) {
	isCaught = caught;
}

bool Player::getCaughtAnimal() const {
	return caughtAnimal;
}

int Player::getCaughtAnimalId() const {
	return caughtAnimalId;
}

void Player::setCaughtAnimalId(int id) {
	caughtAnimalId = id;
}

//bool Player::getIsCaught() {
bool Player::getIsCaught() const {
	return isCaught;
}

bool Player::inRange(Location & myLoc, Location & theirLoc) {
	double dist = sqrt(pow(myLoc.getX() - theirLoc.getX(), 2) +
		pow(myLoc.getY() - theirLoc.getY(), 2) +
		pow(myLoc.getZ() - theirLoc.getZ(), 2) * 1.0);
	if (dist < radius) {
		return true;
	}
	return false;
}

void Player::setStartTime() {
	start = std::chrono::system_clock::now();
}

void Player::setStartJailTime() {
	startJail = std::chrono::system_clock::now();
}

double Player::checkProgress(int opt) {
	auto now = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = now - start;
	if (opt == 1) 
	{
		elapsed_seconds = now - startJail;
	}
	return elapsed_seconds.count();
}

std::string Player::encodePlayerData(bool newPlayerInit)
//std::string Player::encodePlayerData() const
{
	std::stringstream encodedData;
	encodedData << "client: " << playerID << std::endl;

	//encodedData << "isChef: " << isChef << std::endl;
	//encodedData << "model: " << static_cast<int>(modelType) << std::endl;
	//encodedData << "inventory: " << static_cast<int>(inventory) << std::endl;
	//encodedData << "hasCake: " << hasCake << std::endl;
	//encodedData << "hidden: " << hidden << std::endl;
	//encodedData << "location: " << location.getX() << " " << location.getY() << " " << location.getZ() << std::endl;
	for (auto p : dirtyVariablesMap)
	{
		std::string key = p.first;
		bool dirty = p.second;
		if (dirty || newPlayerInit)
		{
			if (encodingFunctions.count(key) > 0)
			{
				encodedData << (this->*encodingFunctions.at(key))();

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
	inventory = static_cast<ItemName>(std::stoi(value));
}
void Player::decodeCakeStatus(std::string value)
{
	hasCake = std::stoi(value) == 1;
}

void Player::decodeModelType(std::string value)
{
	modelType = static_cast<ModelType>(std::stoi(value));
}
void Player::decodeHidden(std::string value)
{
	hidden = std::stoi(value) == 1;
}

void Player::addDecodeFunctions()
{
	decodingFunctions["location"] = &Player::decodeLocation;
	decodingFunctions["model"] = &Player::decodeModelType;
	decodingFunctions["hasCake"] = &Player::decodeCakeStatus;
	decodingFunctions["inventory"] = &Player::decodeInventory;
	decodingFunctions["hidden"] = &Player::decodeHidden;
}

void Player::addEncodeFunctions()
{
	encodingFunctions["location"] = &Player::encodeLocation;
	encodingFunctions["model"] = &Player::encodeModelType;
	encodingFunctions["hasCake"] = &Player::encodeCakeStatus;
	encodingFunctions["inventory"] = &Player::encodeInventory;
	encodingFunctions["hidden"] = &Player::encodeHidden;

	dirtyVariablesMap["location"] = true;
	dirtyVariablesMap["inventory"] = true;
	dirtyVariablesMap["hasCake"] = true;
	dirtyVariablesMap["model"] = true;
	dirtyVariablesMap["hidden"] = true;

}
std::string Player::encodeLocation() {
	std::stringstream encodedData;
	encodedData << "location: " << location.getX() << " " << location.getY() << " " << location.getZ() << std::endl;
	dirtyVariablesMap["location"] = false;

	return encodedData.str();
}
std::string Player::encodeInventory() {
	std::stringstream encodedData;
	encodedData << "inventory: " << static_cast<int>(inventory) << std::endl;
	dirtyVariablesMap["inventory"] = false;

	return encodedData.str();
}
std::string Player::encodeCakeStatus() {
	std::stringstream encodedData;
	encodedData << "hasCake: " << hasCake << std::endl;
	dirtyVariablesMap["hasCake"] = false;

	return encodedData.str();
}

std::string Player::encodeModelType() {
	std::stringstream encodedData;
	encodedData << "model: " << static_cast<int>(modelType) << std::endl;
	dirtyVariablesMap["model"] = false;

	return encodedData.str();
}

std::string Player::encodeHidden() {
	std::stringstream encodedData;
	encodedData << "hidden: " << hidden << std::endl;
	dirtyVariablesMap["hidden"] = false;

	return encodedData.str();
}


