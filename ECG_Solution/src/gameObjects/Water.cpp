#include "Water.h"



Water::Water(Player *p, glm::vec3 size, glm::vec3 position) : Interactable(SceneObject::model_manager->get_model(SceneObject::model_manager->WATER), glm::mat4(1.0f), position), p(p)
{
	physx::PxMaterial* gMaterial = SceneObject::gPhysicsSDK->createMaterial(0.5, 0.5, 0.5);
	physx::PxShape* triggerShape = SceneObject::gPhysicsSDK->createShape(physx::PxSphereGeometry(23), *gMaterial, true);
	;
	triggerShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
	triggerShape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, true);

	physx::PxTransform fieldPos = physx::PxTransform(physx::PxVec3(position.x, position.y, position.z));
	set_actor(gPhysicsSDK->createRigidStatic(fieldPos));

	actor->attachShape(*triggerShape);
	actor->userData = static_cast<void*>(this);

	transform_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(position.x * 2, position.y * 2, position.z * 2));
	current_model.transform(transform_matrix);

	
}


Water::~Water()
{
}

void Water::apply_model()
{
}

void Water::update(double timediff, glm::vec3 dir)
{
	total_time += timediff;
	if (total_time >= 0.2) {
		animation_switch = !animation_switch;
		total_time = 0;
	}

	if (p && p->fishing) {
		dt_sum += timediff;
		if (dt_sum >= 1 / calcs_per_sec) {
			int r = std::rand() % int(1 / (chance_per_sec / calcs_per_sec));
			if (r == 0 && fish_bite_timer == 0) {
				fish_bite_timer = double(std::rand() % 30)/10 + 1;
			}
			dt_sum = 0;
		}

		if (fish_bite_timer != 0) {
			if (fish_bite_timer - timediff >= 0) {
				fish_bite_timer -= timediff;
			}
			else {
				fish_bite_timer = 0;
			}
		}
	}
	else {
		fish_bite_timer = 0;
	}
}

void Water::draw(glm::vec3 cam_pos, float view_frustum)
{
	current_model.draw(cam_pos, view_frustum);
}

void Water::interact(Player & p)
{
	if (!p.fishing) {
		p.fishing = this;
	}
	else {
		p.fishing = nullptr;
	}

	if(fish_bite_timer != 0){
		p.inventory.insert(std::move(std::make_unique<Fish>(Fish())));
		fish_bite_timer = 0;
	}
}

void Water::render(TextRender & r, int window_width, int window_height)
{
	int width = 100;
	int height = 100;
	int margin_top = 50;

	int x_offset = (window_width - width) / 2;
	int y_offset = window_height - height - margin_top;

	if (fish_bite_timer != 0 && animation_switch)
		r.renderquad("rod_caught1", glm::vec2(x_offset, y_offset), glm::vec2(width, height), glm::vec4(1, 1, 1, 1));
	else if (fish_bite_timer != 0 && !animation_switch)
		r.renderquad("rod_caught2", glm::vec2(x_offset, y_offset), glm::vec2(width, height), glm::vec4(1, 1, 1, 1));
	else if (p->fishing) {
		r.renderquad("rod", glm::vec2(x_offset, y_offset), glm::vec2(width, height), glm::vec4(1,1,1,1));
	}
	


}
