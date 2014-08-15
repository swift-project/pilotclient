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
#include <QObject>
#include <QHash>

namespace BlackGui
{
    namespace Models
    {
        //! Single column
        class CColumn
        {
        public:
            /*!
             * Constructor
             * \param headerName
             * \param propertyIndex as in CValueObject::propertyByIndex
             * \param alignment Qt::Alignment
             * \param editable
             */
            CColumn(const QString &headerName, const BlackMisc::CPropertyIndex &propertyIndex, int alignment = -1, bool editable = false);

            /*!
             * Constructor
             * \param headerName
             * \param propertyIndex as in CValueObject::propertyByIndex
             * \param editable
             */
            CColumn(const QString &headerName, const BlackMisc::CPropertyIndex &propertyIndex, bool editable);

            /*!
             * Constructor column is icon
             * \remarks only make sense with isIcon as true
             * \param propertyIndex as in CValueObject::propertyByIndex
             * \param isIcon icon, should be used with true only
             */
            CColumn(const BlackMisc::CPropertyIndex &propertyIndex, bool isIcon);

            //! Alignment for this column?
            bool hasAlignment() const { return this->m_alignment >= 0; }

            //! Editable?
            bool isEditable() const { return this->m_editable; }

            //! Icon?
            bool isIcon() const { return this->m_icon; }

            //! Aligment as QVariant
            QVariant aligmentAsQVariant() const;

            //! Column name
            QString getColumnName(bool i18n = false) const;

            //! Property index
            const BlackMisc::CPropertyIndex &getPropertyIndex() const { return this->m_propertyIndex;}

            //! Translation context
            void setTranslationContext(const QString &translationContext)
            {
                this->m_translationContext = translationContext;
            }

        private:
            QString m_translationContext;
            QString m_columnName;
            int m_alignment;
            BlackMisc::CPropertyIndex m_propertyIndex;
            bool m_editable;
            bool m_icon;
            const char *getTranslationContextChar() const;
            const char *getColumnNameChar() const;
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

            //! Is icon?
            bool isIcon(const QModelIndex &index) const;

            //! Aligment as QVariant
            QVariant aligmentAsQVariant(const QModelIndex &index) const;

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
