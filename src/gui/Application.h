#pragma once

#include <gltools/gl.h>
#include <QtGui>
#include <QApplication>
#include <QMainWindow>
#include <QFileSystemWatcher>
#include "widgets/GLViewer/GLViewer.h"

#include "wrapper/DemoWrapper.h"



namespace gui
{
	class Application : public QApplication
	{
		Q_OBJECT
	public:
		typedef std::function<void()> FileChangedCallback;


		Application( int argc, char **argv, widgets::GLViewer::InitCallback init = 0, widgets::GLViewer::ShutdownCallback shutdown = 0, widgets::GLViewer::RenderCallback render = 0 );
		virtual                         ~Application();

		static Application*             getInstance();
		QMainWindow*                    getMainWindow();
		gui::widgets::GLViewer*         getGlViewer();

		void                            setDemo( Demo::Ptr demo );

		void watchFile( const std::string& filename, FileChangedCallback callback );


	public slots:
		void fileChanged( const QString& path );


	private:
		QMainWindow*                    m_mainWindow;
		gui::widgets::GLViewer*         m_glviewer;
		DemoWrapper::Ptr                m_demoWrapper;
		QFileSystemWatcher              m_fileWatcher;
		std::map<std::string, FileChangedCallback> m_fileChangedCallbacks;

	};

} // namespace gui
