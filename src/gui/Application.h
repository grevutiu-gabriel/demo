#pragma once

#include <gltools/gl.h>
#include <QtGui>
#include <QApplication>
#include <QMainWindow>
#include <QFileSystemWatcher>
#include <QTabWidget>
#include <QSplitter>
#include "widgets/GLViewer/GLViewer.h"

#include "wrapper/DemoWrapper.h"
#include "wrapper/UpdateGraphWrapper.h"
#include "TreeView.h"
#include "UpdateGraphView.h"


namespace gui
{
	class Application : public QApplication
	{
		Q_OBJECT
	public:
		typedef std::function<void()> FileChangedCallback;


		Application( int argc, char **argv, widgets::GLViewer::InitCallback init = 0, widgets::GLViewer::ShutdownCallback shutdown = 0, widgets::GLViewer::RenderCallback render = 0 );
		virtual                         ~Application();

		ObjectWrapper::Ptr              getWrapper( Object::Ptr object );
		UpdateGraphWrapper::Ptr         getWrapper( UpdateGraph::Ptr graph );
		static Application*             getInstance();
		QMainWindow*                    getMainWindow();
		gui::widgets::GLViewer*         getGlViewer();

		DemoWrapper::Ptr                getDemoWrapper();
		void                            openGraphEditor( UpdateGraphWrapper::Ptr graph );

		void watchFile( const std::string& filename, FileChangedCallback callback );


	public slots:
		void fileChanged( const QString& path );


	private:
		QMainWindow*                    m_mainWindow;
		gui::widgets::GLViewer*         m_glviewer;
		QSplitter*                      m_splitter;
		TreeView::Ptr                   m_treeView;
		QTabWidget*                     m_tabWidget;
		std::vector<UpdateGraphView*>   m_updateGraphViews;
		DemoWrapper::Ptr                m_demoWrapper;
		QFileSystemWatcher              m_fileWatcher;
		std::map<std::string, FileChangedCallback> m_fileChangedCallbacks;
		std::map<Object::Ptr, ObjectWrapper::Ptr>  m_objectWrapper;
		std::map<UpdateGraph::Ptr, UpdateGraphWrapper::Ptr>  m_updateGraphWrapper;

	};

} // namespace gui
