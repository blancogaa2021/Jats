#ifndef LABELMODEL_H
#define LABELMODEL_H


#include "datasettableproxy.h"
#include <QTimer>

class LabelModel : public DataSetTableProxy
{
	Q_OBJECT
	Q_ENUMS(Column)

	Q_PROPERTY(int		filteredOut		READ filteredOut									NOTIFY filteredOutChanged		)
	Q_PROPERTY(int		chosenColumn	READ proxyParentColumn	WRITE setProxyParentColumn	NOTIFY proxyParentColumnChanged	)
	Q_PROPERTY(bool		visible			READ visible			WRITE setVisible			NOTIFY visibleChanged			)
	Q_PROPERTY(QString	columnName		READ columnNameQ									NOTIFY columnNameChanged		)
	Q_PROPERTY(float	filterColWidth	READ filterColWidth		WRITE setFilterColWidth		NOTIFY filterColWidthChanged	)
	Q_PROPERTY(float	valueColWidth	READ valueColWidth		WRITE setValueColWidth		NOTIFY valueColWidthChanged		)
	Q_PROPERTY(float	labelColWidth	READ labelColWidth		WRITE setLabelColWidth		NOTIFY labelColWidthChanged		)

public:
	///These are also used directly (aka without this enum) in DataSetPackage::data
	enum class	Column { Filter=0, Value=1, Label=2 };

				LabelModel();

	bool		labelNeedsFilter(size_t col);
	std::string columnName(size_t col);
	QString		columnNameQ()			{ return QString::fromStdString(columnName(proxyParentColumn()));	}
	bool		setData(const QModelIndex & index, const QVariant & value,	int role = -1)						override;
	QVariant	data(	const QModelIndex & index,							int role = Qt::DisplayRole)	const	override;
	QVariant	headerData(int section, Qt::Orientation orientation, int role)							const	override;

	bool		visible()			const {	return _visible; }
	int			filteredOut()		const;
	int			dataColumnCount()	const;

	Q_INVOKABLE void reverse();
	Q_INVOKABLE void moveSelectionUp();
	Q_INVOKABLE void moveSelectionDown();
	Q_INVOKABLE void resetFilterAllows();
	
	Q_INVOKABLE void singleClickForSelect(			int row);
	Q_INVOKABLE void doubleClickSoonAfterSelect(	int row);

	std::vector<bool>			filterAllows(size_t col);
	std::vector<std::string>	labels(size_t col);

	float filterColWidth() const { return getColumnWidth(0); }
	float valueColWidth()  const { return getColumnWidth(1); }
	float labelColWidth()  const { return getColumnWidth(2); }

public slots:
	void filteredOutChangedHandler(int col);
	void setVisible(bool visible);
	void toggleSelected(int row);
	void columnAboutToBeRemoved(int column);
	void columnDataTypeChanged(const QString & colName);

	void setFilterColWidth(float colWidth) { if(setColumnWidth(0, colWidth)) emit filterColWidthChanged(filterColWidth());	}
	void setValueColWidth (float colWidth) { if(setColumnWidth(1, colWidth)) emit valueColWidthChanged(valueColWidth());	}
	void setLabelColWidth (float colWidth) { if(setColumnWidth(2, colWidth)) emit labelColWidthChanged(labelColWidth());	}

	void onChosenColumnChanged();

signals:
	void visibleChanged(bool visible);
	void filteredOutChanged();
	void columnNameChanged();
	void allFiltersReset();
	void labelFilterChanged();
	void filterColWidthChanged(float filterColWidth);
	void valueColWidthChanged( float valueColWidth);
	void labelColWidthChanged( float labelColWidth);

private:
	bool				setColumnWidth(int col, float width);
	float				getColumnWidth(int col)					const { return _colWidths[col]; }
	int					roleFromColumn(Column col)				const;
	std::vector<size_t> getSortedSelection()					const;

private:
	bool				_visible		= false;
	std::vector<float>	_colWidths		= { 60, 120, 400} ;
	std::set<QString>	_selected;
	
	std::map<int, QTimer*>	_toggleSelectTimers;
};
Q_DECLARE_METATYPE(LabelModel::Column)

#endif // LABELMODEL_H
