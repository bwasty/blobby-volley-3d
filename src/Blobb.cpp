#include "BaseApplication.h" //TODO: sort out includes...
#include "Blobb.h"
#include <OgreSceneManager.h>
#include <OgreEntity.h>
#include <OgreSubEntity.h>

Blobb::Blobb(Ogre::SceneManager* sceneMgr, NxOgre::Scene* scene, Vector3 position, BV3D::TEAM team, Ogre::ColourValue colour)
		: mSceneMgr(sceneMgr), mTeam(team), mNxScene(scene) {
	// load mesh, create collision shapes
	Entity* ent = mSceneMgr->createEntity(mTeam==BV3D::TEAM1 ? "Blobb1":"Blobb2", "Blobb.mesh"); //TODO: entity name...what when more than 2 blobbs?
    SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	//Vector3 blobb1_position = Vector3(-BV3D::ARENA_EXTENT[0]/2,1.0,0.0);
	node->translate(position);
	node->attachObject(ent);

	//set colour - have to clone material
	MaterialPtr mat = ent->getSubEntity(0)->getMaterial();
	mat = mat->clone(ent->getName());
	ent->setMaterial(mat);
	mat->getTechnique(0)->getPass(0)->setAmbient(colour);
	mat->getTechnique(0)->getPass(0)->setDiffuse(colour);
	
	NxOgre::NodeRenderableParams nrp;
	nrp.setToDefault();
	nrp.mIdentifierUsage = NxOgre::NodeRenderableParams::IU_Use;
	nrp.mIdentifier = node->getName();

	//NxOgre::Resources::ResourceSystem::getSingleton()->addMeshAs("file://../../media/blobb.nxs", "blobb.nxs");
	//mNxScene->createBody<NxOgre::Body>("blobb",
	//	new NxOgre::TriangleMesh(NxOgre::Resources::ResourceSystem::getSingleton()->getMesh("blobb.nxs")), 
	//	position, nrp, "mass:10");

	NxOgre::CompoundShape* cs = new NxOgre::CompoundShape();
	cs->add(new NxOgre::Sphere(BV3D::BLOBB_SHAPE_DATA[0][1]+0.05)); //lower horizontal radius
	NxOgre::Sphere* upperSphere = new NxOgre::Sphere(BV3D::BLOBB_SHAPE_DATA[0][3]);
	upperSphere->setLocalPose(NxOgre::Pose(0, 0.7, 0));
	cs->add(upperSphere); //upper horizontal radius
	mActor = mNxScene->createBody<NxOgre::Body>("blobb", cs, position, nrp, "mass:10");

	// create Joint
	NxScene* realNxScene = mNxScene->getNxScene();
	NxActor* realActor = mActor->getNxActor();

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
}

void Blobb::move(Ogre::Vector2 direction) {
	mActor->addForce(direction.x, 0, direction.y);
}

void Blobb::jump(float height) {
	//TODO!: prevent Blobb from jumping when already in the air
	mActor->addForce(0, height, 0);
}