#include "BaseApplication.h" //TODO: sort out includes...
#include "Blobb.h"

Blobb::Blobb(Ogre::SceneManager* sceneMgr, NxOgre::Scene* nxScene, Vector3 position, BV3D::TEAM team) //TODO: add color param?
		: mSceneMgr(sceneMgr), mTeam(team), mNxScene(nxScene) {
	//TODO: load mesh, create collision shapes
			Entity* ent = mSceneMgr->createEntity(mTeam==BV3D::TEAM1 ? "Blobb1":"Blobb2", "Blobb.mesh"); //TODO: entity name...what when more than 2 blobbs?
    SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	//Vector3 blobb1_position = Vector3(-BV3D::ARENA_EXTENT[0]/2,1.0,0.0);
	node->translate(position);
	node->attachObject(ent);

	NxOgre::NodeRenderableParams nrp;
	nrp.setToDefault();
	nrp.mIdentifierUsage = NxOgre::NodeRenderableParams::IU_Use;
	nrp.mIdentifier = node->getName();


	//TODO: replace with 2 spheres...
	//NxOgre::Resources::ResourceSystem::getSingleton()->addMeshAs("file://../../media/blobb.nxs", "blobb.nxs"); //TODO: workaround, -> ogre resource system??
	//mNxScene->createBody<NxOgre::Body>("blobb",
	//	new NxOgre::TriangleMesh(NxOgre::Resources::ResourceSystem::getSingleton()->getMesh("blobb.nxs")), 
	//	position, nrp, "mass:10");

	NxOgre::CompoundShape* cs = new NxOgre::CompoundShape();
	cs->add(new NxOgre::Sphere(BV3D::BLOBB_SHAPE_DATA[0][1]+0.05)); //lower horizontal radius
	NxOgre::Sphere* upperSphere = new NxOgre::Sphere(BV3D::BLOBB_SHAPE_DATA[0][3]);
	upperSphere->setLocalPose(NxOgre::Pose(0, 0.7, 0));
	cs->add(upperSphere); //upper horizontal radius
	mNxScene->createBody<NxOgre::Body>("blobb", cs, position, nrp, "mass:10");
}