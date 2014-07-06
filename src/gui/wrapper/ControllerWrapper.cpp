#include "ControllerWrapper.h"

// LoadVolumeWrapper
#include <QFile>
#include "../Application.h"

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





//  loadGeometry ----------------------




LoadGeometryWrapper::LoadGeometryWrapper(LoadGeometry::Ptr loadGeometry):
	ControllerWrapper(loadGeometry),
	m_loadGeometry(loadGeometry)
{
	addExternalProperty( PropertyT<std::string>::create("filename",
						std::bind(&LoadGeometryWrapper::getFilename, this),
						std::bind(&LoadGeometryWrapper::setFilename, this, std::placeholders::_1)));

	//std::string newFilename = base::expand(m_loadVolume->getFilename());
	//if( QFile(QString::fromStdString(newFilename)).exists() )
	//	Application::getInstance()->watchFile(newFilename, std::bind( &LoadVolumeWrapper::reload, this ));
}

LoadGeometryWrapper::~LoadGeometryWrapper()
{

}

LoadGeometryWrapper::Ptr LoadGeometryWrapper::create(LoadGeometry::Ptr loadGeometry)
{
	return std::make_shared<LoadGeometryWrapper>(loadGeometry);
}

void LoadGeometryWrapper::setFilename(const std::string &filename)
{
	std::string currentFilename = base::expand(m_loadGeometry->getFilename());
	//Application::getInstance()->unwatchFile(currentFilename);

	m_loadGeometry->setFilename(filename);

	//updatePropertyList();

	//std::string newFilename = base::expand(m_loadVolume->getFilename());
	//if( QFile(QString::fromStdString(newFilename)).exists() )
	//	Application::getInstance()->watchFile(newFilename, std::bind( &LoadVolumeWrapper::reload, this ));
}

std::string LoadGeometryWrapper::getFilename()
{
	return m_loadGeometry->getFilename();
}

void LoadGeometryWrapper::reload()
{
	std::cout << "LoadGeometryWrapper::reloading file\n";
	m_loadGeometry->setFilename(m_loadGeometry->getFilename());
	Application::getInstance()->getGlViewer()->update();
}


//  loadVolume ----------------------




LoadVolumeWrapper::LoadVolumeWrapper(LoadVolume::Ptr loadVolume):
	ControllerWrapper(loadVolume),
	m_loadVolume(loadVolume)
{
	addExternalProperty( PropertyT<std::string>::create("filename",
						std::bind(&LoadVolumeWrapper::getFilename, this),
						std::bind(&LoadVolumeWrapper::setFilename, this, std::placeholders::_1)));

	std::string newFilename = base::expand(m_loadVolume->getFilename());
	if( QFile(QString::fromStdString(newFilename)).exists() )
		Application::getInstance()->watchFile(newFilename, std::bind( &LoadVolumeWrapper::reload, this ));
}

LoadVolumeWrapper::~LoadVolumeWrapper()
{

}

LoadVolumeWrapper::Ptr LoadVolumeWrapper::create(LoadVolume::Ptr loadVolume)
{
	return std::make_shared<LoadVolumeWrapper>(loadVolume);
}

void LoadVolumeWrapper::setFilename(const std::string &filename)
{
	std::string currentFilename = base::expand(m_loadVolume->getFilename());
	Application::getInstance()->unwatchFile(currentFilename);

	m_loadVolume->setFilename(filename);

	std::string newFilename = base::expand(m_loadVolume->getFilename());
	if( QFile(QString::fromStdString(newFilename)).exists() )
		Application::getInstance()->watchFile(newFilename, std::bind( &LoadVolumeWrapper::reload, this ));
}

std::string LoadVolumeWrapper::getFilename()
{
	return m_loadVolume->getFilename();
}

void LoadVolumeWrapper::reload()
{
	std::cout << "reloading file!\n";
	m_loadVolume->setFilename(m_loadVolume->getFilename());
	Application::getInstance()->getGlViewer()->update();
}




//  loadTexture2d ----------------------




LoadTexture2dWrapper::LoadTexture2dWrapper(LoadTexture2d::Ptr loadTexture2d):
	ControllerWrapper(loadTexture2d),
	m_loadTexture2d(loadTexture2d)
{
	addExternalProperty( PropertyT<std::string>::create("filename",
						std::bind(&LoadTexture2dWrapper::getFilename, this),
						std::bind(&LoadTexture2dWrapper::setFilename, this, std::placeholders::_1)));

	//std::string newFilename = base::expand(m_loadVolume->getFilename());
	//if( QFile(QString::fromStdString(newFilename)).exists() )
	//	Application::getInstance()->watchFile(newFilename, std::bind( &LoadVolumeWrapper::reload, this ));
}

LoadTexture2dWrapper::~LoadTexture2dWrapper()
{

}

LoadTexture2dWrapper::Ptr LoadTexture2dWrapper::create(LoadTexture2d::Ptr loadTexture2d)
{
	return std::make_shared<LoadTexture2dWrapper>(loadTexture2d);
}

void LoadTexture2dWrapper::setFilename(const std::string &filename)
{
	std::string currentFilename = base::expand(m_loadTexture2d->getFilename());
	//Application::getInstance()->unwatchFile(currentFilename);

	m_loadTexture2d->setFilename(filename);

	//std::string newFilename = base::expand(m_loadVolume->getFilename());
	//if( QFile(QString::fromStdString(newFilename)).exists() )
	//	Application::getInstance()->watchFile(newFilename, std::bind( &LoadVolumeWrapper::reload, this ));
}

std::string LoadTexture2dWrapper::getFilename()
{
	return m_loadTexture2d->getFilename();
}

void LoadTexture2dWrapper::reload()
{
	std::cout << "LoadTexture2dWrapper::reloading file\n";
	m_loadTexture2d->setFilename(m_loadTexture2d->getFilename());
	Application::getInstance()->getGlViewer()->update();
}




//  loadShader ----------------------




LoadShaderWrapper::LoadShaderWrapper(LoadShader::Ptr loadShader):
	ControllerWrapper(loadShader),
	m_loadShader(loadShader)
{
	addExternalProperty( PropertyT<std::string>::create("filename",
						std::bind(&LoadShaderWrapper::getFilename, this),
						std::bind(&LoadShaderWrapper::setFilename, this, std::placeholders::_1)));

	if( m_loadShader->getShader() )
	{
		std::vector<std::string> shaderFiles;
		m_loadShader->getShader()->getFiles(shaderFiles);
		for( auto file:shaderFiles )
		{
			if( QFile(QString::fromStdString(file)).exists() )
				Application::getInstance()->watchFile(file, std::bind( &LoadShaderWrapper::reload, this ));
		}
	}


}

LoadShaderWrapper::~LoadShaderWrapper()
{

}

LoadShaderWrapper::Ptr LoadShaderWrapper::create(LoadShader::Ptr loadShader)
{
	return std::make_shared<LoadShaderWrapper>(loadShader);
}

void LoadShaderWrapper::setFilename(const std::string &filename)
{
	if( m_loadShader->getShader() )
	{
		std::vector<std::string> shaderFiles;
		m_loadShader->getShader()->getFiles(shaderFiles);
		for( auto file:shaderFiles )
		{
			if( QFile(QString::fromStdString(file)).exists() )
				Application::getInstance()->unwatchFile(file);
		}
	}

	m_loadShader->setFilename(filename);

	updatePropertyList();

	if( m_loadShader->getShader() )
	{
		std::vector<std::string> shaderFiles;
		m_loadShader->getShader()->getFiles(shaderFiles);
		for( auto file:shaderFiles )
		{
			if( QFile(QString::fromStdString(file)).exists() )
				Application::getInstance()->watchFile(file, std::bind( &LoadShaderWrapper::reload, this ));
		}
	}
}

std::string LoadShaderWrapper::getFilename()
{
	return m_loadShader->getFilename();
}

void LoadShaderWrapper::reload()
{
	std::cout << "LoadShaderWrapper::reloading\n";
	m_loadShader->reload();//setFilename(m_loadShader->getFilename());
	updatePropertyList();
	emit propertyReferenceChanged();
	Application::getInstance()->getGlViewer()->update();
}
} // namespace gui
