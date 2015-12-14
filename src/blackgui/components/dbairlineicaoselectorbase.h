/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_DBAIRLINEICAOSELECTORBASE_H
#define BLACKGUI_DBAIRLINEICAOSELECTORBASE_H

#include "blackcore/webdataservices.h"
#include "blackgui/dropbase.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include <QFrame>
#include <QScopedPointer>
#include <QCompleter>

namespace BlackGui
{
    namespace Components
    {
        /*!
         * Airline ICAO selector base class
         */
        class CDbAirlineIcaoSelectorBase :
            public QFrame,
            public BlackMisc::Network::CWebDataServicesAware,
            public BlackGui::CDropBase
        {
            Q_OBJECT

        public:
            //! Destructor
            virtual ~CDbAirlineIcaoSelectorBase();

            //! \copydoc CWebDataReaderAware::setProvider
            virtual void setProvider(BlackMisc::Network::IWebDataServicesProvider *webDataReaderProvider) override;

            //! Current airline ICAO
            virtual void setAirlineIcao(const BlackMisc::Aviation::CAirlineIcaoCode &icao);

            //! Current airline ICAO by key
            bool setAirlineIcao(int key);

            //! Airline ICAO
            const BlackMisc::Aviation::CAirlineIcaoCode &getAirlineIcao() const { return m_currentIcao; }

            //! Read only
            virtual void setReadOnly(bool readOnly) = 0;

            //! Clear selection
            virtual void clear() = 0;

        signals:
            //! ICAO was changed
            void changedAirlineIcao(const BlackMisc::Aviation::CAirlineIcaoCode &icao);

        protected:
            //! Constructor
            explicit CDbAirlineIcaoSelectorBase(QWidget *parent = nullptr);

            //! Create a new completer
            virtual QCompleter *createCompleter() = 0;

            //! \copydoc QWidget::dragEnterEvent
            virtual void dragEnterEvent(QDragEnterEvent *event) override;

            //! \copydoc QWidget::dragMoveEvent
            virtual void dragMoveEvent(QDragMoveEvent *event) override;

            //! \copydoc QWidget::dragLeaveEvent
            virtual void dragLeaveEvent(QDragLeaveEvent *event) override;

            //! \copydoc QWidget::dropEvent
            virtual void dropEvent(QDropEvent *event) override;

            QScopedPointer<QCompleter>            m_completer;   //!< completer used
            BlackMisc::Aviation::CAirlineIcaoCode m_currentIcao; //!< current ICAO object

        private slots:
            //! Airlines have been read
            void ps_codesRead(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState readState, int count);

            //! Data have been changed
            void ps_completerActivated(const QString &icaoString);
        };
    } // ns
} // ns
#endif // guard
