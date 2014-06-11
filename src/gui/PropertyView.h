#pragma once

#include <memory>
#include <functional>

#include <QObject>

#include "widgets/qtpropertybrowser/qtpropertymanager.h"
#include "widgets/qtpropertybrowser/qtvariantproperty.h"
#include "widgets/qtpropertybrowser/qttreepropertybrowser.h"

#include "wrapper/ObjectWrapper.h"

namespace gui
{


class PropertyView : public QObject
{
	Q_OBJECT
public:
	typedef std::shared_ptr<PropertyView> Ptr;

	PropertyView();
	~PropertyView();
	static Ptr create();

	void addObject( ObjectWrapper::Ptr objectWrapper );
	void clear();
	void update(); // updates all properties presentations (called when, for example the frame changes and properties changed)

	void setPropertyEnabled( Property::Ptr prop, bool enabled );
	QtProperty *createQtProperty(Property::Ptr prop);

	QWidget* getWidget();

signals:
	void propertyChanged();
public slots:
	void onQtPropertyChanged( QtProperty* prop );
private:
	struct PropertyWrapper
	{
		typedef std::function<void()> UpdateCallback;
		Property::Ptr property;
		UpdateCallback updateQtProperty; // updates the propertyview from values in properties
		UpdateCallback updateProperty; // updates property values from values in the propertyview
	};

	QtTreePropertyBrowser* m_propertyBrowser;
	QtVariantEditorFactory* m_variantFactory;

	QtVariantPropertyManager* m_variantManager;
	//QtIntPropertyManager*     m_intManager;

	std::map<QtProperty*, PropertyWrapper> m_map;
	std::map<Property::Ptr, QtProperty*> m_mapInv;

};






}
