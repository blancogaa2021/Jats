#ifndef TABLEMODELVARIABLESLEVELS_H
#define TABLEMODELVARIABLESLEVELS_H

#include <QAbstractListModel>

#include "common.h"
#include "boundmodel.h"
#include "droptarget.h"

#include "tablemodelvariablesavailable.h"
#include "variableinfo.h"

#include "options/optionstable.h"
#include "options/optionstring.h"
#include "options/optionlist.h"

 #include "utils.h"

class TableModelVariablesLevels : public TableModel, public BoundModel, public DropTarget, public VariableInfoConsumer
{
	Q_OBJECT
public:
	explicit TableModelVariablesLevels(QWidget *parent = 0);

	virtual void bindTo(Option *option) OVERRIDE;
	virtual void mimeDataMoved(const QModelIndexList &indexes) OVERRIDE;

	virtual int rowCount(const QModelIndex &parent) const OVERRIDE;
	virtual int columnCount(const QModelIndex &parent) const OVERRIDE;
	virtual QVariant data(const QModelIndex &index, int role) const OVERRIDE;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const OVERRIDE;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role) OVERRIDE;

	virtual Qt::DropActions supportedDropActions() const OVERRIDE;
	virtual Qt::DropActions supportedDragActions() const OVERRIDE;
	virtual QStringList mimeTypes() const OVERRIDE;
	virtual QMimeData *mimeData(const QModelIndexList &indexes) const OVERRIDE;
	virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) OVERRIDE;
	virtual bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const OVERRIDE;

	void setSource(TableModelVariablesAvailable *source);

	void setVariableTypesSuggested(int variableTypesSuggested);
	int variableTypesSuggested() const;
	void setVariableTypesAllowed(int variableTypesAllowed);
	int variableTypesAllowed() const;

protected:

	bool isSuggested(const Term &term) const;
	bool isAllowed(const Term &term) const;

private:

	class Row
	{
	public:
		Row(QString title, bool isLevelHeading = false)
		{
			_title = title;
			_isHeading = isLevelHeading;
			_option = NULL;
			_isOption = false;
		}

		Row(OptionList *option)
		{
			_option = option;
			_isOption = true;
			_isHeading = false;
		}

		QString title() const
		{
			return _title;
		}

		OptionList *option() const
		{
			return _option;
		}

		bool isHeading() const
		{
			return _isHeading;
		}

		bool isOption() const
		{
			return _isOption;
		}

	private:
		QString _title;
		OptionList *_option;
		bool _isHeading;
		bool _isOption;
	};

	std::vector<Options *> _levels;

	OptionsTable *_boundTo;

	QList<Row> _rows;
	TableModelVariablesAvailable *_source;
	void refresh();

	int _variableTypesSuggested;
	int _variableTypesAllowed;

	QIcon _nominalTextIcon;
	QIcon _nominalIcon;
	QIcon _ordinalIcon;
	QIcon _scaleIcon;

};

#endif // TABLEMODELVARIABLESLEVELS_H
