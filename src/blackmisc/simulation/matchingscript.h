/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_MATCHINGSCRIPT_H
#define BLACKMISC_SIMULATION_MATCHINGSCRIPT_H

#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/blackmiscexport.h"

#include <QString>
#include <QObject>

namespace BlackMisc
{
    namespace Simulation
    {
        //! The network values
        class BLACKMISC_EXPORT MSSwiftValues : public QObject
        {
            Q_OBJECT

            //! MSNetworkValues properties @{
            Q_PROPERTY(QString callsign           READ getCallsign           WRITE setCallsign           NOTIFY callsignChanged)
            Q_PROPERTY(int     dbAircraftIcaoId   READ getDbAircraftIcaoId   WRITE setDbAircraftIcaoId   NOTIFY dbAircraftIcaoIdChanged)
            Q_PROPERTY(int     dbAirlineIcaoId    READ getDbAirlineIcaoId    WRITE setDbAirlineIcaoId    NOTIFY dbAirlineIcaoIdChanged)
            Q_PROPERTY(int     dbLiveryId         READ getDbLiveryId         WRITE setDbLiveryId         NOTIFY dbLiveryIdChanged)
            Q_PROPERTY(QString aircraftIcao       READ getAircraftIcao       WRITE setAircraftIcao       NOTIFY aircraftIcaoChanged)
            Q_PROPERTY(QString airlineIcao        READ getAirlineIcao        WRITE setAirlineIcao        NOTIFY airlineIcaoChanged)
            Q_PROPERTY(QString virtualAirlineIcao READ getVirtualAirlineIcao WRITE setVirtualAirlineIcao NOTIFY virtualAirlineIcaoChanged)
            Q_PROPERTY(QString livery             READ getLivery             WRITE setLivery             NOTIFY liveryChanged)
            Q_PROPERTY(QString logMessage         READ getLogMessage         WRITE setLogMessage         NOTIFY logMessageChanged)
            Q_PROPERTY(bool    modified           READ isModified            WRITE setModified           NOTIFY modifiedChanged)
            Q_PROPERTY(bool    rerun              READ isRerun               WRITE setRerun              NOTIFY rerunChanged)
            //! @}

        public:
            //! Ctor
            Q_INVOKABLE MSSwiftValues() {}

            //! Ctor
            Q_INVOKABLE MSSwiftValues(const QString &cs,
                                      const QString &aircraftIcao, int idAircraftIcao,
                                      const QString &airlineIcao,  const QString &virtualAirlineIcao, int idAirlineIcao,
                                      const QString &livery,       int liveryId,
                                      const QString &logMsg = {},
                                      bool  modified = false, bool rerun = false) :
                m_callsign(cs.trimmed().toUpper()),
                m_aircraftIcao(aircraftIcao.trimmed().toUpper()),
                m_airlineIcao(airlineIcao.trimmed().toUpper()), m_vAirlineIcao(virtualAirlineIcao),
                m_livery(livery.trimmed().toUpper()),
                m_dbAircraftIcaoId(idAircraftIcao), m_dbAirlineIcaoId(idAirlineIcao), m_dbLiveryId(liveryId),
                m_logMessage(logMsg),
                m_modified(modified), m_rerun(rerun)
            {}

            //! Ctor
            MSSwiftValues(const QString &cs,
                          const BlackMisc::Aviation::CAircraftIcaoCode aircraftIcao,
                          const BlackMisc::Aviation::CAirlineIcaoCode airlineIcao,
                          const QString &livery, int liveryId);

            //! Ctor
            MSSwiftValues(const QString &cs,
                          const BlackMisc::Aviation::CAircraftIcaoCode aircraftIcao,
                          const BlackMisc::Aviation::CLivery livery);

            //! Ctor
            MSSwiftValues(const MSSwiftValues &sv);

            //! Get callsign
            const QString &getCallsign() const { return m_callsign; }

            //! Set callsign
            void setCallsign(const QString &callsign);

            //! Values found in DB? @{
            bool isFoundDbAircraftIcao() const { return m_dbAircraftIcaoId >= 0; }
            bool isFoundDbAirlineIcao()  const { return m_dbAirlineIcaoId  >= 0; }
            bool isFoundDbLivery()       const { return m_dbLiveryId       >= 0; }
            int getDbAircraftIcaoId()    const { return m_dbAircraftIcaoId; }
            int getDbAirlineIcaoId()     const { return m_dbAirlineIcaoId; }
            int getDbLiveryId()          const { return m_dbLiveryId; }
            void setDbAircraftIcaoId(int id);
            void setDbAirlineIcaoId(int id);
            void setDbLiveryId(int id);
            //! @}

            //! Livery, airline, aircraft @{
            const QString &getAircraftIcao()       const { return m_aircraftIcao; }
            const QString &getAirlineIcao()        const { return m_airlineIcao; }
            const QString &getVirtualAirlineIcao() const { return m_vAirlineIcao; }
            const QString &getLivery()             const { return m_livery; }
            void setAircraftIcao(const QString &aircraftIcao);
            void setAirlineIcao(const QString &airlineIcao);
            void setVirtualAirlineIcao(const QString &virtualAirlineIcao);
            void setLivery(const QString &livery);
            //! @}

            //! Log. message @{
            const QString &getLogMessage() const { return m_logMessage; }
            void setLogMessage(const QString &msg);
            //! @}

            //! Modified flag @{
            bool isModified() const { return m_modified; }
            void setModified(bool modified);
            //! @}

            //! Request re-run @{
            bool isRerun() const { return m_rerun; }
            void setRerun(bool rerun);
            //! @}

            //! Changed values @{
            bool hasChangedAircraftIcao(const BlackMisc::Aviation::CAircraftIcaoCode &aircraftIcao) const;
            bool hasChangedAirlineIcao(const BlackMisc::Aviation::CAirlineIcaoCode &airlineIcao) const;
            //! }

        signals:
            //! Callsign changed
            void callsignChanged();

            //! Livery, airline, aircraft changed @{
            void aircraftIcaoChanged();
            void airlineIcaoChanged();
            void virtualAirlineIcaoChanged();
            void liveryChanged();
            //! @}

            //! DB id changed @{
            void dbAircraftIcaoIdChanged();
            void dbAirlineIcaoIdChanged();
            void dbLiveryIdChanged();
            //! @}

            //! Log. message has been changed
            void logMessageChanged();

            //! Modified changed
            void modifiedChanged();

            //! Re-run changed
            void rerunChanged();

        private:
            QString m_callsign;
            QString m_aircraftIcao;
            QString m_airlineIcao;
            QString m_vAirlineIcao;
            QString m_livery;
            int m_dbAircraftIcaoId = -1;
            int m_dbAirlineIcaoId  = -1;
            int m_dbLiveryId       = -1;
            QString m_logMessage;
            bool m_modified = false;
            bool m_rerun    = false;
        };
    } // namespace
} // namespace

#endif // guard
