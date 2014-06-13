#include "ShotEditor.h"


#include <QSplitter>

#include "Application.h"







namespace gui
{

ShotEditor::ShotEditor(ShotWrapper::Ptr shotWrapper):
	QObject(),
	m_shotWrapper(shotWrapper)
{
	m_updateGraphView = UpdateGraphView::create(shotWrapper->getUpdateGraph());
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
	connect( m_propertyView.get(), SIGNAL(propertyChanged()), this, SLOT(onPropertyChanged()) );
}

ShotEditor::~ShotEditor()
{
	//delete m_widget;
}

ShotEditor::Ptr ShotEditor::create(ShotWrapper::Ptr shotWrapper)
{
	return std::make_shared<ShotEditor>(shotWrapper);
}

QWidget *ShotEditor::getWidget()
{
	return m_widget;
}

void ShotEditor::onGraphSelectionChanged()
{
	std::vector<ObjectWrapper::Ptr> selected;
	m_updateGraphView->getSelectedObjects(selected);
	if( !selected.empty() )
	{
		m_propertyView->clear();
		m_propertyView->addObject( selected[0] );
	}
}

void ShotEditor::onPropertyChanged()
{
	Application::getInstance()->getGlViewer()->update();
}

}
