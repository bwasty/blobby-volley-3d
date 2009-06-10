#include <OgreSceneManager.h>
#include <OgreEntity.h>
#include <OgreSubEntity.h>

#include <NxOgre.h>
#include <OGRE3DBody.h>
#include <OGRE3DRenderSystem.h>
#include "NxScene.h"
#include "NxD6JointDesc.h"

#include "Application.h"
#include "Blobb.h"

using namespace Ogre;

Blobb::Blobb(Application* app, Ogre::SceneManager* sceneMgr, NxOgre::Scene* scene, Vector3 position, BV3D::TEAM team, Ogre::ColourValue colour)
		: mApp(app), mSceneMgr(sceneMgr), mTeam(team), mNxScene(scene) {
	// create the 2 compound spheres
	NxOgre::Shapes blobbSpheres;
	//TODO!!: define and set material for shapes, make dynamically changeable
	//TODO!!!!: sphere's don't work correctly, see visual debugger
	blobbSpheres.insert(new NxOgre::Sphere(BV3D::BLOBB_SHAPE_DATA[0][1]+0.05)); //lower horizontal radius
	blobbSpheres.insert(new NxOgre::Sphere(BV3D::BLOBB_SHAPE_DATA[0][3]));		//upper horizontal radius
	blobbSpheres[0]->setLocalPose(NxOgre::Matrix44(NxOgre::Real3(0, 0.7, 0)));
	NxOgre::RigidBodyDescription rbd;
	rbd.mMass = 10.0;
	mBody = mApp->getPhysicsRenderSystem()->createBody(blobbSpheres, NxOgre::Real3(position.x, position.y, position.z), "Blobb.mesh", rbd);

	//set colour - have to clone material
	Entity* ent = mBody->getEntity();
	MaterialPtr mat = ent->getSubEntity(0)->getMaterial();
	mat = mat->clone(ent->getName());
	ent->setMaterial(mat);
	mat->getTechnique(0)->getPass(0)->setAmbient(colour);
	mat->getTechnique(0)->getPass(0)->setDiffuse(colour);

	// create Joint
	NxScene* realNxScene = mApp->getPhysicsRenderSystem()->getScene()->getScene();
	NxActor* realActor = mBody->getNxActor();

	NxD6JointDesc d6Desc;
	   d6Desc.actor[0] = realActor;    
	d6Desc.actor[1] = 0;
	//d6Desc.setGlobalAnchor(globalAnchor);//Important when DOFs are unlocked.    
	//d6Desc.setGlobalAxis(globalAxis);//Joint configuration.    

	// lock all rotational axes, so that Blobb doesn't topple over
	d6Desc.twistMotion = NX_D6JOINT_MOTION_LOCKED;    
	d6Desc.swing1Motion = NX_D6JOINT_MOTION_LOCKED;    
	d6Desc.swing2Motion = NX_D6JOINT_MOTION_LOCKED;    

	d6Desc.projectionMode = NX_JPM_NONE; //TODO: d6Desc.projectionMode = NX_JPM_NONE; -  needed?
	mD6Joint=(NxD6Joint*)realNxScene->createJoint(d6Desc);
}

void Blobb::move(Ogre::Vector2 direction) {
	mBody->addForce(NxOgre::Real3(direction.x, 0, direction.y));
}

void Blobb::jump(float force) {
	if (mBody->getGlobalPosition().y < 1.1) //TODO: Blobb::jump - height criterion too weak?
		mBody->addForce(NxOgre::Real3(0, force, 0));
}