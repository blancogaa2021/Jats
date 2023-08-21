#include "expanddataproxymodel.h"
#include "datasettablemodel.h"

ExpandDataProxyModel::ExpandDataProxyModel(QObject *parent)
	: QObject{parent}
{
}

int ExpandDataProxyModel::rowCount(bool includeVirtuals) const
{
	if (!_sourceModel)
		return 0;
	return _sourceModel->rowCount() + (includeVirtuals && _expandDataSet ? EXTRA_ROWS : 0);
}

int ExpandDataProxyModel::columnCount(bool includeVirtuals) const
{
	if (!_sourceModel)
		return 0;
	return _sourceModel->columnCount() + (includeVirtuals && _expandDataSet ? EXTRA_COLS : 0);
}

QVariant ExpandDataProxyModel::data(int row, int col, int role) const
{
	if (!_sourceModel || role == -1) // Role not defined
		return QVariant();

	if (col < _sourceModel->columnCount() && row < _sourceModel->rowCount())
		return _sourceModel->data(_sourceModel->index(row, col), role);

	switch(role)
	{
	case int(dataPkgRoles::selected):				return false;
	case int(dataPkgRoles::lines):
	{
		DataSetTableModel * dataSetTable = dynamic_cast<DataSetTableModel *>(_sourceModel);

		if (row < _sourceModel->rowCount() && dataSetTable && dataSetTable->showInactive() && !DataSetPackage::pkg()->getRowFilter(row))
			return DataSetPackage::getDataSetViewLines(false, false, false, false);
		return DataSetPackage::getDataSetViewLines(col>0, row>0, true, true);
	}
	case int(dataPkgRoles::value):					return "";
//	case int(dataPkgRoles::itemInputValue):			return "string"; ???
	default:										return QVariant();
	}

	return QVariant(); //gcc might complain some more I guess?
}

QVariant ExpandDataProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (!_sourceModel || role == -1) // Role not defined
		return QVariant();

	if (orientation == Qt::Orientation::Horizontal)
	{
		if (section < _sourceModel->columnCount())
			return _sourceModel->headerData(section, orientation, role);
		else
			switch(role)
			{
			case int(dataPkgRoles::columnIsComputed):				return false;
			case int(dataPkgRoles::computedColumnIsInvalidated):	return false;
			case int(dataPkgRoles::filter):							return false;
			case int(dataPkgRoles::computedColumnError):			return "";
			case int(dataPkgRoles::columnType):						return int(columnType::unknown);
			case int(dataPkgRoles::maxColString):					return "XXXXXXXXXXX";
			default:												return "";
			}
	}
	else if (orientation == Qt::Orientation::Vertical)
	{
		if (section < _sourceModel->rowCount())
			return _sourceModel->headerData(section, orientation, role);
		else
			return  DataSetPackage::pkg()->dataRowCount() + (section - _sourceModel->rowCount()) + 1;
	}

	return QVariant();
}

Qt::ItemFlags ExpandDataProxyModel::flags(int row, int column) const
{
	if (!_sourceModel)
		return Qt::NoItemFlags;

	if (column < _sourceModel->columnCount() && row < _sourceModel->rowCount())
		return _sourceModel->flags(index(row, column));

	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

QModelIndex ExpandDataProxyModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!_sourceModel)
		return QModelIndex();

	return _sourceModel->index(row, column, parent);
}

bool ExpandDataProxyModel::filtered(int row, int column) const
{
	if (!_sourceModel)
		return false;

	if (column < _sourceModel->columnCount() && row < _sourceModel->rowCount())
	{
		QModelIndex ind(_sourceModel->index(row, column));
		return _sourceModel->data(ind, getRole("filter")).toBool();
	}

	return true;
}

bool ExpandDataProxyModel::isRowVirtual(int row) const
{
	if (!_sourceModel)
		return false;

	return row >= _sourceModel->rowCount();
}

bool ExpandDataProxyModel::isColumnVirtual(int col) const
{
	if (!_sourceModel)
		return false;

	return col >= _sourceModel->columnCount();
}

void ExpandDataProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
	if(_sourceModel != sourceModel)
		_sourceModel = sourceModel;

	_setRolenames();
}

void ExpandDataProxyModel::_setRolenames()
{
	_roleNameToRole.clear();

	if(_sourceModel == nullptr) return;

	auto roleNames = _sourceModel->roleNames();

	for(auto rn : roleNames.keys())
		_roleNameToRole[roleNames[rn].toStdString()] = rn;
}

int ExpandDataProxyModel::getRole(const std::string &roleName) const
{
	auto it = _roleNameToRole.find(roleName);
	if (it == _roleNameToRole.end())
		return -1;
	else
		return it->second;
}

void ExpandDataProxyModel::removeRows(int start, int count)
{
	if (!_sourceModel)
		return;

	_sourceModel->removeRows(start, count);
}

void ExpandDataProxyModel::removeColumns(int start, int count)
{
	if (!_sourceModel)
		return;

	_sourceModel->removeColumns(start, count);
}

void ExpandDataProxyModel::removeRow(int row)
{
	if (!_sourceModel)
		return;

	if (row >= 0 && row < _sourceModel->columnCount())
		_sourceModel->removeRow(row);
}

void ExpandDataProxyModel::removeColumn(int col)
{
	if (!_sourceModel)
		return;

	if (col >= 0 && col < _sourceModel->columnCount())
		_sourceModel->removeColumn(col);
}

void ExpandDataProxyModel::insertRow(int row)
{
	if (!_sourceModel)
		return;

	_sourceModel->insertRow(row);
}

void ExpandDataProxyModel::insertColumn(int col)
{
	if (!_sourceModel)
		return;

	_sourceModel->insertColumn(col);
}

QString ExpandDataProxyModel::insertColumnSpecial(int col, bool computed, bool R)
{
	DataSetTableModel * dataSetTable = dynamic_cast<DataSetTableModel *>(_sourceModel);

	if (dataSetTable)
		return dataSetTable->insertColumnSpecial(col, computed, R);

	return "";
}

void ExpandDataProxyModel::_expandIfNecessary(int row, int col)
{
	if (!_sourceModel || row < 0 || col < 0 || row >= rowCount() || col >= columnCount())
		return;

	for (int colNr = _sourceModel->columnCount(); colNr <= col; colNr++)
		insertColumnSpecial(colNr, false, false);
	for (int rowNr = _sourceModel->rowCount(); rowNr <= row; rowNr++)
		insertRow(rowNr);
}

bool ExpandDataProxyModel::setData(int row, int col, const QVariant &value, int role)
{
	if (!_sourceModel || row < 0 || col < 0)
		return false;

	_expandIfNecessary(row, col);

	return _sourceModel->setData(index(row, col), value, role);
}

void ExpandDataProxyModel::pasteSpreadsheet(int row, int col, const std::vector<std::vector<QString>> & cells, QStringList newColNames)
{
	DataSetTableModel* dataSetTable = qobject_cast<DataSetTableModel*>(_sourceModel);

	if (!dataSetTable || row < 0 || col < 0)
		return;

	_expandIfNecessary(row, col);

	dataSetTable->pasteSpreadsheet(row, col, cells, newColNames);
}
