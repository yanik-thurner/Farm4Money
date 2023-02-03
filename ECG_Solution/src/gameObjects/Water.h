#pragma once
#include "Interactable.h"
#include "Fish.h"

class Water :
	public Interactable
{
private:
	double dt_sum = 0;
	double fish_bite_timer = 0;

	const double calcs_per_sec = 10;
	const double chance_per_sec = 1.0 / 3.0;

	bool animation_switch = false;
	float total_time = 0;

	Player *p;
public:
	Water(Player *p, glm::vec3 size, glm::vec3 position);
	~Water();

	std::shared_ptr<Shader> shader;

	// Inherited via Interactable
	virtual void apply_model() override;
	virtual void update(double timediff, glm::vec3 dir) override;
	virtual void draw(glm::vec3 cam_pos, float view_frustum) override;
	virtual void interact(Player & p) override;
	void render(TextRender& r, int window_width, int window_height);
};

