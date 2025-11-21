// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef SWIFT_GUI_LABELANDICON_H
#define SWIFT_GUI_LABELANDICON_H

#include <QFrame>
#include <QObject>
#include <QScopedPointer>
#include <QString>

#include "gui/swiftguiexport.h"

class QPixmap;
class QWidget;

namespace Ui
{
    class CLabelAndIcon;
}

namespace swift::gui
{
    /*!
     * Label and icon frame
     */
    class SWIFT_GUI_EXPORT CLabelAndIcon : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CLabelAndIcon(QWidget *parent = nullptr);

        //! Destructor
        ~CLabelAndIcon() override;

        //! Set values (left to right)
        void set(const QPixmap &pixmap, const QString &text);

        //! Set values (left to right)
        void set(const QString &text, const QPixmap &pixmap);

    private:
        QScopedPointer<Ui::CLabelAndIcon> ui;
    };

} // namespace swift::gui

#endif // SWIFT_GUI_LABELANDICON_H
