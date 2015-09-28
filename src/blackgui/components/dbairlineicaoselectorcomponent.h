/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_DBAIRLINEICAOSELECTORCOMPONENT_H
#define BLACKGUI_DBAIRLINEICAOSELECTORCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackcore/webdataservices.h"
#include "blackgui/dropbase.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include <QFrame>
#include <QScopedPointer>
#include <QCompleter>

namespace Ui { class CDbAirlineIcaoSelectorComponent; }

namespace BlackGui
{
    namespace Components
    {
        /*!
         * Distributor selector
         */
        class BLACKGUI_EXPORT CDbAirlineIcaoSelectorComponent :
            public QFrame,
            public BlackMisc::Network::CWebDataServicesAware,
            public BlackGui::CDropBase
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CDbAirlineIcaoSelectorComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CDbAirlineIcaoSelectorComponent();

            //! \copydoc CWebDataReaderAware::setProvider
            virtual void setProvider(BlackMisc::Network::IWebDataServicesProvider *webDataReaderProvider) override;

            //! Current airline ICAO
            void setAirlineIcao(const BlackMisc::Aviation::CAirlineIcaoCode &icao);

            //! Current airline ICAO
            void setAirlineIcao(int key);

            //! Distributor
            BlackMisc::Aviation::CAirlineIcaoCode getAirlineIcao() const;

            //! Read only
            void setReadOnly(bool readOnly);

            //! Display distributor description
            void withIcaoDescription(bool description);

            //! Set with valid Distributor
            bool isSet() const;

            //! Clear selection
            void clear();

        signals:
            //! ICAO was changed
            void changedAirlineIcao(const BlackMisc::Aviation::CAirlineIcaoCode &icao);

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
            //! Distributors have been read
            void ps_codesRead(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState readState, int count);

            //! Data have been changed
            void ps_dataChanged();

            //! Data have been changed
            void ps_completerActivated(const QString &icaoString);

        private:
            QScopedPointer<Ui::CDbAirlineIcaoSelectorComponent> ui;
            QScopedPointer<QCompleter> m_completerIcaoDescription;
            QList<QMetaObject::Connection> m_signals;
            BlackMisc::Aviation::CAirlineIcaoCode m_currentIcao;
        };
    }
}
#endif // guard
