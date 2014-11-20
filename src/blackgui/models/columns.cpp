/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "columns.h"
#include <QCoreApplication>

namespace BlackGui
{
    namespace Models
    {

        CColumn::CColumn(const QString &headerName, const QString &toolTip, const BlackMisc::CPropertyIndex &propertyIndex, CDefaultFormatter *formatter, bool editable) :
            m_columnName(headerName), m_columnToolTip(toolTip), m_formatter(formatter ? formatter : new CDefaultFormatter()), m_propertyIndex(propertyIndex),
            m_editable(editable)
        {}

        CColumn::CColumn(const BlackMisc::CPropertyIndex &propertyIndex) :
            m_formatter(new CPixmapFormatter()), m_propertyIndex(propertyIndex)
        {}

        CColumn::CColumn(const QString &toolTip, const BlackMisc::CPropertyIndex &propertyIndex) :
            m_columnToolTip(toolTip), m_formatter(new CPixmapFormatter()), m_propertyIndex(propertyIndex)
        {}

        const char *CColumn::getTranslationContextChar() const
        {
            return this->m_translationContext.toUtf8().constData();
        }

        const char *CColumn::getColumnNameChar() const
        {
            return this->m_columnName.toUtf8().constData();
        }

        const char *CColumn::getColumnToolTipChar() const
        {
            return this->m_columnToolTip.toUtf8().constData();
        }

        QString CColumn::getColumnName(bool i18n) const
        {
            if (!i18n || this->m_translationContext.isEmpty()) return this->m_columnName;
            return QCoreApplication::translate(this->getTranslationContextChar(), this->getColumnNameChar());
        }

        QString CColumn::getColumnToolTip(bool i18n) const
        {
            if (!i18n || this->m_columnToolTip.isEmpty()) return this->m_columnToolTip;
            return QCoreApplication::translate(this->getTranslationContextChar(), this->getColumnToolTipChar());
        }

        CColumn CColumn::standardValueObject(const QString &headerName, const BlackMisc::CPropertyIndex &propertyIndex, int alignment)
        {
            return CColumn(headerName, propertyIndex, new CValueObjectFormatter(alignment));
        }

        CColumn CColumn::standardValueObject(const QString &headerName, const QString &toolTip, const BlackMisc::CPropertyIndex &propertyIndex, int alignment)
        {
            return CColumn(headerName, toolTip, propertyIndex, new CValueObjectFormatter(alignment));
        }

        CColumn CColumn::standardString(const QString &headerName, const BlackMisc::CPropertyIndex &propertyIndex, int alignment)
        {
            return CColumn(headerName, propertyIndex, new CStringFormatter(alignment));
        }

        CColumn CColumn::standardString(const QString &headerName, const QString &toolTip, const BlackMisc::CPropertyIndex &propertyIndex, int alignment)
        {
            return CColumn(headerName, toolTip, propertyIndex, new CStringFormatter(alignment));
        }

        // --------------- columns ----------------------------------------------

        CColumns::CColumns(const QString &translationContext, QObject *parent) :
            QObject(parent), m_translationContext(translationContext)
        {
            // void
        }

        void CColumns::addColumn(CColumn column)
        {
            Q_ASSERT(!this->m_translationContext.isEmpty());
            column.setTranslationContext(this->m_translationContext);
            this->m_columns.push_back(column);
        }

        QString CColumns::propertyIndexToColumnName(const BlackMisc::CPropertyIndex &propertyIndex, bool i18n) const
        {
            int column = this->propertyIndexToColumn(propertyIndex);
            return this->m_columns.at(column).getColumnName(i18n);
        }

        QString CColumns::columnToName(int column, bool i18n) const
        {
            Q_ASSERT(isValidColumn(column));
            return this->m_columns.at(column).getColumnName(i18n);
        }

        BlackMisc::CPropertyIndex CColumns::columnToPropertyIndex(int column) const
        {
            Q_ASSERT(isValidColumn(column));
            return this->m_columns.at(column).getPropertyIndex();
        }

        int CColumns::propertyIndexToColumn(const BlackMisc::CPropertyIndex &propertyIndex) const
        {
            for (int i = 0; i < this->m_columns.size(); i++)
            {
                if (this->m_columns.at(i).getPropertyIndex() == propertyIndex)
                {
                    return i;
                }
            }
            return -1;
        }

        int CColumns::nameToPropertyIndex(const QString &name) const
        {
            for (int i = 0; i < this->m_columns.size(); i++)
            {
                if (this->m_columns.at(i).getColumnName(false) == name)
                    return i;
            }
            return -1;
        }

        int CColumns::size() const
        {
            return this->m_columns.size();
        }

        bool CColumns::hasAlignment(const QModelIndex &index) const
        {
            if (!isValidColumn(index)) return false;
            return this->m_columns.at(index.column()).hasAlignment();
        }

        bool CColumns::isEditable(const QModelIndex &index) const
        {
            if (!isValidColumn(index)) return false;
            return this->m_columns.at(index.column()).isEditable();
        }

        const CDefaultFormatter *CColumns::getFormatter(const QModelIndex &index) const
        {
            if (!isValidColumn(index)) return nullptr;
            return this->m_columns.at(index.column()).getFormatter();
        }

        const char *CColumns::getTranslationContextChar() const
        {
            return this->m_translationContext.toUtf8().constData();
        }
    }
} // namespace
