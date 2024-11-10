// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_MODELS_COLUMNS_H
#define SWIFT_GUI_MODELS_COLUMNS_H

#include "gui/swiftguiexport.h"
#include "gui/models/columnformatters.h"
#include "misc/propertyindex.h"
#include "misc/variant.h"

#include <QList>
#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QtGlobal>

class QModelIndex;

namespace swift::gui::models
{
    //! Single column
    class SWIFT_GUI_EXPORT CColumn
    {
    public:
        //! Constructor
        CColumn(const QString &headerName, const swift::misc::CPropertyIndex &propertyIndex, CDefaultFormatter *formatter, bool editable = false) : CColumn(headerName, "", propertyIndex, formatter, editable)
        {}

        //! Constructor
        CColumn(const QString &headerName, const QString &toolTip, const swift::misc::CPropertyIndex &propertyIndex, CDefaultFormatter *formatter, bool editable = false);

        //! Constructor, icon with tool tip
        CColumn(const QString &toolTip, const swift::misc::CPropertyIndex &propertyIndex);

        //! Constructor used for icons
        CColumn(const swift::misc::CPropertyIndex &propertyIndex);

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
        swift::misc::CPropertyIndex getSortPropertyIndex() const { return m_sortPropertyIndex; }

        //! Sort index available
        bool hasSortPropertyIndex() const;

        //! Property index used when sorting, option alternative
        void setSortPropertyIndex(const swift::misc::CPropertyIndex &propertyIndex);

        //! Formatter
        void setFormatter(CDefaultFormatter *formatter)
        {
            Q_ASSERT(formatter);
            m_formatter.reset(formatter);
        }

        //! Formatter
        const CDefaultFormatter *getFormatter() const;

        //! Aligment as CVariant
        swift::misc::CVariant getAlignment() const;

        //! Column name
        const QString &getColumnName() const { return m_columnName; }

        //! Column tooltip
        const QString &getColumnToolTip() const { return m_columnToolTip; }

        //! Property index
        const swift::misc::CPropertyIndex &getPropertyIndex() const { return m_propertyIndex; }

        //! Translation context
        void setTranslationContext(const QString &translationContext) { m_translationContext = translationContext; }

        //! Width in percentage
        int getWidthPercentage() const { return m_widthPercentage; }

        //! Having a width percentage
        bool hasWidthPercentage() const { return m_widthPercentage > 0; }

        //! Width percentage
        void setWidthPercentage(int width) { m_widthPercentage = width; }

        //! If incognito mode, do NOT display daza
        bool isIncognito() const { return m_incognito; }

        //! Mark as incognito enabled
        void setIncognito(bool incognito) { m_incognito = incognito; }

        //! Get a standard value object formatted column
        static CColumn standardValueObject(const QString &headerName, const swift::misc::CPropertyIndex &propertyIndex, int alignment = CDefaultFormatter::alignDefault());

        //! Get a standard value object formatted column
        static CColumn standardValueObject(const QString &headerName, const QString &toolTip, const swift::misc::CPropertyIndex &propertyIndex, int alignment = CDefaultFormatter::alignDefault());

        //! Get a standard string object formatted column
        static CColumn standardString(const QString &headerName, const swift::misc::CPropertyIndex &propertyIndex, int alignment = CDefaultFormatter::alignDefault());

        //! Get a standard string object formatted column
        static CColumn standardString(const QString &headerName, const QString &toolTip, const swift::misc::CPropertyIndex &propertyIndex, int alignment = CDefaultFormatter::alignDefault());

        //! Get a standard string object formatted column
        static CColumn orderColumn(const swift::misc::CPropertyIndex &propertyIndex = swift::misc::CPropertyIndexRef::GlobalIndexIOrderable, int alignment = CDefaultFormatter::alignRightVCenter());

        //! Get a standard integer value formatted column
        static CColumn standardInteger(const QString &headerName, const QString &toolTip, const swift::misc::CPropertyIndex &propertyIndex, int alignment = CDefaultFormatter::alignRightVCenter());

        //! An empty column
        static CColumn emptyColumn();

    private:
        QString m_translationContext;
        QString m_columnName;
        QString m_columnToolTip;
        int m_widthPercentage = -1;
        QSharedPointer<CDefaultFormatter> m_formatter; //!< Used formatter
        swift::misc::CPropertyIndex m_propertyIndex; //!< Property index for column
        swift::misc::CPropertyIndex m_sortPropertyIndex; //!< Property index used when sorted (optional alternative)

        //! Incognito formatter
        static const CIncognitoFormatter *incongitoFormatter();

        bool m_editable = false;
        bool m_sortable = true;
        bool m_incognito = false;
    };

    /*!
     * Header data for a table
     */
    class SWIFT_GUI_EXPORT CColumns : public QObject
    {
        Q_OBJECT

    public:
        /*!
         * Columns constructors
         * \param translationContext
         * \param parent
         */
        CColumns(const QString &translationContext, QObject *parent = nullptr);

        //! Add a column
        void addColumn(const CColumn &column);

        //! Add a column as incognito enabled
        void addColumnIncognito(const CColumn &column);

        //! Property index to name
        QString propertyIndexToColumnName(const swift::misc::CPropertyIndex &propertyIndex, bool i18n = false) const;

        //! Column index to name
        QString columnToName(int column, bool i18n = false) const;

        //! Column to property index
        swift::misc::CPropertyIndex columnToPropertyIndex(int column) const;

        //! Column to property index for sort, considers \sa CColumn::getSo
        swift::misc::CPropertyIndex columnToSortPropertyIndex(int column) const;

        //! Property index to column
        int propertyIndexToColumn(const swift::misc::CPropertyIndex &propertyIndex) const;

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
        swift::misc::CVariant getAlignment(const QModelIndex &index) const;

        //! Formatter
        const CDefaultFormatter *getFormatter(const QModelIndex &index) const;

        //! Column at position
        const CColumn &at(int columnNumber) const { return m_columns.at(columnNumber); }

        //! Clear
        void clear() { m_columns.clear(); }

        //! @{
        //! Set columns
        void setColumns(const QList<CColumn> &columns) { m_columns = columns; }
        void setColumns(const CColumns &columns) { m_columns = columns.m_columns; }
        //! @}

        //! Columns
        const QList<CColumn> &columns() const { return m_columns; }

        //! Any column with width percentage?
        bool hasAnyWidthPercentage() const;

        //! Set the width percentages
        void setWidthPercentages(const QList<int> &percentages);

        //! Calculate the absolute width
        QList<int> calculateWidths(int totalWidth) const;

        //! Insert an empty column
        void insertEmptyColumn();

        //! Ending with an empty column
        bool endsWithEmptyColumn() const;

    private:
        QList<CColumn> m_columns; //!< all columns
        QString m_translationContext; //!< for future usage
    };
} // ns

#endif // guard
