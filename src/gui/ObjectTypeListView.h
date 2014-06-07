#pragma once

#include <memory>
#include "widgets/ListWidget/ListWidget.h"
#include "wrapper/ObjectWrapper.h"


namespace gui
{
	class ObjectTypeListView : QObject
	{
		Q_OBJECT
	public:
		typedef std::shared_ptr<ObjectTypeListView> Ptr;

		ObjectTypeListView();
		~ObjectTypeListView();


		static Ptr create();
		QWidget* getWidget();



		QMimeData* getMimeData( QListWidgetItem* item );
		ObjectWrapper::Ptr getObjectWrapper(QListWidgetItem *item);


	public slots:
		void filterTextChanged(const QString & text);









	private:
		QStringList m_typeNames;
		QWidget* m_widget;
		ListWidget* m_listWidget;
	};
}
