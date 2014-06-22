#include "Application.h"


#include <QSplitter>
#include <QTreeView>
#include <QMenuBar>
#include <QFrame>


#include <iostream>

#include "wrapper/ControllerWrapper.h"
#include "wrapper/CompositionElementWrapper.h"

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
		if( std::dynamic_pointer_cast<Composition>(object) )
			wrapper = CompositionWrapper::create(std::dynamic_pointer_cast<Composition>(object));
		else
		if( std::dynamic_pointer_cast<CompositionElement>(object) )
			wrapper = CompositionElementWrapper::create(std::dynamic_pointer_cast<CompositionElement>(object));
		else
		if( std::dynamic_pointer_cast<LoadGeometry>(object) )
			wrapper = LoadGeometryWrapper::create(std::dynamic_pointer_cast<LoadGeometry>(object));
		else
		if( std::dynamic_pointer_cast<LoadShader>(object) )
			wrapper = LoadShaderWrapper::create(std::dynamic_pointer_cast<LoadShader>(object));
		else
		if( std::dynamic_pointer_cast<LoadTexture2d>(object) )
			wrapper = LoadTexture2dWrapper::create(std::dynamic_pointer_cast<LoadTexture2d>(object));
		else
		if( std::dynamic_pointer_cast<LoadVolume>(object) )
			wrapper = LoadVolumeWrapper::create(std::dynamic_pointer_cast<LoadVolume>(object));
		else
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



	void Application::openCompositionEditor(CompositionWrapper::Ptr compositionWrapper)
	{
		CompositionEditor::Ptr se;
		auto it = m_compositionEditor.find(compositionWrapper);
		if(it!=m_compositionEditor.end())
		{
			se = it->second;
		}else
		{
			// create composition editor ---
			se = CompositionEditor::create(compositionWrapper);
			m_tabWidget->addTab( se->getWidget(), QString::fromStdString(compositionWrapper->getName()) );
			// register
			m_compositionEditor[compositionWrapper] = se;
		}

		// bring tab to front, change demo rendering composition
		bool found = false;
		int compositionIndex = 0;
		std::vector<Composition::Ptr>& compositions = m_demoWrapper->getDemo()->getCompositions();
		for( auto composition : compositions)
		{
			if( compositionWrapper->getComposition() == composition )
			{
				found=true;
				continue;
			}
			++compositionIndex;
		}

		if(found)
			m_demoWrapper->getDemo()->m_currentCompositionIndex = compositionIndex;
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

	void Application::unwatchFile(const std::string &filename)
	{
		m_fileWatcher.removePath( QString::fromStdString(filename) );
	}

	void Application::serializeGuiInfo(Serializer &out)
	{
		std::vector<Composition::Ptr>& compositions = m_demoWrapper->getDemo()->getCompositions();
		// composition editors
		{
			// make all updategraphviews(in open compositioneditors) update the node positions in
			for( auto compositionEditor:m_compositionEditor )
			{
				compositionEditor.second->updateGuiInfo();
			}


			// serialize updategraphwrappers
			houdini::json::ArrayPtr updateGraphWrappers = houdini::json::Array::create();
			for( auto it:m_updateGraphWrapper )
			{
				houdini::json::ObjectPtr json = houdini::json::Object::create();
				UpdateGraphWrapper::Ptr wrapper = it.second;

				// find composition to which this wrapper belongs...
				Composition::Ptr foundComposition;
				for( auto composition:compositions )
					if( getWrapper(composition->getUpdateGraph()) == wrapper )
						foundComposition = composition;
				if(!foundComposition)
					continue;

				json->append( "composition", out.serialize(foundComposition) );
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

			Composition::Ptr composition = std::dynamic_pointer_cast<Composition>(in.deserializeObject(json->getValue("composition")));

			UpdateGraphWrapper::Ptr updateGraphWrapper = getWrapper(composition->getUpdateGraph());

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
