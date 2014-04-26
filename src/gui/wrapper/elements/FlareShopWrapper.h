#pragma once

#include <QObject>

#include "../framework/elements/FlareShop/FlareShop.h"


namespace gui
{

class FlareShopWrapper : public QObject
{
	Q_OBJECT
public:
	typedef std::shared_ptr<FlareShopWrapper> Ptr;

	FlareShopWrapper( FlareShop::Ptr fshop );

	static Ptr create( FlareShop::Ptr fs );

	virtual bool eventFilter( QObject* obj, QEvent* event )override;

signals:
	void changed();
public slots:
public:
	FlareShop::Ptr m_fshop;
};

} // namespace gui
