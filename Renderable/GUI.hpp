#pragma once
#include "../CoreWorkerBackend/Window.hpp"
#include "Drawable.hpp"

#include <list>

class GUI : public Drawable
{
	public:
		GUI() 
		{

		};
		GUI(Window& w);
		~GUI();

		GUI& attach(Drawable* subscriber); // subscriber model

		virtual void draw();
		virtual void update();

		

	private:

		std::list<Drawable*> Attachments;

};

