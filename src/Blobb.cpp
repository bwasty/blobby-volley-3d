#include <OgreSceneManager.h>
#include <OgreEntity.h>
#include <OgreSubEntity.h>

#include <NxOgre.h>
//#include "OGRE3DCommon.h"
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
	blobbSpheres.insert(new NxOgre::Sphere(BV3D::BLOBB_SHAPE_DATA[0][1]+0.05)); //TODO!!: define and set material for shapes, make dynamically changeable
	blobbSpheres.insert(new NxOgre::Sphere(BV3D::BLOBB_SHAPE_DATA[0][3]));
	NxOgre::RigidBodyDescription rbd;
	rbd.mMass = 10.0;
	mBody = mApp->mNxRenderSystem->createBody(blobbSpheres, NxOgre::Real3(position.x, position.y, position.z), "Blobb.mesh", rbd);

	//set colour - have to clone material
	Entity* ent = mBody->getEntity();
	MaterialPtr mat = ent->getSubEntity(0)->getMaterial();
	mat = mat->clone(ent->getName());
	ent->setMaterial(mat);
	mat->getTechnique(0)->getPass(0)->setAmbient(colour);
	mat->getTechnique(0)->getPass(0)->setDiffuse(colour);


	// create Joint
	NxScene* realNxScene = mApp->mNxRenderSystem->getScene()->getScene();
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

	d6Desc.projectionMode = NX_JPM_NONE; //TODO: needed?
	mD6Joint=(NxD6Joint*)realNxScene->createJoint(d6Desc);


	//// old code for bleeding
	//// load mesh, create collision shapes
	//Entity* ent = mSceneMgr->createEntity(mTeam==BV3D::TEAM1 ? "Blobb1":"Blobb2", "Blobb.mesh"); //TODO: entity name...what when more than 2 blobbs?
 //   SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	////Vector3 blobb1_position = Vector3(-BV3D::ARENA_EXTENT[0]/2,1.0,0.0);
	//node->translate(position);
	//node->attachObject(ent);

	////set colour - have to clone material
	//MaterialPtr mat = ent->getSubEntity(0)->getMaterial();
	//mat = mat->clone(ent->getName());
	//ent->setMaterial(mat);
	//mat->getTechnique(0)->getPass(0)->setAmbient(colour);
	//mat->getTechnique(0)->getPass(0)->setDiffuse(colour);
	//
	//NxOgre::NodeRenderableParams nrp;
	//nrp.setToDefault();
	//nrp.mIdentifierUsage = NxOgre::NodeRenderableParams::IU_Use;
	//nrp.mIdentifier = node->getName();

	//NxOgre::CompoundShape* cs = new NxOgre::CompoundShape();
	//cs->add(new NxOgre::Sphere(BV3D::BLOBB_SHAPE_DATA[0][1]+0.05)); //lower horizontal radius
	//NxOgre::Sphere* upperSphere = new NxOgre::Sphere(BV3D::BLOBB_SHAPE_DATA[0][3]);
	//upperSphere->setLocalPose(NxOgre::Pose(0, 0.7, 0));
	//cs->add(upperSphere); //upper horizontal radius
	//mActor = mNxScene->createBody<NxOgre::Body>("blobb", cs, position, nrp, "mass:10");

	//// create Joint
	//NxScene* realNxScene = mNxScene->getNxScene();
	//NxActor* realActor = mActor->getNxActor();

	//NxD6JointDesc d6Desc;
 //   d6Desc.actor[0] = realActor;    
	//d6Desc.actor[1] = 0;
 //   //d6Desc.setGlobalAnchor(globalAnchor);//Important when DOFs are unlocked.    
	////d6Desc.setGlobalAxis(globalAxis);//Joint configuration.    

	//// lock all rotational axes, so that Blobb doesn't topple over
	//d6Desc.twistMotion = NX_D6JOINT_MOTION_LOCKED;    
	//d6Desc.swing1Motion = NX_D6JOINT_MOTION_LOCKED;    
	//d6Desc.swing2Motion = NX_D6JOINT_MOTION_LOCKED;    

 //   d6Desc.projectionMode = NX_JPM_NONE; //TODO: needed?
 //   mD6Joint=(NxD6Joint*)realNxScene->createJoint(d6Desc);
}

void Blobb::move(Ogre::Vector2 direction) {
	//mActor->addForce(direction.x, 0, direction.y);
	mBody->addForce(NxOgre::Real3(direction.x, 0, direction.y));
}

void Blobb::jump(float height) {
	//TODO!: prevent Blobb from jumping when already in the air
	//mActor->addForce(0, height, 0);
	mBody->addForce(NxOgre::Real3(0, height, 0));
}