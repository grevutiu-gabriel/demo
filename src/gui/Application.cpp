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

		// file watcher ---
		connect( &m_fileWatcher, SIGNAL(fileChanged(QString)), this, SLOT(fileChanged(QString)) );


		// gl viewer ---
		m_glviewer = new gui::widgets::GLViewer(init, shutdown, render);


		m_mainWindow->setCentralWidget( m_glviewer );


		// its important to show gui at the very last, as this triggers glview init
		// which in turn triggers demo loading...
		m_mainWindow->show();
	}

	Application::~Application()
	{
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
