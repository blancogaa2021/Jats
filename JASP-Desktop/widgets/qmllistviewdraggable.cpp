//
// Copyright (C) 2013-2018 University of Amsterdam
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public
// License along with this program.  If not, see
// <http://www.gnu.org/licenses/>.
//

#include "qmllistviewdraggable.h"
#include "listmodeldraggable.h"
#include "listmodelassignedinterface.h"
#include "../analysis/analysisform.h"
#include "../analysis/jaspcontrolbase.h"
#include <QTimer>
#include <QQmlProperty>
#include "log.h"

QMLListViewDraggable::QMLListViewDraggable(JASPControlBase* item)
	: QMLListView(item)
{
}

void QMLListViewDraggable::setUp()
{
	QMLListView::setUp();
	
	_draggableModel = dynamic_cast<ListModelDraggable*>(model());
	_draggableModel->setItemType(getItemProperty("itemType").toString());
	_draggableModel->setTermsAreVariables(getItemProperty("showVariableTypeIcon").toBool());
	QString dropMode = getItemProperty("dropMode").toString();
	if (dropMode.isEmpty()) dropMode = "None";
	_draggableModel->setDropMode(qmlDropModeFromQString(dropMode));
	
	QQuickItem::connect(_item, SIGNAL(itemDoubleClicked(int)),							this, SLOT(itemDoubleClickedHandler(int)));
	QQuickItem::connect(_item, SIGNAL(itemsDropped(QVariant, QVariant, int, QString)),	this, SLOT(itemsDroppedHandler(QVariant, QVariant, int, QString)));
}

void QMLListViewDraggable::itemDoubleClickedHandler(int index)
{
	ListModel *targetModel = form()->getRelatedModel(this);
	
	if (!targetModel)
	{
		addError(tr("No related list found for VariablesList %1").arg(name()));
		return;
	}
	
	ListModelDraggable *draggableTargetModel = dynamic_cast<ListModelDraggable*>(targetModel);
	if (!draggableTargetModel)
	{
		addError(tr("Wrong kind of related list (%1) found for VariablesList %2").arg(targetModel->name()).arg(name()));
		return;
	}
	
	QList<int> indexes;
	indexes.push_back(index);
	moveItems(indexes, draggableTargetModel);
}

void QMLListViewDraggable::itemsDroppedHandler(QVariant vindexes, QVariant vdropList, int dropItemIndex, QString assignOption)
{
	QQuickItem* dropList = qobject_cast<QQuickItem*>(vdropList.value<QObject*>());
	ListModelDraggable* dropModel = nullptr;
	
	if (!dropList)
		dropModel = dynamic_cast<ListModelDraggable*>(form()->getRelatedModel(this));
	else
	{
		QVariant vdropModel = QQmlProperty(dropList, "model").read();
		dropModel = qobject_cast<ListModelDraggable*>(vdropModel.value<QObject*>());
	}
	
	if (!dropModel)
	{
		Log::log()  << "No drop element!" << std::endl;
		return;
	}
	
	QList<QVariant> vvindexes = vindexes.toList();
	if (!vvindexes.empty())
	{
		_tempIndexes.clear();
		for (QVariant &index : vvindexes)
			_tempIndexes.push_back(index.toInt());
	}
	else
		_tempIndexes = vindexes.value<QList<int> >();
	
	_tempDropModel = dropModel;
	_tempDropItemIndex = dropItemIndex;
	_tempAssignOption = assignOption;
	// the call to itemsDropped is called from an item that will be removed (the items of the variable list
	// will be re-created). So itemsDropped should not call _moveItems directly.
	QTimer::singleShot(0, this, SLOT(moveItemsDelayedHandler()));
}

void QMLListViewDraggable::moveItemsDelayedHandler()
{
	moveItems(_tempIndexes, _tempDropModel, _tempDropItemIndex, _tempAssignOption);
}

void QMLListViewDraggable::moveItems(QList<int> &indexes, ListModelDraggable* targetModel, int dropItemIndex, const QString& assignOption)
{
	if (targetModel && indexes.size() > 0)
	{
		std::sort(indexes.begin(), indexes.end());
		Options* options = form()->getAnalysisOptions();
		if (options != nullptr)
			options->blockSignals(true);
		
		ListModelDraggable* sourceModel = draggableModel();
		if (sourceModel == targetModel)
			sourceModel->moveTerms(indexes, dropItemIndex);
		else
		{
			bool refreshSource = false;
			Terms termsAdded;
			Terms removedTermsWhenAdding;
			QList<int> indexAdded = indexes;

			if (!sourceModel->copyTermsWhenDropped())
			{
				Terms terms = sourceModel->termsFromIndexes(indexes);
				if (terms.size() == 0)
				{
					Log::log() << "No terms found when trying to moving them" << std::endl;
					return;
				}
				termsAdded = targetModel->canAddTerms(terms);

				if (termsAdded.size() > 0)
					removedTermsWhenAdding = targetModel->addTerms(termsAdded, dropItemIndex, assignOption);

				if (termsAdded.size() != terms.size())
				{
					indexAdded.clear();
					for (int i = 0; i < indexes.size(); i++)
					{
						int index = indexes[i];
						if (i < int(terms.size()))
						{
							const Term& term = terms[size_t(i)];
							if (termsAdded.contains(term))
								indexAdded.append(index);
						}
					}
					refreshSource = true;
				}
			}
				
			if (!targetModel->copyTermsWhenDropped())
			{
				if (indexAdded.size() > 0)
				{
					sourceModel->removeTerms(indexAdded);
					refreshSource = false;
				}
				if (removedTermsWhenAdding.size() > 0)
				{
					sourceModel->addTerms(removedTermsWhenAdding);
					refreshSource = false;
				}
			}

			if (refreshSource)
				sourceModel->refresh();
		}
		
		if (options != nullptr)
			options->blockSignals(false);	
	}
	else
	{
		Log::log()  << (!targetModel ? "no dropModel" : "no indexes") << std::endl;
	}
}
