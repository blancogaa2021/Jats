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

#include "listmodelinteractionassigned.h"
#include "utilities/qutils.h"
#include "analysis/options/optionterm.h"
#include "analysis/options/optionboolean.h"
#include "listmodeltermsavailable.h"
#include "listmodeltermsassigned.h"

using namespace std;

ListModelInteractionAssigned::ListModelInteractionAssigned(QMLListView* listView)
	: ListModelAssignedInterface(listView)
{
	_copyTermsWhenDropped = true;
}

void ListModelInteractionAssigned::initTermsWithTemplate(const std::vector<Options *> &terms, Options* rowTemplate)
{
	beginResetModel();
	
	_rows = terms;
	_rowTemplate = rowTemplate;

	_covariates.clear();
	_fixedFactors.clear();
	_randomFactors.clear();
	_interactionTerms.clear();
	_interactionTerms.removeParent();

	for (Options *row : _rows)
	{
		OptionTerm *nameOption = static_cast<OptionTerm*>(row->get(0));
		vector<string> term = nameOption->term();

		_interactionTerms.add(Term(term));
	}
	
	endResetModel();
}



QVariant ListModelInteractionAssigned::data(const QModelIndex &index, int role) const
{
	if (index.isValid() == false)
		return QVariant();

	if (role == Qt::DisplayRole || role == ListModel::NameRole)
	{
		int colNo = index.column();
		int rowNo = index.row();
		Options *row = _rows.at(rowNo);

		if (colNo == 0) {

			OptionTerms *termOption = static_cast<OptionTerms *>(row->get(0));
			Term t(termOption->value().front());
			return t.asQString();
		}
	}

	return ListModelAssignedInterface::data(index, role);
}

int ListModelInteractionAssigned::rowCount(const QModelIndex &) const
{
	return _rows.size();
}


void ListModelInteractionAssigned::setAvailableModel(ListModelAvailableInterface *source)
{
	ListModelAssignedInterface::setAvailableModel(source);
	_interactionTerms.setSortParent(source->allTerms());
}

void ListModelInteractionAssigned::removeTerms(const QList<int> &indices)
{
	// sort indices, and delete from end to beginning

	QList<int> sorted = indices;
	std::sort(sorted.begin(), sorted.end(), qGreater<int>());

	int lastRowDeleted = -1;

	Terms terms = _interactionTerms;
	Terms toRemove;

	for (const int &index : sorted)
	{
		if (index != lastRowDeleted)
		{
			toRemove.add(terms.at(index));
			terms.remove(index);
		}

		lastRowDeleted = index;
	}

	for (const Term &rem : toRemove)
	{
		size_t i = 0;

		while (i < terms.size())
		{
			const Term &term = terms.at(i);

			if (term.containsAll(rem))
				terms.remove(i);
			else
				i++;
		}
	}

	setTerms(terms);
}

Terms *ListModelInteractionAssigned::termsFromIndexes(const QList<int> &indexes) const
{
	Terms* terms = new Terms;
	for (const int &index : indexes)
	{
		Options *row = _rows.at(index);
		if (row->size() == 1)
		{
			OptionTerms *termOption = static_cast<OptionTerms *>(row->get(0));
			const vector<string>& val =  termOption->value().front();
			if (val.size() == 1)
			{
				Term term(val);		
				terms->add(term);
			}
		}
	}
	
	return terms;
}

const Terms &ListModelInteractionAssigned::terms() const
{
	return _interactionTerms;
}

const vector<Options *>& ListModelInteractionAssigned::rows() const
{
	return _rows;
}

void ListModelInteractionAssigned::availableTermsChanged(Terms *termsAdded, Terms *termsRemoved)
{
	if (termsAdded && termsAdded->size() > 0)
	{
		Terms fixedFactors;
		Terms randomFactors;
		Terms covariates;
		for (const Term& term : *termsAdded)
		{
			QString itemType = getItemType(term);
			if (itemType == "fixedFactors")
				fixedFactors.add(term);
			else if (itemType == "randomFactors")
				randomFactors.add(term);
			else if (itemType == "covariates")
				covariates.add(term);
		}
		
		if (fixedFactors.size() > 0)
			addFixedFactors(fixedFactors);
		
		if (randomFactors.size() > 0)
			addRandomFactors(randomFactors);
		
		if (covariates.size() > 0)
			addCovariates(covariates);
	}
	
	if (termsRemoved && termsRemoved->size() > 0)
		removeVariables(*termsRemoved);
}

void ListModelInteractionAssigned::addFixedFactors(const Terms &terms)
{
	_fixedFactors.add(terms);

	Terms existingTerms = _interactionTerms;

	Terms newTerms = _interactionTerms;
	newTerms.discardWhatDoesContainTheseComponents(_randomFactors);
	newTerms.discardWhatDoesContainTheseComponents(_covariates);
	existingTerms.add(newTerms.ffCombinations(terms));

	setTerms(existingTerms);

}

void ListModelInteractionAssigned::addRandomFactors(const Terms &terms)
{
	_randomFactors.add(terms);

	Terms newTerms = _interactionTerms;
	newTerms.add(terms);

	setTerms(newTerms, true);

}

void ListModelInteractionAssigned::addCovariates(const Terms &terms)
{
	_covariates.add(terms);

	Terms newTerms = _interactionTerms;
	newTerms.add(terms);

	setTerms(newTerms);

}

void ListModelInteractionAssigned::removeVariables(const Terms &terms)
{
	_fixedFactors.remove(terms);
	_randomFactors.remove(terms);
	_covariates.remove(terms);

	Terms newTerms = _interactionTerms;
	newTerms.discardWhatDoesContainTheseComponents(terms);

	setTerms(newTerms);
	
}

QString ListModelInteractionAssigned::getItemType(const Term &term)
{
	QString type;
	ListModelTermsAvailable* _source = dynamic_cast<ListModelTermsAvailable*>(source());	
	ListModel* model = _source->getSourceModelOfTerm(term);
	if (model)
	{
		type = model->getItemType();
		if (type.isEmpty() || type == "variables")
			type = model->name();
	}
	
	return type;
}

bool ListModelInteractionAssigned::canAddTerms(Terms *terms) const
{
	Q_UNUSED(terms);

	return true;
}

Terms* ListModelInteractionAssigned::_addTerms(Terms *terms, int assignType)
{
	Terms dropped;
	dropped.setSortParent(source()->allTerms());
	dropped.set(*terms);

	Terms newTerms;

	switch (assignType)
	{
	case Cross:
		newTerms = dropped.crossCombinations();
		break;
	case Interaction:
		newTerms = dropped.wayCombinations(dropped.size());
		break;
	case MainEffects:
		newTerms = dropped.wayCombinations(1);
		break;
	case All2Way:
		newTerms = dropped.wayCombinations(2);
		break;
	case All3Way:
		newTerms = dropped.wayCombinations(3);
		break;
	case All4Way:
		newTerms = dropped.wayCombinations(4);
		break;
	case All5Way:
		newTerms = dropped.wayCombinations(5);
		break;
	default:
		(void)newTerms;
	}

	Terms allTerms = _interactionTerms;
	allTerms.add(newTerms);
	setTerms(allTerms);
	
	return nullptr;
}

Terms* ListModelInteractionAssigned::addTerms(Terms *terms, int dropItemIndex)
{
	Q_UNUSED(dropItemIndex);
	return _addTerms(terms, Cross);
}

OptionTerm *ListModelInteractionAssigned::termOptionFromRow(Options *row)
{
	return static_cast<OptionTerm *>(row->get(0));
}

void ListModelInteractionAssigned::setTerms(const Terms &terms, bool newTermsAreNuisance)
{	
	beginResetModel();
	_interactionTerms.set(terms);

	_rows.erase(
		std::remove_if(
			_rows.begin(),
			_rows.end(),
			[&](Options *row)
			{
				OptionTerm *termCell = termOptionFromRow(row);
				Term existingTerm = Term(termCell->term());
		
				bool shouldRemove = true;
		
				for (const Term &term : terms)
				{
					if (term == existingTerm)
					{
						shouldRemove = false;
						break;
					}		
				}
		
				if (shouldRemove)
					delete row;
					
				return shouldRemove;
			}
		),
		_rows.end()
	);

	Terms::const_iterator itr = terms.begin();

	for (size_t i = 0; i < terms.size(); i++)
	{
		const Term &term = *itr;

		if (i < _rows.size())
		{
			vector<Options *>::iterator otr = _rows.begin();
			otr += i;
			Options *row = *otr;
			OptionTerm *termCell = termOptionFromRow(row);
			Term existingTerm = Term(termCell->term());

			if (existingTerm != term)
			{
				Options *row = static_cast<Options *>(_rowTemplate->clone());
				OptionTerms *termCell = static_cast<OptionTerms *>(row->get(0));
				termCell->setValue(term.scomponents());

				if (row->size() > 1 && newTermsAreNuisance)
				{
					OptionBoolean *nuisance = static_cast<OptionBoolean *>(row->get(1));
					nuisance->setValue(true);
				}

				_rows.insert(otr, row);
			}
		}
		else
		{
			Options *row = static_cast<Options *>(_rowTemplate->clone());
			OptionTerms *termCell = static_cast<OptionTerms *>(row->get(0));
			termCell->setValue(term.scomponents());

			if (row->size() > 1 && newTermsAreNuisance)
			{
				OptionBoolean *nuisance = static_cast<OptionBoolean *>(row->get(1));
				nuisance->setValue(true);
			}

			_rows.push_back(row);
		}

		itr++;
	}

	updateNuisances();

	endResetModel();
	
	emit modelChanged();
}

void ListModelInteractionAssigned::updateNuisances(bool checked)
{
	if (_rows.size() > 0)
	{
		Options *row = _rows.front();
		if (row->size() < 2)
			return; // no nuisance terms
	}

	// if a higher order interaction is specified as nuisance, then all lower order terms should be changed to nuisance as well

	for (size_t i = 0; i < _rows.size(); i++)
	{
		Options *row = _rows.at(i);
		OptionTerm *termOption = static_cast<OptionTerm*>(row->get(0));
		OptionBoolean *nuisanceOption = static_cast<OptionBoolean*>(row->get(1));
		Term term = Term(termOption->term());

		if (nuisanceOption->value() == checked)
		{
			for (size_t j = 0; j < _rows.size(); j++)
			{
				if (i == j)
					continue;

				Options *r = _rows.at(j);

				OptionTerm *tOption = static_cast<OptionTerm*>(r->get(0));
				OptionBoolean *nOption = static_cast<OptionBoolean*>(r->get(1));
				Term t = Term(tOption->term());

				if (checked)
				{
					if (term.containsAll(t))
						nOption->setValue(true);
				}
				else
				{
					if (t.containsAll(term))
						nOption->setValue(false);
				}
			}
		}
	}

//	emit dataChanged(this->index(0,1), this->index(_rows.size() - 1, 1));
}

