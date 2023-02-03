#include "FarmSimulationEventCallback.h"
#include "gameObjects\Field.h"


FarmSimulationEventCallback::FarmSimulationEventCallback()
{
}


FarmSimulationEventCallback::~FarmSimulationEventCallback()
{
}

void FarmSimulationEventCallback::onConstraintBreak(physx::PxConstraintInfo * constraints, physx::PxU32 count)
{
}

void FarmSimulationEventCallback::onWake(physx::PxActor ** actors, physx::PxU32 count)
{
}

void FarmSimulationEventCallback::onSleep(physx::PxActor ** actors, physx::PxU32 count)
{
}

void FarmSimulationEventCallback::onContact(const physx::PxContactPairHeader & pairHeader, const physx::PxContactPair * pairs, physx::PxU32 nbPairs)
{
	std::cout << "contact" << std::endl;
}

void FarmSimulationEventCallback::onTrigger(physx::PxTriggerPair * pairs, physx::PxU32 count)
{
	for (int i = 0; i < count; i++) {
		Interactable* inter = static_cast<Interactable*> ((*(pairs + i)).triggerActor->userData);
		if (inter && (*(pairs + i)).status == physx::PxPairFlag::eNOTIFY_TOUCH_FOUND) {
			inter->playerInRange = true;
		}
		else if (inter && (*(pairs + i)).status == physx::PxPairFlag::eNOTIFY_TOUCH_LOST) {
			inter->playerInRange = false;
		}
	}
	
}

void FarmSimulationEventCallback::onAdvance(const physx::PxRigidBody * const * bodyBuffer, const physx::PxTransform * poseBuffer, const physx::PxU32 count)
{
}
