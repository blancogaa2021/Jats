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

#ifndef LISTMODEL_H
#define LISTMODEL_H

#include <QAbstractListModel>
#include <QQmlComponent>

#include "common.h"
#include "analysis/options/variableinfo.h"
#include "analysis/options/terms.h"

class JASPListControl;
class RowControls;
class JASPControl;
class BoundControl;
class Option;

class ListModel : public QAbstractTableModel, public VariableInfoConsumer
{
	Q_OBJECT
	typedef QMap<QString, RowControls*> rowControlMap;

public:
	enum ListModelRoles
	{
        NameRole = Qt::UserRole + 1,
		TypeRole,
		SelectedRole,
		SelectableRole,
		ColumnTypeRole,
		RowComponentRole,
		ValueRole
    };
	typedef QMap<QString, QMap<QString, Option*> > RowControlsOptions;

	ListModel(JASPListControl* listView);
	
			QHash<int, QByteArray>	roleNames()													const override;
			int						rowCount(const QModelIndex &parent = QModelIndex())			const override;
			int						columnCount(const QModelIndex &parent = QModelIndex())		const override { return 1; }
			QVariant				data(const QModelIndex &index, int role = Qt::DisplayRole)	const override;

	virtual void					endResetModel();

			JASPListControl*		listView()													const		{ return _listView; }
			const QString &			name() const;
	virtual const Terms &			terms(const QString& what = QString())						const;
			bool					areTermsVariables()											const		{ return _areTermsVariables; }
			bool					areTermsInteractions()										const		{ return _areTermsInteractions; }
			bool					needsSource()												const		{ return _needsSource;			}
			void					setNeedsSource(bool needs)												{ _needsSource = needs;			}
	virtual QString					getItemType(const Term& term)								const		{ return _itemType; }
	virtual void					setTermsAreVariables(bool areVariables);
	virtual void					setTermsAreInteractions(bool interactions)								{ _areTermsInteractions = interactions; }
			void					setItemType(QString type)												{ _itemType = type; }
			void					addControlError(const QString& error)						const;
	virtual void					refresh();
	virtual void					initTerms(const Terms &terms, const RowControlsOptions& allOptionsMap = RowControlsOptions());
			Terms					getSourceTerms();
			ListModel*				getSourceModelOfTerm(const Term& term);

			void					setRowComponent(QQmlComponent* rowComponents);
	virtual void					setUpRowControls();
	const rowControlMap	&			getRowControls() const { return _rowControlsMap; }
	virtual JASPControl	*			getRowControl(const QString& key, const QString& name)		const;
	virtual bool					addRowControl(const QString& key, JASPControl* control);

	Q_INVOKABLE int					searchTermWith(QString searchString);
	Q_INVOKABLE void				selectItem(int _index, bool _select);
	Q_INVOKABLE void				clearSelectedItems(bool emitSelectedChange = true);
	Q_INVOKABLE void				setSelectedItem(int _index);
	Q_INVOKABLE void				selectAllItems();
	Q_INVOKABLE QList<int>			selectedItems()															{ return _selectedItems; }
	Q_INVOKABLE QList<QString>		selectedItemsTypes()													{ return _selectedItemsTypes.toList(); }
	Q_INVOKABLE QList<QString>		itemTypes();

			void					replaceVariableName(const std::string & oldName, const std::string & newName);


signals:
			void termsChanged();
			void selectedItemsChanged();
			void oneTermChanged(const QString& oldName, const QString& newName);

public slots:	
	virtual void sourceTermsChanged();

private:
			void	_addSelectedItemType(int _index);
			void	_sourceTermsChangedHandler(Option *option = nullptr);
			void	_initTerms(const Terms &terms, const RowControlsOptions& allOptionsMap, bool setupRowConnections = true);
			void	_connectSourceControls(ListModel* sourceModel, const QSet<QString>& controls);

protected:
	JASPListControl*				_listView = nullptr;
	QString							_itemType;
	Terms							_terms;
	QList<int>						_selectedItems;
	QSet<QString>					_selectedItemsTypes;
	bool							_needsSource			= true;
	bool							_areTermsVariables		= true;
	bool							_areTermsInteractions	= false;
	QMap<QString, RowControls* >	_rowControlsMap;
	QQmlComponent *					_rowComponent = nullptr;
	RowControlsOptions				_rowControlsOptions;
	QList<BoundControl *>			_rowControlsConnected;

};

#endif // LISTMODEL_H
