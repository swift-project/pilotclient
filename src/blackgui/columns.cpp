/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "columns.h"
#include <QModelIndex>

namespace BlackGui
{

/*
 * Header
 */
CColumns::CColumns(const QString &translationContext, QObject *parent) :
    QObject(parent), m_translationContext(translationContext)
{
    // void
}

/*
 * Add column name
 */
void CColumns::addColumn(int propertyIndex, const QString &name, int alignment)
{
    this->m_headerNames.append(name);
    this->m_propertyIndexes.append(propertyIndex);
    this->m_alignments.append(alignment);
}

/*
 * Property index to name
 */
QString CColumns::propertyIndexToName(int propertyIndex) const
{
    int column = this->propertyIndexToColumn(propertyIndex);
    return this->m_headerNames.at(column);
}

/*
 * Index to name
 */
QString CColumns::columnToName(int column) const
{
    Q_ASSERT(column >= 0 && column < this->m_headerNames.size());
    return this->m_headerNames.at(column);
}

/*
 * Get property index
 */
int CColumns::columnToPropertyIndex(int column) const
{
    Q_ASSERT(column >= 0 && column < this->m_propertyIndexes.size());
    return this->m_propertyIndexes.at(column);
}

/*
 * Property index to column
 */
int CColumns::propertyIndexToColumn(int propertyIndex) const
{
    return this->m_propertyIndexes.indexOf(propertyIndex);
}

/*
 * Name to property index
 */
int CColumns::nameToPropertyIndex(const QString &name) const
{
    int column = this->m_headerNames.indexOf(name);
    if (column < 0) return -1;
    return this->m_propertyIndexes.at(column);
}

/*
 * Size
 */
int CColumns::size() const
{
    return this->m_headerNames.size();
}

/*
 * Alignmet
 */
bool CColumns::hasAlignment(const QModelIndex &index) const
{
    if (index.column() < 0 || index.column() >= this->m_alignments.size()) return false;
    return this->m_alignments.at(index.column()) >= 0;
}

/*
 * Aligmnet as QVariant
 */
QVariant CColumns::aligmentAsQVariant(const QModelIndex &index) const
{
    if (index.column() < 0 || index.column() >= this->m_alignments.size()) return QVariant();
    if (!this->hasAlignment(index)) return QVariant(Qt::AlignVCenter | Qt::AlignLeft); // default
    return QVariant(this->m_alignments.at(index.column()));
}

/*
 * Context
 */
const char *CColumns::getTranslationContext() const
{
    return this->m_translationContext.toUtf8().constData();
}

} // namespace BlackGui
