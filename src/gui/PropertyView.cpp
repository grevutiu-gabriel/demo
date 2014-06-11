#include "PropertyView.h"






namespace gui
{

PropertyView::PropertyView() : QObject()
{
	m_variantManager = new QtVariantPropertyManager();
	m_variantFactory = new QtVariantEditorFactory();
	m_propertyBrowser = new QtTreePropertyBrowser();

	m_propertyBrowser->setFactoryForManager(m_variantManager, m_variantFactory);
	m_propertyBrowser->setPropertiesWithoutValueMarked(true);
	m_propertyBrowser->setRootIsDecorated(false);

	m_propertyBrowser->show();

	connect( m_variantManager, SIGNAL(propertyChanged(QtProperty*)), this, SLOT(onQtPropertyChanged(QtProperty*)) );
}

PropertyView::~PropertyView()
{
	m_propertyBrowser->clear();
	delete m_variantFactory;
	delete m_variantManager;
	delete m_propertyBrowser;
}

PropertyView::Ptr PropertyView::create()
{
	return std::make_shared<PropertyView>();
}

QtProperty* PropertyView::createQtProperty( Property::Ptr prop )
{
	QtProperty* qprop = 0;
	PropertyWrapper wrapper;
	wrapper.property = prop;


	std::string type = prop->getType();
	//std::cout << "PropertyView::createQtProperty: " << type << std::endl;
	if(type == "float")
	{
		PropertyT<float>::Ptr propt = std::dynamic_pointer_cast<PropertyT<float>>(prop);
		qprop = m_variantManager->addProperty(QVariant::Double, QString::fromStdString(prop->getName()));
		wrapper.updateQtProperty = [=]{m_variantManager->setValue(qprop, propt->get());};
		wrapper.updateProperty = [=]{propt->set(m_variantManager->value(qprop).toFloat());};
	}else
	if(type == "int")
	{
		PropertyT<int>::Ptr propt = std::dynamic_pointer_cast<PropertyT<int>>(prop);
		qprop = m_variantManager->addProperty(QVariant::Int, QString::fromStdString(prop->getName()));
		wrapper.updateQtProperty = [=]{m_variantManager->setValue(qprop, propt->get());};
		wrapper.updateProperty = [=]{propt->set(m_variantManager->value(qprop).toInt());};
	}else
	if(type == "string")
	{
		PropertyT<std::string>::Ptr propt = std::dynamic_pointer_cast<PropertyT<std::string>>(prop);
		qprop = m_variantManager->addProperty(QVariant::String, QString::fromStdString(prop->getName()));
		wrapper.updateQtProperty = [=]{m_variantManager->setValue(qprop, QString::fromStdString(propt->get()));};
		wrapper.updateProperty = [=]{propt->set(m_variantManager->value(qprop).toString().toStdString());};
	}else
	if(type == "bool")
	{
		PropertyT<bool>::Ptr propt = std::dynamic_pointer_cast<PropertyT<bool>>(prop);
		qprop = m_variantManager->addProperty(QVariant::Bool, QString::fromStdString(prop->getName()));
		wrapper.updateQtProperty = [=]{m_variantManager->setValue(qprop, propt->get());};
		wrapper.updateProperty = [=]{propt->set(m_variantManager->value(qprop).toBool());};
	}else
	{
		// unknown data type
		QtVariantProperty* qvprop = m_variantManager->addProperty(QVariant::String, QString::fromStdString(prop->getName()));
		qprop = qvprop;
		m_variantManager->setValue(qprop, QString::fromStdString(type));
		qvprop->setAttribute(QLatin1String("readOnly"), true);
		wrapper.updateQtProperty = []{};
		wrapper.updateProperty = []{};
	}

	if(qprop)
	{
		m_map[qprop] = wrapper;
		m_mapInv[wrapper.property] = qprop;
	}
	// identify the type of the given prop
	return qprop;
}

QWidget *PropertyView::getWidget()
{
	return m_propertyBrowser;
}

void PropertyView::onQtPropertyChanged(QtProperty *prop)
{
	//std::cout << "property changed\n";
	auto it = m_map.find(prop);
	if( it != m_map.end() )
	{
		//std::cout << "propery changed: " << it->second.property->getName() << std::endl;
		it->second.updateProperty();
		emit propertyChanged();
	}
}

void PropertyView::addObject(ObjectWrapper::Ptr objectWrapper)
{
	// retrieve qtProperties from objectwrapper
	// we dont care what they actually wrap
	std::vector<Property::Ptr> props;
	objectWrapper->getProperties(props);

	// create root property and add given props as childs
	QtProperty *topItem = m_variantManager->addProperty(QtVariantPropertyManager::groupTypeId(), QString::fromStdString(objectWrapper->getName()));

	for( auto prop:props )
	{
		QtProperty* qprop = createQtProperty( prop );
		if(qprop)
			topItem->addSubProperty(qprop);
	}

	m_propertyBrowser->addProperty(topItem);
}

void PropertyView::clear()
{
	m_propertyBrowser->clear();
	m_map.clear();
	m_mapInv.clear();
}

void PropertyView::update()
{
	for(auto it:m_map)
	{
		PropertyWrapper &wrapper = it.second;
		wrapper.updateQtProperty();
	}

}

void PropertyView::setPropertyEnabled(Property::Ptr prop, bool enabled)
{
	auto it = m_mapInv.find(prop);
	if(it!=m_mapInv.end())
	{
		QtProperty* qprop = it->second;
		qprop->setEnabled(enabled);
	}
}








}
