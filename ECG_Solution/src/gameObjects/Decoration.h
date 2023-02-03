#pragma once
#include "SceneObject.h"
#include "Collidable.h"

class Decoration :
	public SceneObject, public Collidable
{
private:
	physx::PxShape* shape;
	bool vf_affected = true;
	
public:
	Decoration(glm::vec3 position, std::vector<Model> models, bool vf_affected);
	~Decoration();

	void setModelMatVec(std::vector<std::shared_ptr<Material>> mats);

	// Inherited via SceneObject
	virtual void apply_model() override;
	virtual void update(double timediff, glm::vec3 dir) override;
	virtual void draw(glm::vec3 cam_pos, float view_frustum) override;
};

