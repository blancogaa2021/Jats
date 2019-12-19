#include "jaspcontrolbase.h"
#include "log.h"
#include "analysisform.h"
#include <QQmlProperty>
#include <QQmlContext>

JASPControlBase::JASPControlBase(QQuickItem *parent) : QQuickItem(parent)
{
	setFlag(ItemIsFocusScope);
	setActiveFocusOnTab(true);
}

void JASPControlBase::setFocusOnTab(bool focus)
{
	if (focus != activeFocusOnTab())
	{
		setActiveFocusOnTab(focus);
		emit focusOnTabChanged();
	}
}

void JASPControlBase::setHasError(bool hasError)
{
	if (section())
		QMetaObject::invokeMethod(section(), "addControlWithError", Qt::DirectConnection, Q_ARG(QVariant, name()), Q_ARG(QVariant, hasError));

	if (form())
		form()->addControlErrorSet(this, hasError);

	if (hasError != _hasError)
	{
		_hasError = hasError;
		emit hasErrorChanged();
	}
}

void JASPControlBase::setHasWarning(bool hasWarning)
{
	if (section())
		QMetaObject::invokeMethod(section(), "addControlWithError", Qt::DirectConnection, Q_ARG(QVariant, name()), Q_ARG(QVariant, hasWarning));

	if (form())
		form()->addControlWarningSet(this, hasWarning);

	if (hasWarning != _hasWarning)
	{
		_hasWarning = hasWarning;
		emit hasWarningChanged();
	}
}

void JASPControlBase::componentComplete()
{
	QQuickItem::componentComplete();

	QObject* p = this;
	do
	{
		p = p->parent();
		_form = qobject_cast<AnalysisForm*>(p);
	}
	while (p && !_form);

	_wrapper = JASPControlWrapper::buildJASPControlWrapper(this);

	QQmlContext* context = qmlContext(this);
	bool hasContextForm = context->contextProperty("hasContextForm").toBool();
	bool noDirectSetup = context->contextProperty("noDirectSetup").toBool();
	if (hasContextForm)
		_form = context->contextProperty("form").value<AnalysisForm*>();

	if (!hasContextForm && _form)
		_form->addControl(this);
	else if (!noDirectSetup)
		_wrapper->setUp();
}

void JASPControlBase::addControlError(QString message)
{
	if (_form)
		_form->addControlError(this, message, false);
}

void JASPControlBase::addControlErrorTemporary(QString message)
{
	if (_form)
		_form->addControlError(this, message, true);
}

void JASPControlBase::addControlWarning(QString message)
{
	if (_form)
		_form->addControlError(this, message, false, true);
}

void JASPControlBase::addControlWarningTemporary(QString message)
{
	if (_form)
		_form->addControlError(this, message, true, true);
}

void JASPControlBase::clearControlError()
{
	if (_form)
		_form->clearControlError(this);
}

QQmlListProperty<QQmlComponent> JASPControlBase::rowComponents()
{
	return QQmlListProperty<QQmlComponent>(this, this,
			 &JASPControlBase::appendRowComponent,
			 &JASPControlBase::rowComponentsCount,
			 &JASPControlBase::rowComponent,
			 &JASPControlBase::clearRowComponents);
}

void JASPControlBase::appendRowComponent(QQmlComponent* p) {
	_rowComponents.append(p);
}

int JASPControlBase::rowComponentsCount() const
{
	return _rowComponents.count();
}

QQmlComponent* JASPControlBase::rowComponent(int index) const
{
	return _rowComponents.at(index);
}

void JASPControlBase::clearRowComponents() {
	_rowComponents.clear();
}

void JASPControlBase::appendRowComponent(QQmlListProperty<QQmlComponent>* list, QQmlComponent* p) {
	reinterpret_cast< JASPControlBase* >(list->data)->appendRowComponent(p);
}

void JASPControlBase::clearRowComponents(QQmlListProperty<QQmlComponent>* list) {
	reinterpret_cast< JASPControlBase* >(list->data)->clearRowComponents();
}

QQmlComponent* JASPControlBase::rowComponent(QQmlListProperty<QQmlComponent>* list, int i) {
	return reinterpret_cast< JASPControlBase* >(list->data)->rowComponent(i);
}

int JASPControlBase::rowComponentsCount(QQmlListProperty<QQmlComponent>* list) {
	return reinterpret_cast< JASPControlBase* >(list->data)->rowComponentsCount();
}
