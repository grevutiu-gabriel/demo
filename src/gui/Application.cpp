#include "Application.h"


#include <QSplitter>
#include <QTreeView>
#include <QMenuBar>
#include <QFrame>


#include <iostream>

#include "wrapper/ControllerWrapper.h"

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

		// list view for object types
		m_typeList = ObjectTypeListView::create();

		// tab widget for graph views ---
		m_tabWidget = new QTabWidget();

		QSplitter* leftSplitter = new QSplitter(Qt::Vertical);
		leftSplitter->addWidget(m_treeView->m_widget);
		leftSplitter->addWidget(m_typeList->getWidget());

		QList<int> sizes;
		m_splitter = new QSplitter();
		m_splitter->addWidget(leftSplitter);
		sizes.push_back(250);
		m_splitter->addWidget(m_tabWidget);
		sizes.push_back(1000);

		m_splitter->setStretchFactor(0, 0);
		m_splitter->setStretchFactor(1, 1);
		m_splitter->setSizes(sizes);
		m_splitter->resize( 850, 700 );


		// file watcher ---
		connect( &m_fileWatcher, SIGNAL(fileChanged(QString)), this, SLOT(fileChanged(QString)) );


		// gl viewer ---
		m_glviewer = new gui::widgets::GLViewer(init, shutdown, render);


		m_mainWindow->setCentralWidget( m_glviewer );


		m_splitter->move(50, m_splitter->pos().y());
		m_splitter->show();
		// its important to show gui at the very last, as this triggers glview init
		// which in turn triggers demo loading...
		m_mainWindow->move(m_splitter->width()+50+15, m_mainWindow->pos().y());
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

		if( std::dynamic_pointer_cast<Controller>(object) )
			wrapper = ControllerWrapper::create(std::dynamic_pointer_cast<Controller>(object));
		else
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



	void Application::openShotEditor(ShotWrapper::Ptr shotWrapper)
	{
		ShotEditor::Ptr se;
		auto it = m_shotEditor.find(shotWrapper);
		if(it!=m_shotEditor.end())
		{
			se = it->second;
		}else
		{
			// create shot editor ---
			se = ShotEditor::create(shotWrapper);
			m_tabWidget->addTab( se->getWidget(), QString::fromStdString(shotWrapper->getName()) );
			// register
			m_shotEditor[shotWrapper] = se;
		}

		// bring tab to front, change demo rendering shot
		bool found = false;
		int shotIndex = 0;
		std::vector<Shot::Ptr>& shots = m_demoWrapper->getDemo()->getShots();
		for( auto shot : shots)
		{
			if( shotWrapper->getShot() == shot )
			{
				found=true;
				continue;
			}
			++shotIndex;
		}

		if(found)
			m_demoWrapper->getDemo()->m_currentShotIndex = shotIndex;
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

	void Application::serializeGuiInfo(Serializer &out)
	{
		std::vector<Shot::Ptr>& shots = m_demoWrapper->getDemo()->getShots();
		// shot editors
		{
			// make all updategraphviews(in open shoteditors) update the node positions in
			for( auto shotEditor:m_shotEditor )
			{
				shotEditor.second->updateGuiInfo();
			}


			// serialize updategraphwrappers
			houdini::json::ArrayPtr updateGraphWrappers = houdini::json::Array::create();
			for( auto it:m_updateGraphWrapper )
			{
				houdini::json::ObjectPtr json = houdini::json::Object::create();
				UpdateGraphWrapper::Ptr wrapper = it.second;

				// find shot to which this wrapper belongs...
				Shot::Ptr foundShot;
				for( auto shot:shots )
					if( getWrapper(shot->getUpdateGraph()) == wrapper )
						foundShot = shot;
				if(!foundShot)
					continue;

				json->append( "shot", out.serialize(foundShot) );
				wrapper->serialize(out, json);

				updateGraphWrappers->append(json);
			}
			out.write("updategraphwrappers", updateGraphWrappers);
		}
	}

	void Application::deserializeGuiInfo(Deserializer &in)
	{
		houdini::json::ArrayPtr updateGraphWrappers = in.readArray("updategraphwrappers");
		for( int i=0, numElements=updateGraphWrappers->size();i<numElements;++i )
		{
			houdini::json::ObjectPtr json = updateGraphWrappers->getObject(i);

			Shot::Ptr shot = std::dynamic_pointer_cast<Shot>(in.deserializeObject(json->getValue("shot")));

			UpdateGraphWrapper::Ptr updateGraphWrapper = getWrapper(shot->getUpdateGraph());

			updateGraphWrapper->deserialize( in, json );
		}
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
