/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef TICKLABEL_H
#define TICKLABEL_H

#include "blackmisc/icons.h"
#include <QLabel>
#include <QPixmap>

namespace BlackGui
{
    //! Representing a ticked or crossed icon
    class CTickLabel : public QLabel
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
        QPixmap m_pixmapTicked = BlackMisc::CIcons::tick16();
        QPixmap m_pixmapUnticked = BlackMisc::CIcons::cross16();

        //! Set the label according to \sa
        void setLabel();

    };

} // namespace

#endif // guard
