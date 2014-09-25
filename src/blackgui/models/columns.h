/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COLUMNS_H
#define BLACKGUI_COLUMNS_H

#include "blackmisc/valueobject.h" // for qHash overload, include before Qt stuff due GCC issue
#include "blackmisc/collection.h"
#include "blackmisc/propertyindex.h"
#include "columnformatters.h"
#include <QModelIndex>
#include <QObject>
#include <QHash>
#include <QScopedPointer>

namespace BlackGui
{
    namespace Models
    {
        //! Single column
        class CColumn
        {
        public:
            //! Constructor
            CColumn(const QString &headerName, const BlackMisc::CPropertyIndex &propertyIndex, CDefaultFormatter *formatter, bool editable = false) :
                CColumn(headerName, "", propertyIndex, formatter, editable)
            { }

            //! Constructor
            CColumn(const QString &headerName, const QString &toolTip, const BlackMisc::CPropertyIndex &propertyIndex, CDefaultFormatter *formatter, bool editable = false);

            //! Constructor, icon with tool tip
            CColumn(const QString &toolTip, const BlackMisc::CPropertyIndex &propertyIndex);

            //! Constructor used for icons
            CColumn(const BlackMisc::CPropertyIndex &propertyIndex);

            //! Alignment for this column?
            bool hasAlignment() const { return (!this->m_formatter.isNull() && this->m_formatter->hasAlignment()); }

            //! Editable?
            bool isEditable() const { return this->m_editable; }

            //! Formatter
            void setFormatter(CDefaultFormatter *formatter) { Q_ASSERT(formatter); m_formatter.reset(formatter); }

            //! Formatter
            const CDefaultFormatter *getFormatter() const { return this->m_formatter.data(); }

            //! Aligment as QVariant
            QVariant getAlignment() const;

            //! Column name
            QString getColumnName(bool i18n = false) const;

            //! Column tooltip
            QString getColumnToolTip(bool i18n = false) const;

            //! Property index
            const BlackMisc::CPropertyIndex &getPropertyIndex() const { return this->m_propertyIndex;}

            //! Translation context
            void setTranslationContext(const QString &translationContext) { this->m_translationContext = translationContext; }

            //! Get a standard value object formatted column
            static CColumn standardValueObject(const QString &headerName, const BlackMisc::CPropertyIndex &propertyIndex, int alignment = CDefaultFormatter::alignDefault());

            //! Get a standard value object formatted column
            static CColumn standardValueObject(const QString &headerName, const QString &toolTip, const BlackMisc::CPropertyIndex &propertyIndex, int alignment = CDefaultFormatter::alignDefault());

            //! Get a standard string object formatted column
            static CColumn standardString(const QString &headerName, const BlackMisc::CPropertyIndex &propertyIndex, int alignment = CDefaultFormatter::alignDefault());

            //! Get a standard string object formatted column
            static CColumn standardString(const QString &headerName, const QString &toolTip, const BlackMisc::CPropertyIndex &propertyIndex, int alignment = CDefaultFormatter::alignDefault());

        private:
            QString m_translationContext;
            QString m_columnName;
            QString m_columnToolTip;
            QSharedPointer<CDefaultFormatter> m_formatter;
            BlackMisc::CPropertyIndex m_propertyIndex;
            bool m_editable;
            const char *getTranslationContextChar() const;
            const char *getColumnNameChar() const;
            const char *getColumnToolTipChar() const;
        };

        /*!
         * Header data for a table
         */
        class CColumns : public QObject
        {
        public:
            /*!
             * Columns constructors
             * \param translationContext
             * \param parent
             */
            CColumns(const QString &translationContext, QObject *parent = nullptr);

            //! Add a column
            void addColumn(CColumn column);

            //! Property index to name
            QString propertyIndexToColumnName(const BlackMisc::CPropertyIndex &propertyIndex, bool i18n = false) const;

            //! Column index to name
            QString columnToName(int column, bool i18n = false) const;

            //! Column to property index
            BlackMisc::CPropertyIndex columnToPropertyIndex(int column) const;

            //! Property index to column
            int propertyIndexToColumn(const BlackMisc::CPropertyIndex &propertyIndex) const;

            //! Column index to property index
            int indexToPropertyIndex(const BlackMisc::CPropertyIndex &propertyIndex) const;

            //! Column index to name
            int nameToPropertyIndex(const QString &name) const;

            //! Size (number of columns)
            int size() const;

            //! Alignment for this column?
            bool hasAlignment(const QModelIndex &index) const;

            //! Is this column editable?
            bool isEditable(const QModelIndex &index) const;

            //! Valid column?
            bool isValidColumn(const QModelIndex &index) const
            {
                return (index.column() >= 0 && index.column() < this->m_columns.size());
            }

            //! Valid column?
            bool isValidColumn(int column) const
            {
                return column >= 0 && column < this->m_columns.size();
            }

            //! Aligment as QVariant
            QVariant getAlignment(const QModelIndex &index) const;

            //! Translation context
            const QString &getTranslationContext() const { return m_translationContext; }

            //! Formatter
            const CDefaultFormatter *getFormatter(const QModelIndex &index) const;

            //! Column at position
            const CColumn &at(int columnNumber) const { return this->m_columns.at(columnNumber); }

            //! Clear
            void clear() { this->m_columns.clear(); }

        private:
            QList<CColumn> m_columns;
            QString m_translationContext;
            const char *getTranslationContextChar() const;
        };
    }
} // namespace BlackGui

#endif // guard
