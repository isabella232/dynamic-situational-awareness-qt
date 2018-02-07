// Copyright 2017 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

#include "AlertCondition.h"
#include "AlertConditionListModel.h"

AlertConditionListModel::AlertConditionListModel(QObject* parent):
  QAbstractListModel(parent)
{
  m_roles[AlertConditionListRoles::Name] = "name";
  m_roles[AlertConditionListRoles::Level] = "level";
  m_roles[AlertConditionListRoles::Description] = "description";
  m_roles[AlertConditionListRoles::ConditionEnabled] = "conditionEnabled";
}

AlertConditionListModel::~AlertConditionListModel()
{

}

bool AlertConditionListModel::addAlertCondition(AlertCondition* condition)
{
  if (!condition)
    return false;

  const int size = m_conditions.size();
  const int insertIdx = size == 0 ? 0 : size;

  connect(condition, &AlertCondition::conditionChanged, this, [this, condition]()
  {
    auto it = m_conditions.cbegin();
    auto itEnd = m_conditions.cend();
    int currRow = 0;
    for (; it != itEnd; ++it, ++currRow)
    {
      AlertCondition* candidateCondition = *it;
      if (!candidateCondition)
        continue;

      if (candidateCondition == condition)
      {
        const QModelIndex changedIndex = index(currRow, 0);
        emit dataChanged(changedIndex, changedIndex);
      }
    }
  });

  beginInsertRows(QModelIndex(), insertIdx, insertIdx);
  m_conditions.append(condition);
  endInsertRows();

  return true;
}

AlertCondition* AlertConditionListModel::conditionAt(int rowIndex) const
{
  return m_conditions.value(rowIndex, nullptr);
}

void AlertConditionListModel::removeAt(int rowIndex)
{
  AlertCondition* condition = conditionAt(rowIndex);
  if (!condition)
    return;

  beginRemoveRows(QModelIndex(), rowIndex, rowIndex);
  m_conditions.removeAt(rowIndex);
  endRemoveRows();
}

int AlertConditionListModel::rowCount(const QModelIndex&) const
{
  return m_conditions.size();
}

QVariant AlertConditionListModel::data(const QModelIndex& index, int role) const
{
  if (index.row() < 0 || index.row() > rowCount())
    return QVariant();

  AlertCondition* condition = m_conditions.at(index.row());
  if (!condition)
    return QVariant();

  switch (role)
  {
  case AlertConditionListRoles::Name:
    return condition->name();
  case AlertConditionListRoles::Level:
    return static_cast<int>(condition->level());
  case AlertConditionListRoles::Description:
    return condition->description();
  case AlertConditionListRoles::ConditionEnabled:
    return condition->enabled();
  default:
    break;
  }

  return QVariant();
}

/*!
  \brief Sets the data stored under \a role at \a index in the model to \a value.

  The role should make use of the \l AlertConditionListRoles enum.

  Return \c true if the data was successfully set and \c false otherwise.
 */
bool AlertConditionListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if (!index.isValid() || value.isNull())
    return false;

  AlertCondition* condition = conditionAt(index.row());
  if (!condition)
    return false;

  bool valueSet = false;
  switch (role)
  {
  case AlertConditionListRoles::Name:
    break;
  case AlertConditionListRoles::Level:
    break;
  case AlertConditionListRoles::Description:
    break;
  case AlertConditionListRoles::ConditionEnabled:
  {
    const bool newEnabled = value.toBool();
    condition->setEnabled(newEnabled);
    valueSet = true;
  }
  default:
    break;
  }

  if (valueSet)
    dataChanged(index, index);

  return valueSet;
}

QHash<int, QByteArray> AlertConditionListModel::roleNames() const
{
  return m_roles;
}
