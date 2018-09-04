/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/models/columns.h"
#include "blackmisc/compare.h"

#include <QByteArray>
#include <QCoreApplication>
#include <QModelIndex>

using namespace BlackMisc;

namespace BlackGui
{
    namespace Models
    {
        CColumn::CColumn(const QString &headerName, const QString &toolTip, const CPropertyIndex &propertyIndex, CDefaultFormatter *formatter, bool editable) :
            m_columnName(headerName), m_columnToolTip(toolTip), m_formatter(formatter ? formatter : new CDefaultFormatter()), m_propertyIndex(propertyIndex),
            m_editable(editable)
        {}

        CColumn::CColumn(const CPropertyIndex &propertyIndex) :
            m_formatter(new CPixmapFormatter()), m_propertyIndex(propertyIndex)
        {}

        bool CColumn::hasSortPropertyIndex() const
        {
            return !m_sortPropertyIndex.isEmpty();
        }

        void CColumn::setSortPropertyIndex(const CPropertyIndex &propertyIndex)
        {
            Q_ASSERT(!propertyIndex.isEmpty());
            m_sortPropertyIndex = propertyIndex;
        }

        CColumn::CColumn(const QString &toolTip, const CPropertyIndex &propertyIndex) :
            m_columnToolTip(toolTip), m_formatter(new CPixmapFormatter()), m_propertyIndex(propertyIndex)
        {}

        const char *CColumn::getTranslationContextChar() const
        {
            return m_translationContext.toUtf8().constData();
        }

        const char *CColumn::getColumnNameChar() const
        {
            return m_columnName.toUtf8().constData();
        }

        const char *CColumn::getColumnToolTipChar() const
        {
            return m_columnToolTip.toUtf8().constData();
        }

        QString CColumn::getColumnName(bool i18n) const
        {
            if (!i18n || m_translationContext.isEmpty()) return m_columnName;
            return QCoreApplication::translate(this->getTranslationContextChar(), this->getColumnNameChar());
        }

        QString CColumn::getColumnToolTip(bool i18n) const
        {
            if (!i18n || m_columnToolTip.isEmpty()) return m_columnToolTip;
            return QCoreApplication::translate(this->getTranslationContextChar(), this->getColumnToolTipChar());
        }

        CColumn CColumn::standardValueObject(const QString &headerName, const CPropertyIndex &propertyIndex, int alignment)
        {
            return CColumn(headerName, propertyIndex, new CValueObjectFormatter(alignment));
        }

        CColumn CColumn::standardValueObject(const QString &headerName, const QString &toolTip, const CPropertyIndex &propertyIndex, int alignment)
        {
            return CColumn(headerName, toolTip, propertyIndex, new CValueObjectFormatter(alignment));
        }

        CColumn CColumn::standardString(const QString &headerName, const CPropertyIndex &propertyIndex, int alignment)
        {
            return CColumn(headerName, propertyIndex, new CStringFormatter(alignment));
        }

        CColumn CColumn::standardString(const QString &headerName, const QString &toolTip, const CPropertyIndex &propertyIndex, int alignment)
        {
            return CColumn(headerName, toolTip, propertyIndex, new CStringFormatter(alignment));
        }

        CColumn CColumn::orderColumn(const CPropertyIndex &propertyIndex, int alignment)
        {
            return CColumn("#", "order", propertyIndex, new CStringFormatter(alignment));
        }

        CColumn CColumn::standardInteger(const QString &headerName, const QString &toolTip, const CPropertyIndex &propertyIndex, int alignment)
        {
            return CColumn(headerName, toolTip, propertyIndex, new CIntegerFormatter(alignment));
        }

        // --------------- columns ----------------------------------------------

        CColumns::CColumns(const QString &translationContext, QObject *parent) :
            QObject(parent), m_translationContext(translationContext)
        {
            // void
        }

        void CColumns::addColumn(CColumn column)
        {
            Q_ASSERT(!m_translationContext.isEmpty());
            column.setTranslationContext(m_translationContext);
            m_columns.push_back(column);
        }

        QString CColumns::propertyIndexToColumnName(const CPropertyIndex &propertyIndex, bool i18n) const
        {
            int column = this->propertyIndexToColumn(propertyIndex);
            return m_columns.at(column).getColumnName(i18n);
        }

        QString CColumns::columnToName(int column, bool i18n) const
        {
            Q_ASSERT(isValidColumn(column));
            return m_columns.at(column).getColumnName(i18n);
        }

        CPropertyIndex CColumns::columnToPropertyIndex(int column) const
        {
            Q_ASSERT(isValidColumn(column));
            return m_columns.at(column).getPropertyIndex();
        }

        CPropertyIndex CColumns::columnToSortPropertyIndex(int column) const
        {
            Q_ASSERT(isValidColumn(column));
            const CColumn col = m_columns[column];
            Q_ASSERT(col.isSortable());
            if (!col.isSortable()) { return CPropertyIndex(); }
            if (col.hasSortPropertyIndex()) { return col.getSortPropertyIndex(); }
            return col.getPropertyIndex();
        }

        int CColumns::propertyIndexToColumn(const CPropertyIndex &propertyIndex) const
        {
            for (int i = 0; i < m_columns.size(); i++)
            {
                if (m_columns.at(i).getPropertyIndex() == propertyIndex)
                {
                    return i;
                }
            }
            return -1;
        }

        int CColumns::nameToPropertyIndex(const QString &name) const
        {
            for (int i = 0; i < m_columns.size(); i++)
            {
                if (m_columns.at(i).getColumnName(false) == name)
                    return i;
            }
            return -1;
        }

        int CColumns::size() const
        {
            return m_columns.size();
        }

        bool CColumns::hasAlignment(const QModelIndex &index) const
        {
            if (!isValidColumn(index)) return false;
            return m_columns.at(index.column()).hasAlignment();
        }

        bool CColumns::isEditable(const QModelIndex &index) const
        {
            if (!isValidColumn(index)) return false;
            return m_columns.at(index.column()).isEditable();
        }

        bool CColumns::isEditable(int column) const
        {
            if (!isValidColumn(column)) return false;
            return m_columns.at(column).isEditable();
        }

        bool CColumns::isSortable(const QModelIndex &index) const
        {
            if (!isValidColumn(index)) return false;
            return m_columns.at(index.column()).isSortable();
        }

        bool CColumns::isSortable(int column) const
        {
            if (!isValidColumn(column)) return false;
            return m_columns.at(column).isSortable();
        }

        bool CColumns::isValidColumn(const QModelIndex &index) const
        {
            return (index.column() >= 0 && index.column() < m_columns.size());
        }

        bool CColumns::isValidColumn(int column) const
        {
            return column >= 0 && column < m_columns.size();
        }

        const CDefaultFormatter *CColumns::getFormatter(const QModelIndex &index) const
        {
            if (!isValidColumn(index)) { return nullptr; }
            return m_columns.at(index.column()).getFormatter();
        }

        const char *CColumns::getTranslationContextChar() const
        {
            return m_translationContext.toUtf8().constData();
        }
    }
} // namespace
