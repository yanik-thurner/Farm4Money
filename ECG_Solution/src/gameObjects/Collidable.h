#pragma once
#include "SceneObject.h"
#include "PhysX\PxPhysicsAPI.h"

class Collidable {
	

public:
	Collidable(Model model, glm::vec3 position) : position(position), model(model) {
		verticies = std::make_unique<physx::PxVec3[]>(model.getMeshes()[0].positions.size());
		for (int i = 0; i < model.getMeshes()[0].positions.size(); i++) {
			*(verticies.get() + i) = physx::PxVec3(model.getMeshes()[0].positions[i].x, model.getMeshes()[0].positions[i].y, model.getMeshes()[0].positions[i].z);
		}

		indices = std::make_unique<physx::PxU32[]>(model.getMeshes()[0].indices.size());
		for (int i = 0; i < model.getMeshes()[0].indices.size(); i++) {
			*(indices.get() + i) = physx::PxU32(model.getMeshes()[0].indices[i]);
		}
	}
	~Collidable() {}

	static physx::PxPhysics* gPhysicsSDK;
	static physx::PxScene *gScene;
	static physx::PxCooking *cooking;

	std::unique_ptr<physx::PxVec3[]> verticies;
	std::unique_ptr<physx::PxU32[]> indices;
	glm::vec3 position;
	Model model;

	physx::PxShape *generate_shape() {
		physx::PxTriangleMeshDesc meshDesc;
		meshDesc.points.count = physx::PxU32(model.getMeshes()[0].positions.size());
		meshDesc.points.stride = sizeof(physx::PxVec3);
		meshDesc.points.data = verticies.get();

		meshDesc.triangles.count = physx::PxU32(model.getMeshes()[0].indices.size() / 3);
		meshDesc.triangles.stride = 3 * sizeof(physx::PxU32);
		meshDesc.triangles.data = indices.get();

		physx::PxDefaultMemoryOutputStream writeBuffer;
		physx::PxTriangleMeshCookingResult::Enum *result = nullptr;
		bool status = SceneObject::cooking->cookTriangleMesh(meshDesc, writeBuffer, result);
		if (!status)
			std::cout << "COOKING ERROR" << std::endl;

		physx::PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());

		physx::PxTriangleMeshGeometry triGeom;
		triGeom.triangleMesh = SceneObject::gPhysicsSDK->createTriangleMesh(readBuffer);
		triGeom.scale = physx::PxMeshScale(physx::PxVec3(0.5, 0.5, 0.5));
		
		physx::PxMaterial* gMaterial = gPhysicsSDK->createMaterial(0.5, 0.5, 0.5);
		physx::PxShape* meshShape = gPhysicsSDK->createShape(triGeom, *gMaterial, false);
		meshShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);

		return meshShape;
	}

	physx::PxRigidStatic* generate_actor(physx::PxShape *shape) {
		physx::PxTransform x = physx::PxTransform();
		physx::PxTransform fieldPos = physx::PxTransform(physx::PxVec3(position.x, position.y - 0.5, position.z));
		physx::PxRigidStatic* meshActor = gPhysicsSDK->createRigidStatic(fieldPos);
		meshActor->attachShape(*shape);
		return meshActor;
	}
};