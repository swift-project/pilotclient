// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_VIEWS_ITEMDELEGATE_H
#define SWIFT_GUI_VIEWS_ITEMDELEGATE_H

#include <QStyledItemDelegate>

namespace swift::gui::views
{
    class CViewBaseNonTemplate;

    /*!
     * Delegate for our view items
     */
    class CViewBaseItemDelegate : public QStyledItemDelegate
    {
        Q_OBJECT

    public:
        //! Constructor
        CViewBaseItemDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

        //! \name QStyledItemDelegate overrides
        //! @{

        //! \copydoc QStyledItemDelegate::paint
        virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

        //! \copydoc QStyledItemDelegate::sizeHint
        virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
        //! @}

    private:
        //! Related CViewBaseNonTemplate
        CViewBaseNonTemplate *viewBase() const;
    };
} // namespace

#endif // guard
