// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_DROPSITE_H
#define BLACKGUI_DROPSITE_H

#include "blackgui/blackguiexport.h"
#include "blackgui/dropbase.h"
#include "misc/variant.h"

#include <QLabel>
#include <QList>
#include <QObject>
#include <QString>

class QDragEnterEvent;
class QDragLeaveEvent;
class QDragMoveEvent;
class QDropEvent;
class QPaintEvent;
class QWidget;

namespace BlackGui
{
    /*!
     * Area where items can be dropped
     * \remark inspired by Qt example: http://doc.qt.io/qt-4.8/qt-draganddrop-dropsite-example.html
     */
    class BLACKGUI_EXPORT CDropSite :
        public QLabel,
        public CDropBase
    {
        Q_OBJECT

    public:
        //! Constructor
        CDropSite(QWidget *parent = nullptr);

        //! Set text for drop site
        void setInfoText(const QString &dropSiteText);

        //! Drop allowed
        void allowDrop(bool allowed) override;

    signals:
        //! Dropped value object
        void droppedValueObject(const swift::misc::CVariant &droppedObject);

    protected:
        //! \copydoc QWidget::dragEnterEvent
        virtual void dragEnterEvent(QDragEnterEvent *event) override;

        //! \copydoc QWidget::dragMoveEvent
        virtual void dragMoveEvent(QDragMoveEvent *event) override;

        //! \copydoc QWidget::dragLeaveEvent
        virtual void dragLeaveEvent(QDragLeaveEvent *event) override;

        //! \copydoc QWidget::dropEvent
        virtual void dropEvent(QDropEvent *event) override;

    private:
        //! Style has been changed
        void onStyleSheetsChanged();

        //! Clear
        void resetText();

        QString m_infoText; //!< text displayed for drop site
        bool m_allowDrop = true; //!< dropping allowed?
        QList<int> m_acceptedMetaTypes; //!< accepted meta types
    };
} // ns

#endif // guard
