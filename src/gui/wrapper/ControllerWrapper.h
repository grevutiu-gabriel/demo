#pragma once


#include "ObjectWrapper.h"
#include "../framework/Controller.h"


namespace gui
{

	class ControllerWrapper : public ObjectWrapper
	{
		Q_OBJECT
	public:
		typedef std::shared_ptr<ControllerWrapper> Ptr;

		ControllerWrapper( Controller::Ptr controller );
		virtual ~ControllerWrapper();
		static Ptr create( Controller::Ptr controller );

	private:
		Controller::Ptr m_controller;
	};

} // namespace gui
