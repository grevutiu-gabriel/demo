#include "ObjectTypeListView.h"


#include <QVBoxLayout>
#include <QLineEdit>
#include <QRegExp>

#include "../framework/Object.h"
#include "Application.h"

#include "wrapper/ObjectWrapperMimeData.h"


namespace gui
{

ObjectTypeListView::ObjectTypeListView():
	QObject()
{
	m_widget = new QWidget();

	// layout --
	QVBoxLayout *layout = new QVBoxLayout();
	m_widget->setLayout(layout);

	// filter input
	QLineEdit* edit = new QLineEdit();
	layout->addWidget(edit);

	// listwidget
	m_listWidget = new ListWidget();
	m_listWidget->setDragEnabled(true);
	layout->addWidget(m_listWidget);

	// propagate typenames
	std::vector<std::string> classNames;
	ObjectFactory::getClassNames(classNames);

	for( auto className:classNames )
		m_classNames.push_back(QString::fromStdString(className));
	m_listWidget->addItems(m_classNames);
	m_listWidget->getMimeData = std::bind( &ObjectTypeListView::getMimeData, this, std::placeholders::_1 );

	connect( edit, SIGNAL(textChanged(QString)), this, SLOT(filterTextChanged(QString)) );
}

ObjectTypeListView::~ObjectTypeListView()
{
	delete m_widget;
}

ObjectTypeListView::Ptr ObjectTypeListView::create()
{
	return std::make_shared<ObjectTypeListView>();
}

QWidget *ObjectTypeListView::getWidget()
{
	return m_widget;
}

QMimeData *ObjectTypeListView::getMimeData(QListWidgetItem *item)
{
	return new ObjectWrapperMimeData( item->text().toStdString(), std::bind( &ObjectTypeListView::getObjectWrapper, this, item ), true );
}

ObjectWrapper::Ptr ObjectTypeListView::getObjectWrapper(QListWidgetItem *item)
{
	// get typename from item
	std::string type = item->text().toStdString();
	// create object
	Object::Ptr object = ObjectFactory::create(type);
	object->setName(type);
	// wrap it up
	ObjectWrapper::Ptr objectWrapper = Application::getInstance()->getWrapper(object);
	return objectWrapper;
}

void ObjectTypeListView::filterTextChanged(const QString &text)
{
	QRegExp regExp( text, Qt::CaseInsensitive, QRegExp::Wildcard );
	m_listWidget->clear();
	m_listWidget->addItems( m_classNames.filter(regExp) );
}

}
