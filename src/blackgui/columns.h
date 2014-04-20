/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKGUI_COLUMNS_H
#define BLACKGUI_COLUMNS_H

#include "blackmisc/valueobject.h" // for qHash overload, include before Qt stuff due GCC issue
#include "blackmisc/collection.h"
#include <QObject>
#include <QHash>

namespace BlackGui
{
    //! Single column
    class CColumn
    {
    public:
        /*!
         * \brief Constructor
         * \param headerName
         * \param propertyIndex as in CValueObject::propertyByIndex
         * \param alignment Qt::Alignment
         * \param editable
         */
        CColumn(const QString &headerName, int propertyIndex, int alignment = -1, bool editable = false);

        /*!
         * \brief Constructor
         * \param headerName
         * \param propertyIndex
         * \param editable
         */
        CColumn(const QString &headerName, int propertyIndex, bool editable);

        //! Alignment for this column?
        bool hasAlignment() const
        {
            return this->m_alignment >= 0;
        }

        //! Editable?
        bool isEditable() const
        {
            return this->m_editable;
        }

        //! Aligment as QVariant
        QVariant aligmentAsQVariant() const;

        //! Column name
        QString getColumnName(bool i18n = false) const;

        //! Property index
        int getPropertyIndex() const { return this->m_propertyIndex;}

        //! Translation context
        void setTranslationContext(const QString &translationContext)
        {
            this->m_translationContext = translationContext;
        }

    private:
        QString m_translationContext;
        QString m_columnName;
        int m_alignment;
        int m_propertyIndex; // property index
        bool m_editable;
        const char *getTranslationContextChar() const;
        const char *getColumnNameChar() const;
    };

    /*!
     * \brief Header data for a table
     */
    class CColumns : public QObject
    {
    public:
        /*!
         * \brief Columns constructors
         * \param translationContext
         * \param parent
         */
        CColumns(const QString &translationContext, QObject *parent = nullptr);

        //! Add a column
        void addColumn(CColumn column);

        //! Property index to name
        QString propertyIndexToColumnName(int propertyIndex, bool i18n = false) const;

        //! Column index to name
        QString columnToName(int column, bool i18n = false) const;

        //! Column to property index
        int columnToPropertyIndex(int column) const;

        //! Property index to column
        int propertyIndexToColumn(int propertyIndex) const;

        //! Column index to property index
        int indexToPropertyIndex(int index) const;

        //! Column index to name
        int nameToPropertyIndex(const QString &name) const;

        //! Size (number of columns)
        int size() const;

        //! Alignment for this column?
        bool hasAlignment(const QModelIndex &index) const;

        //! Is this column editable?
        bool isEditable(const QModelIndex &index) const;

        //! Aligment as QVariant
        QVariant aligmentAsQVariant(const QModelIndex &index) const;

        //! Column at position
        const CColumn &at(int columnNumber) const { return this->m_columns.at(columnNumber); }


    private:
        QList<CColumn> m_columns;
        QString m_translationContext;
        const char *getTranslationContextChar() const;
    };

} // namespace BlackGui

#endif // guard
