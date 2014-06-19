#pragma once


#include "ObjectWrapper.h"
#include "../framework/Controller.h"
#include "../framework/controller/LoadVolume.h"


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

	class LoadVolumeWrapper : public ControllerWrapper
	{
		Q_OBJECT
	public:
		typedef std::shared_ptr<LoadVolumeWrapper> Ptr;

		LoadVolumeWrapper( LoadVolume::Ptr controller );
		virtual ~LoadVolumeWrapper();
		static Ptr create( LoadVolume::Ptr controller );

		void setFilename( const std::string& filename );
		std::string getFilename();
		void reload();

	private:
		LoadVolume::Ptr m_loadVolume;
	};



} // namespace gui
