#include "ControllerWrapper.h"


namespace gui
{

ControllerWrapper::ControllerWrapper(Controller::Ptr controller):
	ObjectWrapper(controller),
	m_controller(controller)
{
	// add external properties, depending on the controller
	if( std::dynamic_pointer_cast<ConstantController<float>>(controller) )
	{
		ConstantController<float>::Ptr controllert = std::dynamic_pointer_cast<ConstantController<float>>(controller);
		addExternalProperty( PropertyT<float>::create("value", std::bind(&ConstantController<float>::getValue, controllert.get()), std::bind(&ConstantController<float>::setValue, controllert.get(), std::placeholders::_1)));
	}else
	if( std::dynamic_pointer_cast<ConstantController<math::V3f>>(controller) )
	{
		ConstantController<math::V3f>::Ptr controllert = std::dynamic_pointer_cast<ConstantController<math::V3f>>(controller);
		addExternalProperty( PropertyT<math::V3f>::create("value", std::bind(&ConstantController<math::V3f>::getValue, controllert.get()), std::bind(&ConstantController<math::V3f>::setValue, controllert.get(), std::placeholders::_1)));
	}

}

ControllerWrapper::~ControllerWrapper()
{

}

ControllerWrapper::Ptr ControllerWrapper::create(Controller::Ptr controller)
{
	return std::make_shared<ControllerWrapper>(controller);
}


} // namespace gui
