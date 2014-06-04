#include "TreeView.h"

#include "TreeViewSceneItem.h"
#include "TreeViewShotItem.h"

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
	tb->addAction("new shot", this, SLOT(newShot()));
	layout->addWidget( tb );

	// treewidget --
	m_treeWidget = new TreeWidget();
	m_treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	m_treeWidget->setDragEnabled(true);
	layout->addWidget( m_treeWidget );

	m_widget->show();


	connect( m_demoWrapper.get(), SIGNAL(sceneAdded(int)), this, SLOT(onSceneAdded(int)) );
	connect( m_demoWrapper.get(), SIGNAL(shotAdded(int)), this, SLOT(onShotAdded(int)) );
	connect( m_treeWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(onCustomContextMenuRequested(const QPoint&)) );
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

void TreeView::onShotAdded(int index)
{
	// add a new TreeViewScene item representing the scene in the treeview
	TreeViewShotItem* item = new TreeViewShotItem( m_demoWrapper->getShotWrapper( index ) );
	m_treeWidget->addTopLevelItem(item);
}

void TreeView::loadScene()
{
	QString fileName = QFileDialog::getOpenFileName(m_widget,
		"Load Scene",
		QString::fromStdString(base::expand("$DATA")),
		"Scene Files(*.scn)");
	if(!fileName.isEmpty())
	{
		m_demoWrapper->loadScene( fileName.toStdString() );
	}
	else
		std::cout << "no file selected!";
}

void TreeView::newShot()
{
	m_demoWrapper->addShot(ShotWrapper::create());
}

void TreeView::onCustomContextMenuRequested(const QPoint &pos)
{
	std::cout << "custom context menu requested\n";
	QTreeWidgetItem* item = m_treeWidget->itemAt(pos);
	if (!item)
		return;

	if( dynamic_cast<TreeViewShotItem*>(item) )
		dynamic_cast<TreeViewShotItem*>(item)->contextMenu(m_treeWidget->viewport()->mapToGlobal(pos));
}

}
