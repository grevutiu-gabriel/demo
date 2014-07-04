#include "CompositionEditor.h"


#include <QSplitter>

#include "Application.h"







namespace gui
{

CompositionEditor::CompositionEditor(CompositionWrapper::Ptr compositionWrapper):
	QObject(),
	m_compositionWrapper(compositionWrapper)
{
	m_updateGraphView = UpdateGraphView::create(compositionWrapper->getUpdateGraph());
	m_propertyView = PropertyView::create();


	QSplitter* splitter2 = new QSplitter();
	splitter2->addWidget(m_propertyView->getWidget());


	QSplitter* splitter1 = new QSplitter(Qt::Vertical);
	splitter1->addWidget(m_updateGraphView->m_view);
	//splitter1->setStretchFactor(0, 1);
	splitter1->addWidget(splitter2);
	//splitter1->setStretchFactor(1, 1);

	QList<int> sizes;
	sizes.push_back(500);
	sizes.push_back(200);
	splitter1->setSizes(sizes);

	m_widget = splitter1;

	connect( m_updateGraphView.get(), SIGNAL(selectionChanged()), this, SLOT(onGraphSelectionChanged()) );
	//connect( m_updateGraphView.get(), SIGNAL(objectCreated(ObjectWrapper::Ptr)), this, SLOT(onObjectCreatedInGraph(ObjectWrapper::Ptr)) );
	connect( m_propertyView.get(), SIGNAL(propertyChanged()), this, SLOT(onPropertyChanged()) );
}

CompositionEditor::~CompositionEditor()
{
	//delete m_widget;
}

CompositionEditor::Ptr CompositionEditor::create(CompositionWrapper::Ptr compositionWrapper)
{
	return std::make_shared<CompositionEditor>(compositionWrapper);
}

QWidget *CompositionEditor::getWidget()
{
	return m_widget;
}

void CompositionEditor::updateGuiInfo()
{
	m_updateGraphView->updateGuiInfo();
}

void CompositionEditor::onGraphSelectionChanged()
{
	std::vector<ObjectWrapper::Ptr> selected;
	m_updateGraphView->getSelectedObjects(selected);
	if( !selected.empty() )
	{
		m_propertyView->clear();
		m_propertyView->addObject( selected[0] );
	}
}

void CompositionEditor::onPropertyChanged()
{
	Application::getInstance()->getGlViewer()->update();
}

void CompositionEditor::onObjectCreatedInGraph(ObjectWrapper::Ptr objectWrapper)
{
//	// if an object has been created from within the updategraph, we check
//	// if this is an element. If it is, we will automatically add it to the composition
//	if( ObjectFactory::derivesFrom(objectWrapper->getObject()->getMetaObject(), "Element" ) )
//	{
//		ElementWrapper::Ptr elementWrapper = std::dynamic_pointer_cast<ElementWrapper>(objectWrapper);
//		if( elementWrapper )
//			m_compositionWrapper->addElement( elementWrapper );
//	}
}

}
