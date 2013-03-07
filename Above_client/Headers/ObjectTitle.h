#ifndef __ObjectTitle_H__
#define __ObjectTitle_H__

#include "Ogre.h"

class ObjectTitle
{
	const Ogre::MovableObject* object;
	const Ogre::Camera* camera;
	Ogre::Overlay* overlay;
	Ogre::OverlayElement* textArea;
	Ogre::OverlayContainer* container;
	Ogre::Vector2 textDim;
	Ogre::Font* font;

	Ogre::Vector2 getTextDimensions(Ogre::String text);
public:

	ObjectTitle(const Ogre::String& name, Ogre::MovableObject* object, Ogre::Camera* camera, const Ogre::String& title,
		const Ogre::String& fontName, const Ogre::ColourValue& color = Ogre::ColourValue::White);

	~ObjectTitle();

	void setTitle(const Ogre::String& title);

	void update();
};

#endif
