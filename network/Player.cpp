#include "Player.h"

Player::Player() : playerID(-1) { std::cout << "player default constructor called\n"; addDecodeFunctions(); }
Player::Player(int anID) : playerID(anID), inventory(ItemName::EMPTY), hasCake(false), isChef(false), modelType(ModelType::RACOON)
{
	location = Location();
	addDecodeFunctions();
	if (anID % 2 == 1) {
		isChef = true;
		modelType = ModelType::CHEF;
	}
}

Player::Player(int anID, Location aLoc) : playerID(anID), location(aLoc), inventory(ItemName::EMPTY), hasCake(false), isChef(false), modelType(ModelType::RACOON)
{
	addDecodeFunctions();
	if (anID % 2 == 1) {
		isChef = true;
		modelType = ModelType::CHEF;
	}
}

Location Player::getLocation() { return location; }
ItemName Player::getInventory() { return inventory; }
bool Player::getInteracting() { return interacting; }
bool Player::getOpenJail() { return openingJail; }
bool Player::getOpeningGate() { return openingGate; }

void Player::setLocation(float argX, float argY, float argZ)
{
	location.update(argX, argY, argZ);
}

void Player::setLocation(Location aLoc)
{
	location.update(aLoc.getX(), aLoc.getY(), aLoc.getZ());
}

void Player::setInventory(ItemName anItem)
{
	inventory = anItem;
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


ModelType Player::getModelType() {
	return modelType;
}

bool Player::getIsChef() {
	return isChef;
}

void Player::setCaughtAnimal(bool caught) {
	caughtAnimal = caught;
}

void Player::setIsCaught(bool caught) {
	isCaught = caught;
}

bool Player::getCaughtAnimal() {
	return caughtAnimal;
}

int Player::getCaughtAnimalId() {
	return caughtAnimalId;
}

void Player::setCaughtAnimalId(int id) {
	caughtAnimalId = id;
}

bool Player::getIsCaught() {
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

std::string Player::encodePlayerData()
{
	std::stringstream encodedData;
	encodedData << "client: " << playerID << std::endl;
	encodedData << "isChef: " << isChef << std::endl;
	encodedData << "model: " << static_cast<int>(modelType) << std::endl;
	encodedData << "inventory: " << static_cast<int>(inventory) << std::endl;
	encodedData << "hasCake: " << hasCake << std::endl;
	encodedData << "location: " << location.getX() << " " << location.getY() << " " << location.getZ() << std::endl;

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
void Player::decodeChefStatus(std::string value)
{
	isChef = std::stoi(value) == 1;
}
void Player::decodeModelType(std::string value)
{
	modelType = static_cast<ModelType>(std::stoi(value));
}

void Player::addDecodeFunctions()
{
	decodingFunctions["location"] = &Player::decodeLocation;
	decodingFunctions["isChef"] = &Player::decodeChefStatus;
	decodingFunctions["model"] = &Player::decodeModelType;
	decodingFunctions["hasCake"] = &Player::decodeCakeStatus;
	decodingFunctions["inventory"] = &Player::decodeInventory;
}
