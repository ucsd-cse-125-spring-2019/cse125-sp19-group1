
#include "UIObject.h"

#define NUM_ITEMS 52
#define CANVAS_PATH    "../Canvas/"

#define CANVAS_MDL_PATH		(CANVAS_PATH "canvas.fbx")
#define UI_BASE_TEX (CANVAS_PATH "base.png")
#define RACCOON_HAPPY_TEX (CANVAS_PATH "raccoonHappy.png")
#define CAT_HAPPY_TEX (CANVAS_PATH "catHappy.png")
#define DOG_HAPPY_TEX (CANVAS_PATH "dogHappy.png")
#define RACCOON_JAIL_TEX (CANVAS_PATH "raccoonJail.png")
#define CAT_JAIL_TEX (CANVAS_PATH "catJail.png")
#define DOG_JAIL_TEX (CANVAS_PATH "dogJail.png")
#define RACCOON_ITEM_TEX (CANVAS_PATH "raccoonItem.png")
#define CAT_ITEM_TEX (CANVAS_PATH "catItem.png")
#define DOG_ITEM_TEX (CANVAS_PATH "dogItem.png")
#define CAKE_ITEM_TEX (CANVAS_PATH "cakeItem.png")
#define YELLOW_KEY_ITEM_TEX (CANVAS_PATH "yellowKeyItem.png")
#define BLUE_KEY_ITEM_TEX (CANVAS_PATH "blueKeyItem.png")
#define GREEN_KEY_ITEM_TEX (CANVAS_PATH "greenKeyItem.png")
#define PLUNGER_ITEM_TEX (CANVAS_PATH "plungerItem.png")
#define ROPE_ITEM_TEX (CANVAS_PATH "ropeItem.png")
#define TOILET_PAPER_ITEM_TEX (CANVAS_PATH "toiletPaperItem.png")
#define RED_SCREWDRIVER_ITEM_TEX (CANVAS_PATH "redScrewdriverItem.png")
#define GREEN_SCREWDRIVER_ITEM_TEX (CANVAS_PATH "greenScrewdriverItem.png")
#define YELLOW_SCREWDRIVER_ITEM_TEX (CANVAS_PATH "yellowScrewdriverItem.png")
#define APPLE_ITEM_TEX (CANVAS_PATH "appleItem.png")
#define ORANGE_ITEM_TEX (CANVAS_PATH "orangeItem.png")
#define BANANA_YELLOW_ITEM_TEX (CANVAS_PATH "bananaItemYellow.png")
#define BANANA_GREEN_ITEM_TEX (CANVAS_PATH "bananaItemGreen.png")
#define BANANA_BLACK_ITEM_TEX (CANVAS_PATH "bananaItemBlack.png")
#define GOALS_TEX (CANVAS_PATH "goals.png")
#define YELLOW_KEY_GOAL_INCOMPLETE_TEX (CANVAS_PATH "yellowKeyGoalIncomplete.png")
#define BLUE_KEY_GOAL_INCOMPLETE_TEX (CANVAS_PATH "blueKeyGoalIncomplete.png")
#define GREEN_KEY_GOAL_INCOMPLETE_TEX (CANVAS_PATH "greenKeyGoalIncomplete.png")
#define YELLOW_KEY_GOAL_COMPLETE_TEX (CANVAS_PATH "yellowKeyGoalComplete.png")
#define BLUE_KEY_GOAL_COMPLETE_TEX (CANVAS_PATH "blueKeyGoalComplete.png")
#define GREEN_KEY_GOAL_COMPLETE_TEX (CANVAS_PATH "greenKeyGoalComplete.png")
#define RED_SCREWDRIVER_GOAL_INCOMPLETE_TEX (CANVAS_PATH "redScrewdriverGoalIncomplete.png")
#define GREEN_SCREWDRIVER_GOAL_INCOMPLETE_TEX (CANVAS_PATH "greenScrewdriverGoalIncomplete.png")
#define YELLOW_SCREWDRIVER_GOAL_INCOMPLETE_TEX (CANVAS_PATH "yellowScrewdriverGoalIncomplete.png")
#define RED_SCREWDRIVER_GOAL_COMPLETE_TEX (CANVAS_PATH "redScrewdriverGoalComplete.png")
#define GREEN_SCREWDRIVER_GOAL_COMPLETE_TEX (CANVAS_PATH "greenScrewdriverGoalComplete.png")
#define YELLOW_SCREWDRIVER_GOAL_COMPLETE_TEX (CANVAS_PATH "yellowScrewdriverGoalComplete.png")
#define PLUNGER_GOAL_INCOMPLETE_TEX (CANVAS_PATH "plungerGoalIncomplete.png")
#define ROPE_GOAL_INCOMPLETE_TEX (CANVAS_PATH "ropeGoalIncomplete.png")
#define TOILET_PAPER_GOAL_INCOMPLETE_TEX (CANVAS_PATH "toiletPaperGoalIncomplete.png")
#define PLUNGER_GOAL_COMPLETE_TEX (CANVAS_PATH "plungerGoalComplete.png")
#define ROPE_GOAL_COMPLETE_TEX (CANVAS_PATH "ropeGoalComplete.png")
#define TOILET_PAPER_GOAL_COMPLETE_TEX (CANVAS_PATH "toiletPaperGoalComplete.png")
#define PROMPT_JAIL_RESCUE_TEX (CANVAS_PATH "promptRescueMate.png")
#define PROMPT_SWING_NET_TEX (CANVAS_PATH "promptSwingNet.png")
#define PROMPT_BOX_SEARCH_TEX (CANVAS_PATH "promptSearchItem.png")
#define PROMPT_GREEN_BANANA_TEX (CANVAS_PATH "powerup_greenbanana.png")
#define PROMPT_YELLOW_BANANA_TEX (CANVAS_PATH "powerup_yellowbanana.png")
#define PROMPT_BLACK_BANANA_TEX (CANVAS_PATH "powerup_blackbanana.png")
#define PROMPT_APPLE_TEX (CANVAS_PATH "powerup_apple.png")
#define PROMPT_ORANGE_TEX (CANVAS_PATH "powerup_orange.png")
#define PROMPT_KEY_TEX (CANVAS_PATH "promptDeliverKey.png")
#define PROMPT_GET_CAKE_TEX (CANVAS_PATH "promptGetCake.png")
#define PROMPT_DROP_ITEM_TEX (CANVAS_PATH "promptDropItem.png")


#ifndef UICANVAS_H
#define UICANVAS_H
class UICanvas {
public:
	enum UIType {
		UI_BASE = 0,
		RACCOON_HAPPY = 1,
		CAT_HAPPY = 2,
		DOG_HAPPY = 3,
		RACCOON_JAIL = 4,
		CAT_JAIL = 5,
		DOG_JAIL = 6,
		RACCOON_ITEM = 7,
		CAT_ITEM = 8,
		DOG_ITEM = 9,
		CAKE_ITEM = 10,
		YELLOW_KEY_ITEM = 11,
		BLUE_KEY_ITEM = 12,
		GREEN_KEY_ITEM = 13,
		PLUNGER_ITEM = 14,
		ROPE_ITEM = 15,
		TOILET_PAPER_ITEM = 16,
		RED_SCREWDRIVER_ITEM = 17,
		GREEN_SCREWDRIVER_ITEM = 18,
		YELLOW_SCREWDRIVER_ITEM = 19,
		APPLE_ITEM = 20,
		ORANGE_ITEM = 21,
		BANANA_YELLOW_ITEM = 22,
		GOALS = 23,
		YELLOW_KEY_GOAL_INCOMPLETE = 24,
		BLUE_KEY_GOAL_INCOMPLETE = 25,
		GREEN_KEY_GOAL_INCOMPLETE = 26,
		YELLOW_KEY_GOAL_COMPLETE = 27,
		BLUE_KEY_GOAL_COMPLETE = 28,
		GREEN_KEY_GOAL_COMPLETE = 29,
		RED_SCREWDRIVER_GOAL_INCOMPLETE = 30,
		GREEN_SCREWDRIVER_GOAL_INCOMPLETE = 31,
		YELLOW_SCREWDRIVER_GOAL_INCOMPLETE = 32,
		RED_SCREWDRIVER_GOAL_COMPLETE = 33,
		GREEN_SCREWDRIVER_GOAL_COMPLETE = 34,
		YELLOW_SCREWDRIVER_GOAL_COMPLETE = 35,
		PLUNGER_GOAL_INCOMPLETE = 36,
		ROPE_GOAL_INCOMPLETE = 37,
		TOILET_PAPER_GOAL_INCOMPLETE = 38,
		PLUNGER_GOAL_COMPLETE = 39,
		ROPE_GOAL_COMPLETE = 40,
		TOILET_PAPER_GOAL_COMPLETE = 41,
		PROMPT_JAIL_RESCUE = 42,
		PROMPT_SWING_NET = 43,
		PROMPT_BOX_SEARCH = 44,
		PROMPT_GREEN_BANANA = 45,
		PROMPT_YELLOW_BANANA = 46,
		PROMPT_BLACK_BANANA = 47,
		PROMPT_ORANGE = 48,
		PROMPT_APPLE = 49,
		BANANA_GREEN_ITEM = 50,
		BANANA_BLACK_ITEM = 51,
		PROMPT_KEY = 52,
		PROMPT_GET_CAKE = 53,
		PROMPT_DROP_ITEM = 54,
	};
	void draw(glm::mat4 * V, glm::mat4 * P, glm::mat4 model);
	void setAngerRatio(float newAngerRatio);
	void setVisible(UIType item, bool visible);
	void setItem(UIType item);
	void removeItems();
	UICanvas(GLuint shaderProgram);
	~UICanvas();
private:
	bool visibleItems[NUM_ITEMS];
	class UIObject *uiItems[NUM_ITEMS];
	GLuint shaderProgram;
	float angerRatio;

};

#endif
