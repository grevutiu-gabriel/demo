#include "LensFlareWrapper.h"

#include <QMouseEvent>

#include "../../Application.h"





namespace gui
{


LensFlareWrapper::LensFlareWrapper( LensFlare::Ptr fshop ):m_fshop(fshop)
{
	//connect( &m_transferFunction, SIGNAL(Changed()), this, SLOT(transferFunctionChanged()) );

	Application::getInstance()->getGlViewer()->installEventFilter(this);
}

bool LensFlareWrapper::eventFilter(QObject *obj, QEvent *event)
{
	if( event->type() == QEvent::MouseMove )
	{
		QMouseEvent* e = dynamic_cast<QMouseEvent*>(event);
		if(e && e->buttons()&Qt::LeftButton)
		{
			int mx = e->x();
			int my = e->y();
			// convert to -1, 1
			float x = float(mx)/float(Application::getInstance()->getGlViewer()->width())*2.0f - 1.0f;
			float y = float(my)/float(Application::getInstance()->getGlViewer()->height())*2.0f - 1.0f;
			m_fshop->setLightPos(math::V3f(x, y, 0.0f));
			emit changed();
			return true;
		}
	}
	return false;
}

LensFlareWrapper::Ptr LensFlareWrapper::create(LensFlare::Ptr fs)
{
	return std::make_shared<LensFlareWrapper>( fs );
}

}//namespace gui
