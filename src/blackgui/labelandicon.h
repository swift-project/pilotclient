/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#ifndef BLACKGUI_LABELANDICON_H
#define BLACKGUI_LABELANDICON_H

#include "blackgui/blackguiexport.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>
#include <QString>

class QPixmap;
class QWidget;

namespace Ui
{
    class CLabelAndIcon;
}

namespace BlackGui
{
    /*!
     * Label and icon frame
     */
    class BLACKGUI_EXPORT CLabelAndIcon : public QFrame
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
