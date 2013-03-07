#include "ObjectTitle.h"
#include <OgreFontManager.h>

using namespace Ogre;

ObjectTitle::ObjectTitle(const String& name, MovableObject* object, Camera* camera, const String& title, 
	const String& fontName, const Ogre::ColourValue& color) : object(object), camera(camera)
{
	overlay = OverlayManager::getSingleton().create(name+"_TitleOverlay");
	container = (OverlayContainer*)OverlayManager::getSingleton().createOverlayElement("Panel", name+"_TitleContainer");
	overlay->add2D(container);

	textArea = OverlayManager::getSingleton().createOverlayElement("TextArea", name+"_TitleTextArea");
	textArea->setDimensions(0.8, 0.8);
	textArea->setMetricsMode(GMM_PIXELS);
	textArea->setPosition(0.1, 0.1);

	FontManager::getSingleton().load(fontName, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	font = (Font*)FontManager::getSingleton().getByName(fontName).getPointer();
	textArea->setParameter("font_name", fontName);
	textArea->setParameter("char_height", font->getParameter("size"));
	textArea->setParameter("horz_align", "left");
	textArea->setColour(color);
	textArea->setCaption(title);

	container->addChild(textArea);
	overlay->show();

	textDim = getTextDimensions(title);
	container->setDimensions(textDim.x, textDim.y);
}

ObjectTitle::~ObjectTitle()
{
	OverlayManager *overlayManager = OverlayManager::getSingletonPtr();
	container->removeChild(textArea->getName());
	overlay->remove2D(container);
	overlayManager->destroyOverlayElement(textArea);
	overlayManager->destroyOverlayElement(container);
	overlayManager->destroy(overlay);
}

void ObjectTitle::setTitle(const String& title)
{
	textArea->setCaption(title);
	textDim = getTextDimensions(title);
	container->setDimensions(textDim.x, textDim.y);
}

void ObjectTitle::update()
{
	if(!object->isInScene())
	{
		overlay->hide();
		return;
	}

	// Derive the average point between the top-most corners of the object's bounding box
	const Ogre::AxisAlignedBox &AABB = object->getWorldBoundingBox(true);
	Ogre::Vector3 point = (AABB.getCorner(AxisAlignedBox::FAR_LEFT_TOP)
		+ AABB.getCorner(AxisAlignedBox::FAR_RIGHT_TOP)
		+ AABB.getCorner(AxisAlignedBox::NEAR_LEFT_TOP)
		+ AABB.getCorner(AxisAlignedBox::NEAR_RIGHT_TOP)) / 4;

	// Is the camera facing that point? If not, hide the overlay and return.
	Ogre::Plane cameraPlane = Plane(Vector3(camera->getDerivedOrientation().zAxis()), camera->getDerivedPosition());
	if(cameraPlane.getSide(point) != Plane::NEGATIVE_SIDE)
	{
		overlay->hide();
		return;
	}
	
	// Derive the 2D screen-space coordinates for that point
	point = camera->getProjectionMatrix() * (camera->getViewMatrix() * point);

	// Transform from coordinate space [-1, 1] to [0, 1]
	Real x = (point.x / 2) + 0.5f;
	Real y = 1 - ((point.y / 2) + 0.5f);

	// Update the position (centering the text)
	container->setPosition(x - (textDim.x / 2), y);
	overlay->show();
}

Vector2 ObjectTitle::getTextDimensions(String text)
{
	Real charHeight = StringConverter::parseReal(font->getParameter("size"));

	Vector2 result(0, 0);

	for(String::iterator i = text.begin(); i < text.end(); i++)
	{   
		if (*i == 0x0020)
			result.x += font->getGlyphAspectRatio(0x0030);
		else
			result.x += font->getGlyphAspectRatio(*i);
	}

	result.x = (result.x * charHeight) / (float)camera->getViewport()->getActualWidth();
	result.y = charHeight / (float)camera->getViewport()->getActualHeight();

	return result;
}
