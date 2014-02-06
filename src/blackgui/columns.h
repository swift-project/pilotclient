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
#include <QObject>
#include <QHash>

namespace BlackGui
{

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

        /*!
         * \brief Add a column name
         * \param propertyIndex
         * \param name
         * \param alignment
         */
        void addColumn(int propertyIndex, const QString &name, int alignment = -1);

        /*!
         * \brief Property index to name
         * \param propertyIndex
         */
        QString propertyIndexToName(int propertyIndex) const;

        /*!
         * \brief Column index to name
         * \param column
         */
        QString columnToName(int column) const;

        //! \brief Column to property index
        int columnToPropertyIndex(int column) const;

        //! \brief Property index to column
        int propertyIndexToColumn(int propertyIndex) const;

        //! \brief Column index to property index
        int indexToPropertyIndex(int index) const;

        //! \brief Column index to name
        int nameToPropertyIndex(const QString &name) const;

        //! \brief Size (number of columns)
        int size() const;

        /*!
         * \brief Alignment for this column?
         * \param index
         * \return
         */
        bool hasAlignment(const QModelIndex &index) const;

        /*!
         * \brief Aligment as QVariant
         * \param index
         * \return
         */
        QVariant aligmentAsQVariant(const QModelIndex &index) const;

        /*!
         * \brief Translation context
         * \return
         */
        const char *getTranslationContext() const;

    private:
        QString m_translationContext;
        QList<QString> m_headerNames;
        QList<int> m_propertyIndexes; // column to property index
        QList<int> m_alignments;
    };

} // namespace BlackGui

#endif // guard
