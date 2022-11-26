#ifndef DRAWABLEOBJECT_H
#define DRAWABLEOBJECT_H

#include "Scene.hpp"

class Drawable
{
	public:
		virtual void draw() = 0;
		virtual void setGui() 
		{

		};

		virtual void update() 
		{

		};

		static Scene* scene;
};

#endif