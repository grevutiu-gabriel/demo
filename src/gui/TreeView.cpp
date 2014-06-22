#include "TreeView.h"

#include "TreeViewSceneItem.h"
#include "TreeViewCompositionItem.h"

#include <QVBoxLayout>
#include <QToolBar>
#include <QFileDialog>


#include "Application.h"

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
	tb->addAction("load", this, SLOT(load()));
	tb->addAction("save", this, SLOT(save()));
	tb->addAction("load scene", this, SLOT(loadScene()));
	tb->addAction("new composition", this, SLOT(newComposition()));
	layout->addWidget( tb );

	// treewidget --
	m_treeWidget = new TreeWidget();
	m_treeWidget->setColumnCount(1);
	m_treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	m_treeWidget->setDragEnabled(true);
	m_treeWidget->viewport()->setAcceptDrops(true);
	layout->addWidget( m_treeWidget );


	//m_widget->setMaximumWidth(250);


	connect( m_demoWrapper.get(), SIGNAL(sceneAdded(int)), this, SLOT(onSceneAdded(int)) );
	connect( m_demoWrapper.get(), SIGNAL(compositionAdded(int)), this, SLOT(onCompositionAdded(int)) );
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

void TreeView::onCompositionAdded(int index)
{
	// add a new TreeViewScene item representing the scene in the treeview
	TreeViewCompositionItem* item = new TreeViewCompositionItem( m_demoWrapper->getCompositionWrapper( index ) );
	m_treeWidget->addTopLevelItem(item);
}

void TreeView::load()
{
	QString fileName = QFileDialog::getOpenFileName(m_widget,
		"Load Demo",
		QString::fromStdString(base::expand("$DATA")),
		"Demo Files(*.dmo)");
	if(!fileName.isEmpty())
	{
		m_demoWrapper->load( fileName.toStdString() );
	}
	else
		std::cout << "no file selected!";
}

void TreeView::save()
{
	QString fileName = QFileDialog::getSaveFileName(m_widget,
		"Save Demo",
		QString::fromStdString(base::expand("$DATA")),
		"Demo Files(*.dmo)");
	if(!fileName.isEmpty())
	{
		m_demoWrapper->save( fileName.toStdString() );
	}
	else
		std::cout << "no file selected!";
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

void TreeView::newComposition()
{
	m_demoWrapper->addComposition(CompositionWrapper::create());
	Application::getInstance()->getGlViewer()->update();
}

void TreeView::onCustomContextMenuRequested(const QPoint &pos)
{
	std::cout << "custom context menu requested\n";
	QTreeWidgetItem* item = m_treeWidget->itemAt(pos);
	if (!item)
		return;

	if( dynamic_cast<TreeWidgetItem*>(item) )
		dynamic_cast<TreeWidgetItem*>(item)->contextMenu(m_treeWidget->viewport()->mapToGlobal(pos));
}

void TreeView::onItemDoubleClicked(QTreeWidgetItem *item, int column)
{
	if( dynamic_cast<TreeViewCompositionItem*>(item) )
		dynamic_cast<TreeViewCompositionItem*>(item)->doubleClick(column);
}

}
