/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/models/columns.h"
#include "blackgui/guiapplication.h"
#include "blackmisc/mixin/mixincompare.h"

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

        const CDefaultFormatter *CColumn::getFormatter() const
        {
            const bool incogntio = this->isIncognito() && sGui && sGui->isIncognito();
            return incogntio ?
                   CColumn::incongitoFormatter() :
                   m_formatter.data();
        }

        CColumn::CColumn(const QString &toolTip, const CPropertyIndex &propertyIndex) :
            m_columnToolTip(toolTip), m_formatter(new CPixmapFormatter()), m_propertyIndex(propertyIndex)
        {}

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

        CColumn CColumn::emptyColumn()
        {
            CColumn col = CColumn("", "", CPropertyIndexRef::GlobalIndexEmpty, new CEmptyFormatter());
            col.setWidthPercentage(1);
            return col;
        }

        const CIncognitoFormatter *CColumn::incongitoFormatter()
        {
            static const CIncognitoFormatter incognito;
            return &incognito;
        }

        // --------------- columns ----------------------------------------------

        CColumns::CColumns(const QString &translationContext, QObject *parent) :
            QObject(parent), m_translationContext(translationContext)
        {
            // void
        }

        void CColumns::addColumn(const CColumn &column)
        {
            Q_ASSERT(!m_translationContext.isEmpty());
            CColumn copy(column);
            copy.setTranslationContext(m_translationContext);
            m_columns.push_back(copy);
        }

        void CColumns::addColumnIncognito(const CColumn &column)
        {
            CColumn copy(column);
            copy.setIncognito(true);
            this->addColumn(copy);
        }

        QString CColumns::propertyIndexToColumnName(const CPropertyIndex &propertyIndex, bool i18n) const
        {
            const int column = this->propertyIndexToColumn(propertyIndex);
            Q_UNUSED(i18n) // not implemented
            return m_columns.at(column).getColumnName();
        }

        QString CColumns::columnToName(int column, bool i18n) const
        {
            Q_ASSERT(isValidColumn(column));
            Q_UNUSED(i18n) // not implemented
            return m_columns.at(column).getColumnName();
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
                if (m_columns.at(i).getColumnName() == name) { return i; }
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
            if (!isValidColumn(column)) { return false; }
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

        bool CColumns::hasAnyWidthPercentage() const
        {
            for (const CColumn &c : m_columns) { if (c.hasWidthPercentage()) { return true; }}
            return false;
        }

        void CColumns::setWidthPercentages(const QList<int> &percentages)
        {
            if (percentages.isEmpty())
            {
                for (CColumn &column : m_columns) { column.setWidthPercentage(-1); }
                return;
            }

            int c = 0;
            for (CColumn &column : m_columns)
            {
                column.setWidthPercentage(percentages.at(c++));
            }
        }

        QList<int> CColumns::calculateWidths(int totalWidth) const
        {
            if (m_columns.isEmpty() || !this->hasAnyWidthPercentage()) { return {}; }

            int totalPercentage = 0;
            const int averagePercentage = 100 / m_columns.size();

            for (const CColumn &c : m_columns)
            {
                totalPercentage += c.hasWidthPercentage() ? c.getWidthPercentage() : averagePercentage;
            }

            if (totalPercentage < 1) { return {}; }

            // ideally totalPercentage would be 100%, but there is no guarantee
            const double part = static_cast<double>(totalWidth) / totalPercentage;
            QList<int> widths;

            int usedWidth = 0;
            for (const CColumn &c : m_columns)
            {
                const int percentage = c.hasWidthPercentage() ? c.getWidthPercentage() : averagePercentage;
                const int restWidth = totalWidth - usedWidth;
                const int width = qMin(restWidth, qRound(part * percentage));
                widths.push_back(width);
                usedWidth += width;
            }

            return widths;
        }

        void CColumns::insertEmptyColumn()
        {
            if (this->endsWithEmptyColumn()) { return; }
            this->addColumn(CColumn::emptyColumn());
        }

        bool CColumns::endsWithEmptyColumn() const
        {
            if (m_columns.isEmpty()) { return false; }
            const CColumn c = m_columns.last();
            return c.getPropertyIndex() == CPropertyIndexRef::GlobalIndexEmpty;
        }

        const CDefaultFormatter *CColumns::getFormatter(const QModelIndex &index) const
        {
            if (!this->isValidColumn(index)) { return nullptr; }
            const CColumn c = m_columns.at(index.column());
            return c.getFormatter();
        }
    } // namespace
} // namespace
