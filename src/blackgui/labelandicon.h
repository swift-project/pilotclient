/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKGUI_LABELANDICON_H
#define BLACKGUI_LABELANDICON_H

#include <QFrame>
#include <QScopedPointer>

namespace Ui { class CLabelAndIcon; }

namespace BlackGui
{
    /*!
     * Label and icon frame
     */
    class CLabelAndIcon : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CLabelAndIcon(QWidget *parent = nullptr);

        //! Destructor
        ~CLabelAndIcon();

        //! Set values (left to right)
        void set(const QPixmap &pixmap, const QString &text);

        //! Set values (left to right)
        void set(const QString &text, const QPixmap &pixmap);

    private:
        QScopedPointer<Ui::CLabelAndIcon> ui;
    };

} // ns

#endif // guard
