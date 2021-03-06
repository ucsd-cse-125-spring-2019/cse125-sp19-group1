#include "UICanvas.h"

UICanvas::UICanvas(GLuint shaderProgram) {
	angerRatio = 0.25f;
	this->shaderProgram = shaderProgram;
	uiItems[UI_BASE] = new UIObject(UI_BASE_TEX);
	visibleItems[UI_BASE] = true;
	uiItems[RACCOON_HAPPY] = new UIObject(RACCOON_HAPPY_TEX);
	visibleItems[RACCOON_HAPPY] = true;
	uiItems[CAT_HAPPY] = new UIObject(CAT_HAPPY_TEX);
	visibleItems[CAT_HAPPY] = true;
	uiItems[DOG_HAPPY] = new UIObject(DOG_HAPPY_TEX);
	visibleItems[DOG_HAPPY] = true;
	uiItems[RACCOON_JAIL] = new UIObject(RACCOON_JAIL_TEX);
	visibleItems[RACCOON_JAIL] = false;
	uiItems[CAT_JAIL] = new UIObject(CAT_JAIL_TEX);
	visibleItems[CAT_JAIL] = false;
	uiItems[DOG_JAIL] = new UIObject(DOG_JAIL_TEX);
	visibleItems[DOG_JAIL] = false;
	uiItems[RACCOON_ITEM] = new UIObject(RACCOON_ITEM_TEX);
	visibleItems[RACCOON_ITEM] = false;
	uiItems[CAT_ITEM] = new UIObject(CAT_ITEM_TEX);
	visibleItems[CAT_ITEM] = false;
	uiItems[DOG_ITEM] = new UIObject(DOG_ITEM_TEX);
	visibleItems[DOG_ITEM] = false;
	uiItems[CAKE_ITEM] = new UIObject(CAKE_ITEM_TEX);
	visibleItems[CAKE_ITEM] = false;
	uiItems[YELLOW_KEY_ITEM] = new UIObject(YELLOW_KEY_ITEM_TEX);
	visibleItems[YELLOW_KEY_ITEM] = false;
	uiItems[BLUE_KEY_ITEM] = new UIObject(BLUE_KEY_ITEM_TEX);
	visibleItems[BLUE_KEY_ITEM] = false;
	uiItems[GREEN_KEY_ITEM] = new UIObject(GREEN_KEY_ITEM_TEX);
	visibleItems[GREEN_KEY_ITEM] = false;
	uiItems[PLUNGER_ITEM] = new UIObject(PLUNGER_ITEM_TEX);
	visibleItems[PLUNGER_ITEM] = false;
	uiItems[ROPE_ITEM] = new UIObject(ROPE_ITEM_TEX);
	visibleItems[ROPE_ITEM] = false;
	uiItems[TOILET_PAPER_ITEM] = new UIObject(TOILET_PAPER_ITEM_TEX);
	visibleItems[TOILET_PAPER_ITEM] = false;
	uiItems[RED_SCREWDRIVER_ITEM] = new UIObject(RED_SCREWDRIVER_ITEM_TEX);
	visibleItems[RED_SCREWDRIVER_ITEM] = false;
	uiItems[GREEN_SCREWDRIVER_ITEM] = new UIObject(GREEN_SCREWDRIVER_ITEM_TEX);
	visibleItems[GREEN_SCREWDRIVER_ITEM] = false;
	uiItems[YELLOW_SCREWDRIVER_ITEM] = new UIObject(YELLOW_SCREWDRIVER_ITEM_TEX);
	visibleItems[YELLOW_SCREWDRIVER_ITEM] = false;
	uiItems[APPLE_ITEM] = new UIObject(APPLE_ITEM_TEX);
	visibleItems[APPLE_ITEM] = false;
	uiItems[ORANGE_ITEM] = new UIObject(ORANGE_ITEM_TEX);
	visibleItems[ORANGE_ITEM] = false;
	uiItems[BANANA_YELLOW_ITEM] = new UIObject(BANANA_YELLOW_ITEM_TEX);
	visibleItems[BANANA_YELLOW_ITEM] = false;
	uiItems[BANANA_GREEN_ITEM] = new UIObject(BANANA_GREEN_ITEM_TEX);
	visibleItems[BANANA_GREEN_ITEM] = false;
	uiItems[BANANA_BLACK_ITEM] = new UIObject(BANANA_BLACK_ITEM_TEX);
	visibleItems[BANANA_BLACK_ITEM] = false;
	uiItems[GOALS] = new UIObject(GOALS_TEX);
	visibleItems[GOALS] = false;
	uiItems[YELLOW_KEY_GOAL_INCOMPLETE] = new UIObject(YELLOW_KEY_GOAL_INCOMPLETE_TEX);
	visibleItems[YELLOW_KEY_GOAL_INCOMPLETE] = true;
	uiItems[YELLOW_KEY_GOAL_COMPLETE] = new UIObject(YELLOW_KEY_GOAL_COMPLETE_TEX);
	visibleItems[YELLOW_KEY_GOAL_COMPLETE] = false;
	uiItems[BLUE_KEY_GOAL_INCOMPLETE] = new UIObject(BLUE_KEY_GOAL_INCOMPLETE_TEX);
	visibleItems[BLUE_KEY_GOAL_INCOMPLETE] = true;
	uiItems[BLUE_KEY_GOAL_COMPLETE] = new UIObject(BLUE_KEY_GOAL_COMPLETE_TEX);
	visibleItems[BLUE_KEY_GOAL_COMPLETE] = false;
	uiItems[GREEN_KEY_GOAL_INCOMPLETE] = new UIObject(GREEN_KEY_GOAL_INCOMPLETE_TEX);
	visibleItems[GREEN_KEY_GOAL_INCOMPLETE] = true;
	uiItems[GREEN_KEY_GOAL_COMPLETE] = new UIObject(GREEN_KEY_GOAL_COMPLETE_TEX);
	visibleItems[GREEN_KEY_GOAL_COMPLETE] = false;
	uiItems[RED_SCREWDRIVER_GOAL_INCOMPLETE] = new UIObject(RED_SCREWDRIVER_GOAL_INCOMPLETE_TEX);
	visibleItems[RED_SCREWDRIVER_GOAL_INCOMPLETE] = true;
	uiItems[RED_SCREWDRIVER_GOAL_COMPLETE] = new UIObject(RED_SCREWDRIVER_GOAL_COMPLETE_TEX);
	visibleItems[RED_SCREWDRIVER_GOAL_COMPLETE] = false;
	uiItems[GREEN_SCREWDRIVER_GOAL_INCOMPLETE] = new UIObject(GREEN_SCREWDRIVER_GOAL_INCOMPLETE_TEX);
	visibleItems[GREEN_SCREWDRIVER_GOAL_INCOMPLETE] = true;
	uiItems[GREEN_SCREWDRIVER_GOAL_COMPLETE] = new UIObject(GREEN_SCREWDRIVER_GOAL_COMPLETE_TEX);
	visibleItems[GREEN_SCREWDRIVER_GOAL_COMPLETE] = false;
	uiItems[YELLOW_SCREWDRIVER_GOAL_INCOMPLETE] = new UIObject(YELLOW_SCREWDRIVER_GOAL_INCOMPLETE_TEX);
	visibleItems[YELLOW_SCREWDRIVER_GOAL_INCOMPLETE] = true;
	uiItems[YELLOW_SCREWDRIVER_GOAL_COMPLETE] = new UIObject(YELLOW_SCREWDRIVER_GOAL_COMPLETE_TEX);
	visibleItems[YELLOW_SCREWDRIVER_GOAL_COMPLETE] = false;
	uiItems[PLUNGER_GOAL_INCOMPLETE] = new UIObject(PLUNGER_GOAL_INCOMPLETE_TEX);
	visibleItems[PLUNGER_GOAL_INCOMPLETE] = true;
	uiItems[PLUNGER_GOAL_COMPLETE] = new UIObject(PLUNGER_GOAL_COMPLETE_TEX);
	visibleItems[PLUNGER_GOAL_COMPLETE] = false;
	uiItems[ROPE_GOAL_INCOMPLETE] = new UIObject(ROPE_GOAL_INCOMPLETE_TEX);
	visibleItems[ROPE_GOAL_INCOMPLETE] = true;
	uiItems[ROPE_GOAL_COMPLETE] = new UIObject(ROPE_GOAL_COMPLETE_TEX);
	visibleItems[ROPE_GOAL_COMPLETE] = false;
	uiItems[TOILET_PAPER_GOAL_INCOMPLETE] = new UIObject(TOILET_PAPER_GOAL_INCOMPLETE_TEX);
	visibleItems[TOILET_PAPER_GOAL_INCOMPLETE] = true;
	uiItems[TOILET_PAPER_GOAL_COMPLETE] = new UIObject(TOILET_PAPER_GOAL_COMPLETE_TEX);
	visibleItems[TOILET_PAPER_GOAL_COMPLETE] = false;
	uiItems[PROMPT_JAIL_RESCUE] = new UIObject(PROMPT_JAIL_RESCUE_TEX);
	visibleItems[PROMPT_JAIL_RESCUE] = false;
	uiItems[PROMPT_SWING_NET] = new UIObject(PROMPT_SWING_NET_TEX);
	visibleItems[PROMPT_SWING_NET] = false;
	uiItems[PROMPT_BOX_SEARCH] = new UIObject(PROMPT_BOX_SEARCH_TEX);
	visibleItems[PROMPT_BOX_SEARCH] = false;
	uiItems[PROMPT_GREEN_BANANA] = new UIObject(PROMPT_GREEN_BANANA_TEX);
	visibleItems[PROMPT_GREEN_BANANA] = false;
	uiItems[PROMPT_YELLOW_BANANA] = new UIObject(PROMPT_YELLOW_BANANA_TEX);
	visibleItems[PROMPT_YELLOW_BANANA] = false;
	uiItems[PROMPT_BLACK_BANANA] = new UIObject(PROMPT_BLACK_BANANA_TEX);
	visibleItems[PROMPT_BLACK_BANANA] = false;
	uiItems[PROMPT_ORANGE] = new UIObject(PROMPT_ORANGE_TEX);
	visibleItems[PROMPT_ORANGE] = false;
	uiItems[PROMPT_APPLE] = new UIObject(PROMPT_APPLE_TEX);
	visibleItems[PROMPT_APPLE] = false;
	uiItems[PROMPT_KEY] = new UIObject(PROMPT_KEY_TEX);
	visibleItems[PROMPT_KEY] = false;
	uiItems[PROMPT_GET_CAKE] = new UIObject(PROMPT_GET_CAKE_TEX);
	visibleItems[PROMPT_GET_CAKE] = false;
	uiItems[PROMPT_DROP_ITEM] = new UIObject(PROMPT_DROP_ITEM_TEX);
	visibleItems[PROMPT_DROP_ITEM] = false;
	uiItems[ANGRY_METER_1] = new UIObject(ANGRY_METER_1_TEX);
	visibleItems[ANGRY_METER_1] = true;
	uiItems[ANGRY_METER_2] = new UIObject(ANGRY_METER_2_TEX);
	visibleItems[ANGRY_METER_2] = false;
	uiItems[SPEED_BOOST] = new UIObject(SPEED_METER_TEX);
	visibleItems[SPEED_BOOST] = true;
	uiItems[PROMPT_BUILD_EXIT] = new UIObject(PROMPT_BUILD_EXIT_TEX);
	visibleItems[PROMPT_BUILD_EXIT] = false;
	uiItems[PROMPT_BOOST] = new UIObject(PROMPT_DASH_TEX);
	visibleItems[PROMPT_BOOST] = true;
	uiItems[PROMPT_CHANGE_VIEW] = new UIObject(PROMPT_CHANGE_VIEW_TEX);
	visibleItems[PROMPT_CHANGE_VIEW] = false;
	boostVisible = true;
}

void UICanvas::setItem(UIType item) {
	removeItems();
	visibleItems[item] = true;
	visibleItems[PROMPT_DROP_ITEM] = true;
}

void UICanvas::setItemChef(UIType item) {
	removeItems();
	visibleItems[item] = true;
}

void UICanvas::removeItems() {
	visibleItems[RACCOON_ITEM] = false;
	visibleItems[CAT_ITEM] = false;
	visibleItems[DOG_ITEM] = false;
	visibleItems[CAKE_ITEM] = false;
	visibleItems[YELLOW_KEY_ITEM] = false;
	visibleItems[BLUE_KEY_ITEM] = false;
	visibleItems[GREEN_KEY_ITEM] = false;
	visibleItems[PLUNGER_ITEM] = false;
	visibleItems[ROPE_ITEM] = false;
	visibleItems[TOILET_PAPER_ITEM] = false;
	visibleItems[RED_SCREWDRIVER_ITEM] = false;
	visibleItems[GREEN_SCREWDRIVER_ITEM] = false;
	visibleItems[YELLOW_SCREWDRIVER_ITEM] = false;
	visibleItems[APPLE_ITEM] = false;
	visibleItems[ORANGE_ITEM] = false;
	visibleItems[BANANA_GREEN_ITEM] = false;
	visibleItems[BANANA_YELLOW_ITEM] = false;
	visibleItems[BANANA_BLACK_ITEM] = false;
	visibleItems[PROMPT_DROP_ITEM] = false;
}

UICanvas::~UICanvas() {
	for (int i = 0; i < NUM_ITEMS; i++) {
		delete(uiItems[i]);
	}
}

void UICanvas::setAngerRatio(float newAngerRatio) {
	this->angerRatio = newAngerRatio;
}

void UICanvas::setBoostRatio(float newBoostRatio) {
	this->boostRatio = newBoostRatio;
}

void UICanvas::setVisible(UIType item, bool visible) {
	visibleItems[item] = visible;
}

void UICanvas::setBoostVisible(bool visible) {
	if (!visible) {
		setVisible(UICanvas::SPEED_BOOST, false);
		setVisible(UICanvas::PROMPT_BOOST, false);
		boostVisible = 0;
	}
	else {
		setVisible(UICanvas::SPEED_BOOST, true);
		boostVisible = 1;
	}
}

void UICanvas::draw(glm::mat4 * V, glm::mat4 * P, glm::mat4 model) {
	glDisable(GL_DEPTH_TEST);
	glUseProgram(shaderProgram);
	GLuint uAngryRatio = glGetUniformLocation(shaderProgram, "ratioAngry");
	GLuint uBoostRatio = glGetUniformLocation(shaderProgram, "ratioBoost");
	GLuint uBoostVisible = glGetUniformLocation(shaderProgram, "boostVisible");


	glUniform1f(uAngryRatio, (this->angerRatio));
	glUniform1f(uBoostRatio, (this->boostRatio));
	glUniform1i(uBoostVisible, (this->boostVisible));
	for (int i = 0; i < NUM_ITEMS; i++) {
		if (visibleItems[i]) {
			uiItems[i]->draw(shaderProgram, V, P, model);
		}
	}
}