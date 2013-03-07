/* ------------------------------------------------------------------------------------ *
 * Impementation of the GameWorld class													*
 * Filename: gameWorld.cpp																*
 * ------------------------------------------------------------------------------------ *
 */


#include "gameWorld.h"


GameWorld::GameWorld(SceneManager *m){
	// initialize the variables
	mgr = m; // Pointer to SceneManager
}

void GameWorld::createWorld(){
	// Set ambient light
	mgr->setAmbientLight(ColourValue(0.5, 0.5, 0.5));
	// enable shadows
	mgr->setShadowTechnique(SHADOWTYPE_TEXTURE_ADDITIVE);// SHADOWTYPE_TEXTURE_ADDITIVE

	Light* l = mgr->createLight("SunLight");
	l->setType(Light::LT_DIRECTIONAL);
	l->setDirection(-0.5, -0.5, 0);
	l->setCastShadows(true);
	
	// add 2 Trees
	Entity *tree1, *tree2;
	SceneNode *snT1, *snT2;

	// Tree 1
	tree1 = mgr->createEntity("tree1", "tree1.tga_tree1.tga.mesh");
	tree1->setMaterialName("base/tree1");
	tree1->setCastShadows(true);
	snT1 = mgr->getRootSceneNode()->createChildSceneNode("tree1Node", Vector3(394.852, 62.298, 394.198));
	snT1->attachObject(tree1);
	snT1->rotate(Vector3::NEGATIVE_UNIT_X, Degree(90));
	snT1->rotate(Vector3::UNIT_Z, Degree(36));

	mgr->setSkyDome(true, "Examples/CloudySky", 5, 8, 1000);

	// set fog
	//ColourValue fadeColour(0.9, 0.9, 0.9);
	//mgr->setFog(FOG_LINEAR, fadeColour, 0.01, 200, 400);
	//win->getViewport(0)->setBackgroundColour(fadeColour);
}

GameWorld::~GameWorld(){	
}
