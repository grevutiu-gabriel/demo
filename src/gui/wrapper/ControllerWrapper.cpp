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




//  loadVolume ----------------------




LoadVolumeWrapper::LoadVolumeWrapper(LoadVolume::Ptr loadVolume):
	ControllerWrapper(loadVolume),
	m_loadVolume(loadVolume)
{
	addExternalProperty( PropertyT<std::string>::create("filename", std::bind(&LoadVolume::getFilename, m_loadVolume.get()), std::bind(&LoadVolume::setFilename, m_loadVolume.get(), std::placeholders::_1)));
}

LoadVolumeWrapper::~LoadVolumeWrapper()
{

}

LoadVolumeWrapper::Ptr LoadVolumeWrapper::create(LoadVolume::Ptr loadVolume)
{
	return std::make_shared<LoadVolumeWrapper>(loadVolume);
}

} // namespace gui
