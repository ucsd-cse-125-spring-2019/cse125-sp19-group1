#include "UICanvas.h"

UICanvas::UICanvas(GLuint shaderProgram) {
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
	uiItems[CAT_JAIL] = new UIObject(CAT_JAIL_TEX);
	uiItems[DOG_JAIL] = new UIObject(DOG_JAIL_TEX);
	uiItems[RACCOON_ITEM] = new UIObject(RACCOON_ITEM_TEX);
	uiItems[CAT_ITEM] = new UIObject(CAT_ITEM_TEX);
	uiItems[DOG_ITEM] = new UIObject(DOG_ITEM_TEX);
	uiItems[CAKE_ITEM] = new UIObject(CAKE_ITEM_TEX);
	uiItems[YELLOW_KEY_ITEM] = new UIObject(YELLOW_KEY_ITEM_TEX);
	uiItems[BLUE_KEY_ITEM] = new UIObject(BLUE_KEY_ITEM_TEX);
	uiItems[GREEN_KEY_ITEM] = new UIObject(GREEN_KEY_ITEM_TEX);
	uiItems[PLUNGER_ITEM] = new UIObject(PLUNGER_ITEM_TEX);
	uiItems[ROPE_ITEM] = new UIObject(ROPE_ITEM_TEX);
	uiItems[TOILET_PAPER_ITEM] = new UIObject(TOILET_PAPER_ITEM_TEX);
	uiItems[RED_SCREWDRIVER_ITEM] = new UIObject(RED_SCREWDRIVER_ITEM_TEX);
	uiItems[GREEN_SCREWDRIVER_ITEM] = new UIObject(GREEN_SCREWDRIVER_ITEM_TEX);
	uiItems[YELLOW_SCREWDRIVER_ITEM] = new UIObject(YELLOW_SCREWDRIVER_ITEM_TEX);

}

UICanvas::~UICanvas() {
	for (int i = 0; i < NUM_ITEMS; i++) {
		delete(uiItems[i]);
	}
}

void UICanvas::draw(glm::mat4 * V, glm::mat4 * P, glm::mat4 model) {
	for (int i = 0; i < NUM_ITEMS; i++) {
		if (visibleItems[i]) {
			uiItems[i]->draw(shaderProgram, V, P, model);
		}
	}
}