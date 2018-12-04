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

#include "blackgui/blackguiexport.h"
#include "blackgui/models/columnformatters.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/variant.h"

#include <QList>
#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QtGlobal>

class QModelIndex;

namespace BlackGui
{
    namespace Models
    {
        //! Single column
        class BLACKGUI_EXPORT CColumn
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
            bool hasAlignment() const { return (!m_formatter.isNull() && m_formatter->hasAlignment()); }

            //! Editable?
            bool isEditable() const { return m_editable; }

            //! Set editable
            void setEditable(bool editable) { m_editable = editable; }

            //! Sortable?
            bool isSortable() const { return m_sortable; }

            //! Set sortable
            void setSortable(bool sortable) { m_sortable = sortable; }

            //! Property index used when sorting, option alternative
            BlackMisc::CPropertyIndex getSortPropertyIndex() const { return m_sortPropertyIndex; }

            //! Sort index available
            bool hasSortPropertyIndex() const;

            //! Property index used when sorting, option alternative
            void setSortPropertyIndex(const BlackMisc::CPropertyIndex &propertyIndex);

            //! Formatter
            void setFormatter(CDefaultFormatter *formatter) { Q_ASSERT(formatter); m_formatter.reset(formatter); }

            //! Formatter
            const CDefaultFormatter *getFormatter() const { return m_formatter.data(); }

            //! Aligment as CVariant
            BlackMisc::CVariant getAlignment() const;

            //! Column name
            QString getColumnName(bool i18n = false) const;

            //! Column tooltip
            QString getColumnToolTip(bool i18n = false) const;

            //! Property index
            const BlackMisc::CPropertyIndex &getPropertyIndex() const { return m_propertyIndex;}

            //! Translation context
            void setTranslationContext(const QString &translationContext) { m_translationContext = translationContext; }

            //! Get a standard value object formatted column
            static CColumn standardValueObject(const QString &headerName, const BlackMisc::CPropertyIndex &propertyIndex, int alignment = CDefaultFormatter::alignDefault());

            //! Get a standard value object formatted column
            static CColumn standardValueObject(const QString &headerName, const QString &toolTip, const BlackMisc::CPropertyIndex &propertyIndex, int alignment = CDefaultFormatter::alignDefault());

            //! Get a standard string object formatted column
            static CColumn standardString(const QString &headerName, const BlackMisc::CPropertyIndex &propertyIndex, int alignment = CDefaultFormatter::alignDefault());

            //! Get a standard string object formatted column
            static CColumn standardString(const QString &headerName, const QString &toolTip, const BlackMisc::CPropertyIndex &propertyIndex, int alignment = CDefaultFormatter::alignDefault());

            //! Get a standard string object formatted column
            static CColumn orderColumn(const BlackMisc::CPropertyIndex &propertyIndex = BlackMisc::CPropertyIndex::GlobalIndexIOrderable, int alignment = CDefaultFormatter::alignRightVCenter());

            //! Get a standard integer value formatted column
            static CColumn standardInteger(const QString &headerName, const QString &toolTip, const BlackMisc::CPropertyIndex &propertyIndex, int alignment = CDefaultFormatter::alignRightVCenter());

        private:
            QString m_translationContext;
            QString m_columnName;
            QString m_columnToolTip;
            QSharedPointer<CDefaultFormatter> m_formatter; //!< Used formatter
            BlackMisc::CPropertyIndex m_propertyIndex;     //!< Property index for column
            BlackMisc::CPropertyIndex m_sortPropertyIndex; //!< Property index used when sorted (optional alternative)

            bool m_editable = false;
            bool m_sortable = true;
            const char *getTranslationContextChar() const;
            const char *getColumnNameChar() const;
            const char *getColumnToolTipChar() const;
        };

        /*!
         * Header data for a table
         */
        class BLACKGUI_EXPORT CColumns : public QObject
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

            //! Column to property index for sort, considers \sa CColumn::getSo
            BlackMisc::CPropertyIndex columnToSortPropertyIndex(int column) const;

            //! Property index to column
            int propertyIndexToColumn(const BlackMisc::CPropertyIndex &propertyIndex) const;

            //! Column index to name
            int nameToPropertyIndex(const QString &name) const;

            //! Size (number of columns)
            int size() const;

            //! Alignment for this column?
            bool hasAlignment(const QModelIndex &index) const;

            //! Is this column editable?
            bool isEditable(const QModelIndex &index) const;

            //! Is this column editable?
            bool isEditable(int column) const;

            //! Sortable column?
            bool isSortable(const QModelIndex &index) const;

            //! Sortable column?
            bool isSortable(int column) const;

            //! Valid column?
            bool isValidColumn(const QModelIndex &index) const;

            //! Valid column?
            bool isValidColumn(int column) const;

            //! Aligment as CVariant
            BlackMisc::CVariant getAlignment(const QModelIndex &index) const;

            //! Translation context
            const QString &getTranslationContext() const { return m_translationContext; }

            //! Formatter
            const CDefaultFormatter *getFormatter(const QModelIndex &index) const;

            //! Column at position
            const CColumn &at(int columnNumber) const { return m_columns.at(columnNumber); }

            //! Clear
            void clear() { m_columns.clear(); }

            //! Set columns @{
            void setColumns(const QList<CColumn> &columns) { m_columns = columns; }
            void setColumns(const CColumns &columns) { m_columns = columns.m_columns; }
            //! @}

            //! Columns
            const QList<CColumn> &columns() const { return m_columns; }

        private:
            QList<CColumn> m_columns;
            QString m_translationContext;
            const char *getTranslationContextChar() const;
        };
    }
} // namespace BlackGui

#endif // guard
