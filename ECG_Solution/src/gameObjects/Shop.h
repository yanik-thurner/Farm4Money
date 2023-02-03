#pragma once
#include "Interactable.h"
#include "plants\TomatoSeed.h"
#include "plants\CornSeed.h"
#include "plants\CarrotSeed.h"
#include "plants\MelonSeed.h"
#include "BackpackUpgrade.h"
#include "GraphicsCard.h"
#include "Collidable.h"

class Shop :
	public Interactable, Collidable
{
private:
	static physx::PxShape* shape;

	bool shopping = false;
	std::vector<std::unique_ptr<Item>> inventory;
	std::vector<int> icon_mark_times;
	static const int icon_mark_time = 10;

	// general settings
	int char_length = 29;
	int char_height = 34;

	// bar settings
	int width = 300;
	int height = 600;
	int margin = 100;
	

	// shop text settings
	int shop_text_scale = 1;
	std::string shop_text = "SHOP";
	int shop_text_y_offset = 20;

	// button settings
	int button_margin_top = 50;
	int button_margin = 40;
	int button_size = 80;
	int cols = 2;

	// button text settings
	float button_text_size_scale = 0.10f;
	int button_text_margin = 5;
	float button_text_scale = (float(button_size)*button_text_size_scale) / char_height;

	// colors
	glm::vec4 selected_color = glm::vec4(0.624, 0.635, 0.667, 0.8);
	glm::vec4 quad_color = glm::vec4(0.824, 0.835, 0.867, 0.8);
	glm::vec4 bar_color = glm::vec4(0.722, 0.729, 0.812, 0.8);
	glm::vec3 money_color = glm::vec3(0.6, 0.761, 0.635);
	glm::vec3 text_color = glm::vec3(0, 0, 0);

	
public:
	Shop(glm::vec3 size, glm::vec3 position);
	~Shop();

	// Inherited via Interactable
	virtual void apply_model() override;
	virtual void update(double timediff, glm::vec3 dir) override;
	virtual void draw(glm::vec3 cam_pos, float view_frustum) override;
	virtual void interact(Player & p) override;

	void render(TextRender& r, int window_width, int window_height);
	void buy(int id, Player &p);
};

