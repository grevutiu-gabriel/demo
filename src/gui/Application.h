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
#include "wrapper/ShotWrapper.h"
#include "TreeView.h"
#include "ObjectTypeListView.h"
#include "UpdateGraphView.h"
#include "ShotEditor.h"


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
		void                            openShotEditor( ShotWrapper::Ptr shotWrapper );

		void watchFile( const std::string& filename, FileChangedCallback callback );

		void serializeGuiInfo( Serializer& out );
		void deserializeGuiInfo( Deserializer& in );

	public slots:
		void fileChanged( const QString& path );


	private:
		QMainWindow*                                         m_mainWindow;
		gui::widgets::GLViewer*                              m_glviewer;
		QSplitter*                                           m_splitter;
		TreeView::Ptr                                        m_treeView;
		ObjectTypeListView::Ptr                              m_typeList;
		QTabWidget*                                          m_tabWidget;
		std::map<ShotWrapper::Ptr, ShotEditor::Ptr>          m_shotEditor;
		DemoWrapper::Ptr                                     m_demoWrapper;
		QFileSystemWatcher                                   m_fileWatcher;
		std::map<std::string, FileChangedCallback>           m_fileChangedCallbacks;
		std::map<Object::Ptr, ObjectWrapper::Ptr>            m_objectWrapper;
		std::map<UpdateGraph::Ptr, UpdateGraphWrapper::Ptr>  m_updateGraphWrapper;
	};

} // namespace gui
