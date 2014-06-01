#include "ShotWrapper.h"

#include "../Application.h"
namespace gui
{

	ShotWrapper::ShotWrapper( Shot::Ptr shot ):m_shot(shot)
	{
	}

	ShotWrapper::Ptr ShotWrapper::create()
	{
		Shot::Ptr shot = Shot::create();
		shot->setName("new shot");
		return std::make_shared<ShotWrapper>(shot);
	}

	Shot::Ptr ShotWrapper::getShot()
	{
		return m_shot;
	}


	std::string ShotWrapper::getName() const
	{
		return m_shot->getName();
	}

} // namespace gui
