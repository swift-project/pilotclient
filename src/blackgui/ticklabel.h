// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_TICKLABEL_H
#define BLACKGUI_TICKLABEL_H

#include "blackgui/blackguiexport.h"
#include "misc/icons.h"

#include <QLabel>
#include <QObject>
#include <QPixmap>
#include <QString>

namespace BlackGui
{
    //! Representing a ticked or crossed icon
    class BLACKGUI_EXPORT CTickLabel : public QLabel
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CTickLabel(QWidget *parent = nullptr);

        //! Set ticked
        void setTicked(bool ticked);

        //! Set the tool tips
        void setToolTips(const QString &ticked, const QString &unticked);

        //! Ticked pixmap
        void setPixmapTicked(const QPixmap &pixmap);

        //! Unticked pixmap
        void setPixmapUnticked(const QPixmap &pixmap);

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
} // namespace

#endif // guard
