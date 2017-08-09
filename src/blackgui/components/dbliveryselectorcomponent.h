/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_CDBLIVERYSELECTORCOMPONENT_H
#define BLACKGUI_COMPONENTS_CDBLIVERYSELECTORCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackgui/dropbase.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/network/entityflags.h"

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

namespace Ui { class CDbLiverySelectorComponent; }

namespace BlackGui
{
    namespace Components
    {
        /*!
         * Selector for liveries
         */
        class BLACKGUI_EXPORT CDbLiverySelectorComponent :
            public QFrame,
            public BlackGui::CDropBase
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CDbLiverySelectorComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CDbLiverySelectorComponent();

            //! Current livery
            void setLivery(const BlackMisc::Aviation::CLivery &livery);

            //! Current livery
            void setlivery(const QString &code);

            //! Livery
            BlackMisc::Aviation::CLivery getLivery() const;

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
            void changedLivery(const BlackMisc::Aviation::CLivery &livery);

        protected:
            //! \copydoc QWidget::dragEnterEvent
            virtual void dragEnterEvent(QDragEnterEvent *event) override;

            //! \copydoc QWidget::dragMoveEvent
            virtual void dragMoveEvent(QDragMoveEvent *event) override;

            //! \copydoc QWidget::dragLeaveEvent
            virtual void dragLeaveEvent(QDragLeaveEvent *event) override;

            //! \copydoc QWidget::dropEvent
            virtual void dropEvent(QDropEvent *event) override;

        private slots:
            //! Data have been changed
            void ps_dataChanged();

            //! Completer activated
            void ps_completerActivated(const QString &liveryCode);

            //! Distributors have been read
            void ps_liveriesRead(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState readState, int count);

        private:
            //! Strip extra info from livery code
            QString stripExtraInfo(const QString &liveryCode) const;

            QScopedPointer<Ui::CDbLiverySelectorComponent> ui;
            QScopedPointer<QCompleter>   m_completerLiveries;
            BlackMisc::Aviation::CLivery m_currentLivery;
        };
    } // ns
} // ns

#endif // guard
