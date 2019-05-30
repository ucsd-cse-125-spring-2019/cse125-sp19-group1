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
	uiItems[BANANA_ITEM] = new UIObject(BANANA_ITEM_TEX);
	visibleItems[BANANA_ITEM] = false;
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

}

void UICanvas::setItem(UIType item) {
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
	visibleItems[BANANA_ITEM] = false;
}

UICanvas::~UICanvas() {
	for (int i = 0; i < NUM_ITEMS; i++) {
		delete(uiItems[i]);
	}
}

void UICanvas::setAngerRatio(float newAngerRatio) {
	this->angerRatio = newAngerRatio;
}

void UICanvas::setVisible(UIType item, bool visible) {
	visibleItems[item] = visible;
}

void UICanvas::draw(glm::mat4 * V, glm::mat4 * P, glm::mat4 model) {
	glDisable(GL_DEPTH_TEST);
	glUseProgram(shaderProgram);
	GLuint uAngryRatio = glGetUniformLocation(shaderProgram, "ratioAngry");
	glUniform1f(uAngryRatio, (this->angerRatio));
	for (int i = 0; i < NUM_ITEMS; i++) {
		if (visibleItems[i]) {
			uiItems[i]->draw(shaderProgram, V, P, model);
		}
	}
}