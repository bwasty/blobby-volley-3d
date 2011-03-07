#include "Blobb.h"

//#include <OgreSceneManager.h>
//#include <OgreEntity.h>
//#include <OgreSubEntity.h>

//#include <NxOgre.h>
#include <OGRE3DBody.h>
#include <OGRE3DRenderSystem.h>

#include "Application.h"


using namespace Ogre;

Blobb::Blobb(Application* app, Vector3 position, TEAM team, Ogre::ColourValue colour)
		: mApp(app), mTeam(team) {
	Real arenaScale = mApp->getConfig().getSettingReal("arenaScale");
	// create the 2 compound spheres that make up the physical blobb
	NxOgre::Shapes blobbSpheres;
	blobbSpheres.insert(new NxOgre::Sphere((BLOBB_SHAPE_DATA[0][1]+0.05) * arenaScale)); //lower horizontal radius
	blobbSpheres.insert(new NxOgre::Sphere((BLOBB_SHAPE_DATA[0][3])  * arenaScale));		//upper horizontal radius
	blobbSpheres[1]->setLocalPose(NxOgre::Matrix44(NxOgre::Vec3(0, 0.7  * arenaScale, 0)));

	blobbSpheres[0]->setMaterial(mApp->mBlobbPhysicsMaterial->getIdentifier());
	blobbSpheres[1]->setMaterial(mApp->mBlobbPhysicsMaterial->getIdentifier());

	NxOgre::RigidBodyDescription rbd;
	rbd.mBodyFlags |= NxOgre::Enums::BodyFlags_FreezeRotation;
	mBody = mApp->getPhysicsRenderSystem()->createBody(blobbSpheres, NxOgre::Vec3(position), "Blobb.mesh", rbd);
	mBody->getSceneNode()->scale(Vector3(arenaScale));
	loadSettings(); // sets mass

	//set colour - have to clone material
	Entity* ent = mBody->getEntity();
	MaterialPtr mat = ent->getSubEntity(0)->getMaterial();
	mat = mat->clone(ent->getName());
	ent->setMaterial(mat);
	mat->getTechnique(0)->getPass(0)->setAmbient(colour);
	mat->getTechnique(0)->getPass(0)->setDiffuse(colour);
}

void Blobb::loadSettings() {
	mBody->setMass(mApp->getConfig().getSettingReal("blobbMass"));
}

void Blobb::move(Ogre::Vector2 direction) {
	mBody->addForce(NxOgre::Vec3(direction.x, 0, direction.y));
}

void Blobb::jump() {
	NxOgre::Vec3 p = mBody->getGlobalPosition();
	Vector3 relativePosition = mApp->getArenaInverseTransform() * Vector3(p.x, p.y, p.z);
	if (relativePosition.y < 1.1) //TODO!: Blobb::jump - force criterion: strange behaviour, introduce jumpMode, which is left on ground touch?
		mBody->addForce(NxOgre::Vec3(0, mApp->getConfig().getSettingInt("BlobbJumpForce"), 0));
}
