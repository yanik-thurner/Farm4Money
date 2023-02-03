#include "Shop.h"


physx::PxShape *Shop::shape;
Shop::Shop(glm::vec3 size, glm::vec3 position) : Interactable(SceneObject::model_manager->get_model(ModelManager::ModelID::SHOP), glm::mat4(1.0f), position), Collidable(current_model, position)
{
	if (!shape) shape = generate_shape();
	set_actor(generate_actor(shape));

	physx::PxMaterial* gMaterial = SceneObject::gPhysicsSDK->createMaterial(0.5, 0.5, 0.5);
	physx::PxShape* triggerShape = SceneObject::gPhysicsSDK->createShape(physx::PxBoxGeometry((size.x / 2) + 0.5, (size.y / 2) + 0.5, (size.z / 2) + 0.5), *gMaterial, true);

	triggerShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
	triggerShape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, true);

	actor->attachShape(*triggerShape);
	actor->userData = static_cast<void*>(this);

	transform_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(position.x * 2, position.y * 2, position.z * 2));
	current_model.transform(transform_matrix);

	// fill shop inventory
	inventory.push_back(std::move(std::make_unique<TomatoSeed>()));
	inventory.push_back(std::move(std::make_unique<CornSeed>()));
	inventory.push_back(std::move(std::make_unique<CarrotSeed>()));
	inventory.push_back(std::move(std::make_unique<MelonSeed>()));
	inventory.push_back(std::move(std::make_unique<BackpackUpgrade>()));
	inventory.push_back(std::move(std::make_unique<GraphicsCard>()));
	for (int i = 0; i < inventory.size(); i++) {
		icon_mark_times.push_back(0);
	}

}


Shop::~Shop()
{
}

void Shop::apply_model()
{
}

void Shop::update(double timediff, glm::vec3 dir)
{
	for (int i = 0; i < icon_mark_times.size(); i++) {
		if (icon_mark_times[i] - timediff < 0) {
			icon_mark_times[i] = 0;
		}
		else {
			icon_mark_times[i] = icon_mark_times[i] - timediff;
		}
	}
}

void Shop::draw(glm::vec3 cam_pos, float view_frustum)
{
	current_model.draw(cam_pos, view_frustum);

	
}


void Shop::render(TextRender& r, int window_width, int window_height)
{
	if (playerInRange) {
		if (shopping) {
			int x_offset = (window_width - width - margin);
			int y_offset = (window_height - height) / 2;
			
			// bar
			r.renderquad("quad", glm::vec2(x_offset, y_offset), glm::vec2(width, height), bar_color);

			//SHOP text
			r.renderText(shop_text, x_offset + (width - shop_text.size() * char_length) /2 , height + y_offset - char_height - shop_text_y_offset, 1, money_color);

			// Buttons
			int start_x = x_offset + (width - (button_size * cols + button_margin * (cols - 1))) / 2;
			int start_y = height + y_offset - char_height - shop_text_y_offset - button_size - button_margin_top;
			for (int i = 0; i < inventory.size(); i++) {
				int button_x = start_x + (i%cols)*(button_margin + button_size);
				int button_y = start_y - int((i) / cols) * (button_margin + button_size);

				if (icon_mark_times[i] != 0) 
					r.renderquad("quad", glm::vec2(button_x, button_y), glm::vec2(button_size, button_size), selected_color);
				else
					r.renderquad("quad", glm::vec2(button_x, button_y), glm::vec2(button_size, button_size), quad_color);

				r.renderquad(inventory[i]->toString(), glm::vec2(button_x, button_y), glm::vec2(button_size, button_size), quad_color);

				std::string text = std::to_string(i + 1) + ": $" + std::to_string(inventory[i]->get_value());
				r.renderText(text, button_x + (button_size - text.size()*char_length*button_text_scale)/2, button_y + button_text_margin, button_text_scale, text_color);
			}
		}
	}
	else {
		shopping = false;
	}
}

void Shop::interact(Player & p)
{
	shopping = !shopping;

}

void Shop::buy(int id, Player & p)
{
	if (!playerInRange || !shopping) return;

	icon_mark_times[id] = icon_mark_time;
	
	if (p.inventory.get_money() >= inventory[id]->get_value()) {
		if (dynamic_cast<TomatoSeed*>(inventory[id].get()))
			p.buy(std::move(std::make_unique<TomatoSeed>()));
		else if (dynamic_cast<CornSeed*>(inventory[id].get()))
			p.buy(std::move(std::make_unique<CornSeed>()));
		else if (dynamic_cast<CarrotSeed*>(inventory[id].get()))
			p.buy(std::move(std::make_unique<CarrotSeed>()));
		else if (dynamic_cast<MelonSeed*>(inventory[id].get()))
			p.buy(std::move(std::make_unique<MelonSeed>()));
		else if (dynamic_cast<GraphicsCard*>(inventory[id].get()))
			p.buy(std::move(std::make_unique<GraphicsCard>()));
		else if (dynamic_cast<BackpackUpgrade*>(inventory[id].get()))
			p.upgrade_backpack();

	}
}
