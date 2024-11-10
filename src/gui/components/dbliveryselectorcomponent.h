// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_CDBLIVERYSELECTORCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_CDBLIVERYSELECTORCOMPONENT_H

#include "gui/swiftguiexport.h"
#include "gui/dropbase.h"
#include "misc/aviation/livery.h"
#include "misc/network/entityflags.h"

#include <QFrame>
#include <QMetaObject>
#include <QObject>
#include <QScopedPointer>
#include <QString>

class QCompleter;
class QDragEnterEvent;
class QDragLeaveEvent;
class QDragMoveEvent;
class QDropEvent;
class QWidget;

namespace Ui
{
    class CDbLiverySelectorComponent;
}

namespace swift::gui::components
{
    /*!
     * Selector for liveries
     */
    class SWIFT_GUI_EXPORT CDbLiverySelectorComponent :
        public QFrame,
        public swift::gui::CDropBase
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CDbLiverySelectorComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CDbLiverySelectorComponent();

        //! Current livery
        void setLivery(const swift::misc::aviation::CLivery &livery);

        //! Current livery
        void setLivery(const QString &code);

        //! Livery
        swift::misc::aviation::CLivery getLivery() const;

        //! Livery combined code
        QString getRawCombinedCode() const;

        //! Show description
        void withLiveryDescription(bool description);

        //! Read only
        void setReadOnly(bool readOnly);

        //! Set with valid Livery
        bool isSet() const;

        //! Clear selection
        void clear();

    signals:
        //! Distributor was changed
        void changedLivery(const swift::misc::aviation::CLivery &livery);

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
        //! Data have been changed
        void onDataChanged();

        //! Completer activated
        void onCompleterActivated(const QString &liveryCode);

        //! Distributors have been read
        void onLiveriesRead(swift::misc::network::CEntityFlags::Entity entity, swift::misc::network::CEntityFlags::ReadState readState, int count, const QUrl &url);

        //! Strip extra info from livery code
        QString stripExtraInfo(const QString &liveryCode) const;

        QScopedPointer<Ui::CDbLiverySelectorComponent> ui;
        QScopedPointer<QCompleter> m_completerLiveries;
        swift::misc::aviation::CLivery m_currentLivery;
    };
} // ns

#endif // guard
