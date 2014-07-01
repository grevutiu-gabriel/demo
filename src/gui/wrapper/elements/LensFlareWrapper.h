#pragma once

#include <QObject>

#include "../framework/elements/LensFlare/LensFlare.h"


namespace gui
{

class LensFlareWrapper : public QObject
{
	Q_OBJECT
public:
	typedef std::shared_ptr<LensFlareWrapper> Ptr;

	LensFlareWrapper( LensFlare::Ptr fshop );

	static Ptr create( LensFlare::Ptr fs );

	virtual bool eventFilter( QObject* obj, QEvent* event )override;

signals:
	void changed();
public slots:
public:
	LensFlare::Ptr m_fshop;
};

} // namespace gui
