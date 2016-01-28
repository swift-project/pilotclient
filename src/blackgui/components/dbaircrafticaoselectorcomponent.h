/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_DBAIRCRAFTICAOSELECTORCOMPONENT_H
#define BLACKGUI_COMPONENTS_DBAIRCRAFTICAOSELECTORCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackcore/webdataservices.h"
#include "blackgui/dropbase.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include <QFrame>
#include <QScopedPointer>
#include <QCompleter>

namespace Ui { class CDbAircraftIcaoSelectorComponent; }

namespace BlackGui
{
    namespace Components
    {
        /*!
         * Aircraft ICAO selector
         */
        class BLACKGUI_EXPORT CDbAircraftIcaoSelectorComponent :
            public QFrame,
            public BlackMisc::Network::CWebDataServicesAware,
            public BlackGui::CDropBase
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CDbAircraftIcaoSelectorComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CDbAircraftIcaoSelectorComponent();

            //! \copydoc CWebDataReaderAware::setProvider
            virtual void setProvider(BlackMisc::Network::IWebDataServicesProvider *webDataReaderProvider) override;

            //! Current aircraft ICAO
            void setAircraftIcao(const BlackMisc::Aviation::CAircraftIcaoCode &icao);

            //! Current aircraft ICAO
            void setAircraftIcao(int key);

            //! Distributor
            BlackMisc::Aviation::CAircraftIcaoCode getAircraftIcao() const;

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
            void changedAircraftIcao(const BlackMisc::Aviation::CAircraftIcaoCode &icao);

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
            QScopedPointer<Ui::CDbAircraftIcaoSelectorComponent> ui;
            QScopedPointer<QCompleter> m_completerIcaoDescription;
            BlackMisc::Aviation::CAircraftIcaoCode m_currentIcao;
        };
    }
}
#endif // guard
