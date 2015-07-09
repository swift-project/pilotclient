/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKGUI_DATAMAPPINGCOMPONENT_H
#define BLACKGUI_DATAMAPPINGCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include <QFrame>

namespace Ui { class CDataMappingComponent; }
namespace BlackCore { class CWebDataReader; }

namespace BlackGui
{
    namespace Components
    {
        /**
         * Mappings from/for the database
         */
        class BLACKGUI_EXPORT CDataMappingComponent : public QFrame
        {
            Q_OBJECT

        public:
            //! Contructor
            explicit CDataMappingComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CDataMappingComponent();

            //! Readers have been initialized
            void readersInitialized(BlackCore::CWebDataReader *webReaders);

        private slots:
            //! Aircraft ICAO codes read
            void ps_aircraftIcaoCodeRead(int number);

            //! Airline ICAO codes read
            void ps_airlineIcaoCodeRead(int number);

            //! Liveries read
            void ps_liveriesRead(int number);

            //! Distributors read
            void ps_distributorsRead(int number);

            //! Models read
            void ps_modelsRead(int number);

            //! Request distributor update
            void  ps_requestModelDataUpdate();

        private:
            QScopedPointer<Ui::CDataMappingComponent> ui;
            BlackCore::CWebDataReader     *m_webDataReader = nullptr;
            QList<QMetaObject::Connection> m_webReaderSignalConnections;
        };

    } // ns
} // ns

#endif // guard
