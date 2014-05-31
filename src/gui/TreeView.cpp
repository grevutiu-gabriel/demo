#include "TreeView.h"

#include "TreeViewSceneItem.h"

#include <QVBoxLayout>
#include <QToolBar>
#include <QFileDialog>

namespace gui
{



TreeView::Ptr TreeView::create( DemoWrapper::Ptr demoWrapper)
{
	return std::make_shared<TreeView>( demoWrapper );
}

TreeView::TreeView(DemoWrapper::Ptr demoWrapper) :
	QObject(),
	m_demoWrapper(demoWrapper)
{
	// layout --
	m_widget = new QWidget();
	QVBoxLayout *layout = new QVBoxLayout();
	m_widget->setLayout(layout);

	// toolbar
	QToolBar* tb=new QToolBar();
	tb->addAction("load scene", this, SLOT(loadScene()));
	layout->addWidget( tb );

	// treewidget --
	m_treeWidget = new QTreeWidget();
	layout->addWidget( m_treeWidget );

	m_widget->show();
	connect( m_demoWrapper.get(), SIGNAL(sceneAdded(int)), this, SLOT(onSceneAdded(int)) );
}

TreeView::~TreeView()
{
	delete m_treeWidget;
}

void TreeView::onSceneAdded(int index)
{
	// add a new TreeViewScene item representing the scene in the treeview
	TreeViewSceneItem* item = new TreeViewSceneItem( m_demoWrapper->getSceneWrapper( index ) );
	m_treeWidget->addTopLevelItem(item);
}

void TreeView::loadScene()
{
	std::cout << "load scene!\n";
	QString fileName = QFileDialog::getOpenFileName(m_widget,
		"Load Scene",
		QString::fromStdString(base::expand("$DATA")),
		"Scene Files(*.scn)");
	std::cout << "test: " << fileName.size() << std::endl;
	if(!fileName.isEmpty())
	{
		m_demoWrapper->loadScene( fileName.toStdString() );
		std::cout << "file selected: " << fileName.toStdString() << std::endl;
	}
	else
		std::cout << "no file selected!";
	//std::cout << "no file selected >" << fileName.toStdString() << "<\n";

}

}
