#include "Application.h"


#include <QSplitter>
#include <QTreeView>
#include <QMenuBar>
#include <QFrame>


#include <iostream>



namespace gui
{

	Application::Application( int argc, char **argv, widgets::GLViewer::InitCallback init, widgets::GLViewer::ShutdownCallback shutdown, widgets::GLViewer::RenderCallback render ) : QApplication(argc, argv)
	{
		// demo
		m_demoWrapper = DemoWrapper::create();

		// main window ---
		m_mainWindow = new QMainWindow();
		m_mainWindow->resize(800, 600);


		// tree view ---
		m_treeView = TreeView::create(m_demoWrapper);

		// tab widget for graph views ---
		m_tabWidget = new QTabWidget();

		m_splitter = new QSplitter();
		m_splitter->addWidget(m_treeView->m_widget);
		m_splitter->addWidget(m_tabWidget);


		// file watcher ---
		connect( &m_fileWatcher, SIGNAL(fileChanged(QString)), this, SLOT(fileChanged(QString)) );


		// gl viewer ---
		m_glviewer = new gui::widgets::GLViewer(init, shutdown, render);


		m_mainWindow->setCentralWidget( m_glviewer );


		m_splitter->show();
		// its important to show gui at the very last, as this triggers glview init
		// which in turn triggers demo loading...
		m_mainWindow->show();
	}

	Application::~Application()
	{
	}

	ObjectWrapper::Ptr Application::getWrapper(Object::Ptr object)
	{
		auto it = m_objectWrapper.find(object);
		if(it!=m_objectWrapper.end())
			return it->second;

		ObjectWrapper::Ptr wrapper;

		if(std::dynamic_pointer_cast<Element>(object))
			wrapper = ElementWrapper::create(std::dynamic_pointer_cast<Element>(object));
		else
			wrapper = ObjectWrapper::create( object );

		if(wrapper)
			m_objectWrapper[object] = wrapper;

		return wrapper;
	}

	UpdateGraphWrapper::Ptr Application::getWrapper( UpdateGraph::Ptr graph )
	{
		auto it = m_updateGraphWrapper.find(graph);
		if(it!=m_updateGraphWrapper.end())
			return it->second;
		UpdateGraphWrapper::Ptr graphWrapper = UpdateGraphWrapper::create(graph);
		m_updateGraphWrapper[graph] = graphWrapper;
		return graphWrapper;
	}

	QMainWindow* Application::getMainWindow()
	{
		return m_mainWindow;
	}

	widgets::GLViewer *Application::getGlViewer()
	{
		return m_glviewer;
	}

	DemoWrapper::Ptr Application::getDemoWrapper()
	{
		return m_demoWrapper;
	}

	void Application::openGraphEditor(UpdateGraphWrapper::Ptr graph)
	{
		UpdateGraphView* ugv = new UpdateGraphView(graph);
		m_updateGraphViews.push_back(ugv);
		m_tabWidget->addTab( ugv->m_view, "nebulae" );
	}


	Application* Application::getInstance()
	{
		return (Application*)instance();
	}


	void Application::watchFile(const std::string &filename, Application::FileChangedCallback callback)
	{
		bool test = m_fileWatcher.addPath( QString::fromStdString(filename) );
		m_fileChangedCallbacks[filename] = callback;
	}

	void Application::fileChanged(const QString &path)
	{
		std::cout << "fileChanged: " << path.toStdString() << std::endl;
		std::string filename = path.toStdString();
		auto it = m_fileChangedCallbacks.find(filename);
		if(it!=m_fileChangedCallbacks.end())
			it->second();
	}



}
