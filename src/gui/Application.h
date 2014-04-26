#pragma once

#include <gltools/gl.h>
#include <QtGui>
#include <QApplication>
#include <QMainWindow>
#include "widgets/GLViewer/GLViewer.h"



namespace gui
{
	class Application : public QApplication
	{
		Q_OBJECT
	public:
		Application( int argc, char **argv, widgets::GLViewer::InitCallback init = 0, widgets::GLViewer::ShutdownCallback shutdown = 0, widgets::GLViewer::RenderCallback render = 0 );
		virtual                         ~Application();

		static Application*             getInstance();
		QMainWindow*                    getMainWindow();
		gui::widgets::GLViewer*         getGlViewer();



	public slots:


	private:
		QMainWindow*                    m_mainWindow;
		gui::widgets::GLViewer*         m_glviewer;

	};

} // namespace gui
