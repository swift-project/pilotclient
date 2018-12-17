/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_VIEWBASE_ITEMDELEGATE_H
#define BLACKGUI_VIEWBASE_ITEMDELEGATE_H

#include <QStyledItemDelegate>

namespace BlackGui
{
    namespace Views
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
            virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
            virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
            //! @}

        private:
            //! Related CViewBaseNonTemplate
            CViewBaseNonTemplate *viewBase() const;
        };
    } // namespace
} // namespace
#endif // guard
