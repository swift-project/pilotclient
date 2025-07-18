// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_TICKLABEL_H
#define SWIFT_GUI_TICKLABEL_H

#include <QLabel>
#include <QObject>
#include <QPixmap>
#include <QString>

#include "gui/swiftguiexport.h"
#include "misc/icons.h"

namespace swift::gui
{
    //! Representing a ticked or crossed icon
    class SWIFT_GUI_EXPORT CTickLabel : public QLabel
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CTickLabel(QWidget *parent = nullptr);

        //! Set ticked
        void setTicked(bool ticked);

        //! Set the tool tips
        void setToolTips(const QString &ticked, const QString &unticked);

    signals:
        //! Changed ticked state
        void tickChanged(bool ticked);

    private:
        bool m_isPixmapTicked = false; //!< ticked state
        QString m_toolTipTicked = "on";
        QString m_toolTipUnticked = "off";
        QPixmap m_pixmapTicked = swift::misc::CIcons::tick16();
        QPixmap m_pixmapUnticked = swift::misc::CIcons::cross16();

        //! Set the label
        void setLabel();
    };
} // namespace swift::gui

#endif // SWIFT_GUI_TICKLABEL_H
