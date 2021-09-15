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

#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/blackmiscexport.h"

#include <QString>
#include <QObject>

namespace BlackMisc::Simulation
{
    //! The network values
    class BLACKMISC_EXPORT MSInOutValues : public QObject
    {
        Q_OBJECT

        //! MSNetworkValues properties
        //! @{
        Q_PROPERTY(QString callsign           READ getCallsign           WRITE setCallsign           NOTIFY callsignChanged)
        Q_PROPERTY(QString callsignAsSet      READ getCallsignAsSet)
        Q_PROPERTY(QString flightNumber       READ getFlightNumber)
        Q_PROPERTY(int     dbAircraftIcaoId   READ getDbAircraftIcaoId   WRITE setDbAircraftIcaoId   NOTIFY dbAircraftIcaoIdChanged)
        Q_PROPERTY(int     dbAirlineIcaoId    READ getDbAirlineIcaoId    WRITE setDbAirlineIcaoId    NOTIFY dbAirlineIcaoIdChanged)
        Q_PROPERTY(int     dbLiveryId         READ getDbLiveryId         WRITE setDbLiveryId         NOTIFY dbLiveryIdChanged)
        Q_PROPERTY(int     dbModelId          READ getDbModelId          WRITE setDbModelId          NOTIFY dbModelIdChanged)
        Q_PROPERTY(QString aircraftIcao       READ getAircraftIcao       WRITE setAircraftIcao       NOTIFY aircraftIcaoChanged)
        Q_PROPERTY(QString aircraftFamily     READ getAircraftFamily)
        Q_PROPERTY(QString airlineIcao        READ getAirlineIcao        WRITE setAirlineIcao        NOTIFY airlineIcaoChanged)
        Q_PROPERTY(QString virtualAirlineIcao READ getVirtualAirlineIcao WRITE setVirtualAirlineIcao NOTIFY virtualAirlineIcaoChanged)
        Q_PROPERTY(QString livery             READ getLivery             WRITE setLivery             NOTIFY liveryChanged)
        Q_PROPERTY(QString modelString        READ getModelString        WRITE setModelString        NOTIFY modelStringChanged)
        Q_PROPERTY(QString combinedType       READ getCombinedType       WRITE setCombinedType       NOTIFY combinedTypeChanged)
        Q_PROPERTY(QString logMessage         READ getLogMessage         WRITE setLogMessage         NOTIFY logMessageChanged)
        Q_PROPERTY(bool    hasAircraftFamily  READ hasAircraftFamily)
        Q_PROPERTY(bool    modified           READ isModified            WRITE setModified           NOTIFY modifiedChanged)
        Q_PROPERTY(bool    rerun              READ isRerun               WRITE setRerun              NOTIFY rerunChanged)
        // ----- status values -------
        Q_PROPERTY(bool    hasAirlineIcao     READ hasAirlineIcao)
        Q_PROPERTY(bool    hasAircraftIcao    READ hasAircraftIcao)
        Q_PROPERTY(bool    hasModifiedAircraftIcaoDesignator READ hasModifiedAircraftIcaoDesignator)
        Q_PROPERTY(bool    hasModifiedAircraftFamily         READ hasModifiedAircraftFamily)
        Q_PROPERTY(bool    hasModifiedAirlineIcaoDesignator  READ hasModifiedAirlineIcaoDesignator)
        Q_PROPERTY(bool    hasUnmodifiedDesignators          READ hasUnmodifiedDesignators)
        //! @}

    public:
        //! Ctor
        Q_INVOKABLE MSInOutValues() {}

        //! Ctor
        Q_INVOKABLE MSInOutValues(const QString &cs,           const QString &csAsSet,            const QString &flightNumber,
                                    const QString &aircraftIcao, const QString &aircraftFamily,     const QString &combinedType,       int idAircraftIcao,
                                    const QString &airlineIcao,  const QString &virtualAirlineIcao, int idAirlineIcao,
                                    const QString &livery,       int liveryId,
                                    const QString &logMsg = {},
                                    bool  modified = false, bool rerun = false) :
            m_callsign(cs.trimmed().toUpper()),               m_callsignAsSet(csAsSet),                              m_flightNumber(flightNumber),
            m_aircraftIcao(aircraftIcao.trimmed().toUpper()), m_aircraftFamily(aircraftFamily.trimmed().toUpper()),  m_combinedType(combinedType.trimmed().toUpper()),
            m_airlineIcao(airlineIcao.trimmed().toUpper()),   m_vAirlineIcao(virtualAirlineIcao),
            m_livery(livery.trimmed().toUpper()),
            m_dbAircraftIcaoId(idAircraftIcao), m_dbAirlineIcaoId(idAirlineIcao), m_dbLiveryId(liveryId),
            m_logMessage(logMsg),
            m_modified(modified), m_rerun(rerun)
        {}

        //! Ctor
        MSInOutValues(const BlackMisc::Aviation::CCallsign &cs,
                        const BlackMisc::Aviation::CAircraftIcaoCode &aircraftIcao,
                        const BlackMisc::Aviation::CAirlineIcaoCode &airlineIcao,
                        const QString &livery, int liveryId);

        //! Ctor
        MSInOutValues(const BlackMisc::Aviation::CCallsign &cs,
                        const BlackMisc::Aviation::CAircraftIcaoCode &aircraftIcao,
                        const BlackMisc::Aviation::CLivery &livery);

        //! Ctor
        MSInOutValues(const BlackMisc::Simulation::CAircraftModel &model);

        //! Ctor
        MSInOutValues(const MSInOutValues &sv);

        //! Callsign values
        //! @{
        const QString &getCallsign()      const { return m_callsign; }
        const QString &getCallsignAsSet() const { return m_callsignAsSet; }
        const QString &getFlightNumber()  const { return m_flightNumber; }
        void setCallsign(const QString &callsign);
        //! @}

        //! Values found in DB?
        //! @{
        bool isFoundDbAircraftIcao() const { return m_dbAircraftIcaoId >= 0; }
        bool isFoundDbAirlineIcao()  const { return m_dbAirlineIcaoId  >= 0; }
        bool isFoundDbLivery()       const { return m_dbLiveryId       >= 0; }
        bool isFoundDbModel()        const { return m_dbModelId        >= 0; }
        int getDbAircraftIcaoId()    const { return m_dbAircraftIcaoId; }
        int getDbAirlineIcaoId()     const { return m_dbAirlineIcaoId; }
        int getDbLiveryId()          const { return m_dbLiveryId; }
        int getDbModelId()           const { return m_dbModelId; }
        void setDbAircraftIcaoId(int id);
        void setDbAirlineIcaoId(int id);
        void setDbLiveryId(int id);
        void setDbModelId(int id);
        //! @}

        //! Livery, airline, aircraft, model string
        //! @{
        const QString &getAircraftIcao()       const { return m_aircraftIcao; }
        const QString &getAircraftFamily()     const { return m_aircraftFamily; }
        const QString &getAirlineIcao()        const { return m_airlineIcao; }
        const QString &getVirtualAirlineIcao() const { return m_vAirlineIcao; }
        const QString &getLivery()             const { return m_livery; }
        const QString &getModelString()        const { return m_modelString; }
        const QString &getCombinedType()       const { return m_combinedType; }
        void setAircraftIcao(const QString &aircraftIcao);
        void setAirlineIcao(const QString &airlineIcao);
        void setVirtualAirlineIcao(const QString &virtualAirlineIcao);
        void setLivery(const QString &livery);
        void setModelString(const QString &modelString);
        void setCombinedType(const QString &type);
        bool hasAircraftIcao()   const { return !m_aircraftIcao.isEmpty();   }
        bool hasAirlineIcao()    const { return !m_airlineIcao.isEmpty();    }
        bool hasAircraftFamily() const { return !m_aircraftFamily.isEmpty(); }
        //! @}

        //! Log. message
        //! @{
        const QString &getLogMessage() const { return m_logMessage; }
        void setLogMessage(const QString &msg);
        //! @}

        //! Modified flag
        //! @{
        bool isModified() const { return m_modified; }
        void setModified(bool modified);
        //! @}

        //! Request re-run
        //! @{
        bool isRerun() const { return m_rerun; }
        void setRerun(bool rerun);
        //! @}

        //! Changed values such as modified values
        void evaluateChanges(const BlackMisc::Aviation::CAircraftIcaoCode &aircraft, const BlackMisc::Aviation::CAirlineIcaoCode &airline);

        //! Changed values
        //! @{
        bool hasModifiedAircraftIcaoDesignator() const { return m_modifiedAircraftDesignator; }
        bool hasModifiedAirlineIcaoDesignator()  const { return m_modifiedAirlineDesignator;  }
        bool hasModifiedAircraftFamily() const { return m_modifiedAircraftFamily; }
        bool hasUnmodifiedDesignators()  const { return !this->hasModifiedAirlineIcaoDesignator() && !this->hasModifiedAircraftIcaoDesignator();  }
        bool hasChangedAircraftIcao(const BlackMisc::Aviation::CAircraftIcaoCode &aircraftIcao) const;
        bool hasChangedAircraftIcaoId(const BlackMisc::Aviation::CAircraftIcaoCode &aircraftIcao) const;
        bool hasChangedAirlineIcao(const BlackMisc::Aviation::CAirlineIcaoCode &airlineIcao) const;
        bool hasChangedAirlineIcaoId(const BlackMisc::Aviation::CAirlineIcaoCode &airlineIcao) const;
        bool hasChangedModelString(const QString &modelString) const;
        bool hasChangedLiveryId(const BlackMisc::Aviation::CLivery &livery) const;
        bool hasChangedModelId(const BlackMisc::Simulation::CAircraftModel &model) const;
        bool hasChangedModel(const BlackMisc::Simulation::CAircraftModel &model) const;
        //! @}

    signals:
        //! Callsign changed
        void callsignChanged();

        //! Livery, airline, aircraft, or model changed
        //! @{
        void aircraftIcaoChanged();
        void airlineIcaoChanged();
        void virtualAirlineIcaoChanged();
        void liveryChanged();
        void modelStringChanged();
        void combinedTypeChanged();
        //! @}

        //! DB id changed
        //! @{
        void dbAircraftIcaoIdChanged();
        void dbAirlineIcaoIdChanged();
        void dbLiveryIdChanged();
        void dbModelIdChanged();
        //! @}

        //! Log. message has been changed
        void logMessageChanged();

        //! Modified changed
        void modifiedChanged();

        //! Re-run changed
        void rerunChanged();

    private:
        QString m_callsign;
        QString m_callsignAsSet;
        QString m_flightNumber;
        QString m_aircraftIcao;
        QString m_aircraftFamily;
        QString m_combinedType;
        QString m_airlineIcao;
        QString m_vAirlineIcao;
        QString m_livery;
        QString m_modelString;
        int m_dbAircraftIcaoId = -1;
        int m_dbAirlineIcaoId  = -1;
        int m_dbLiveryId       = -1;
        int m_dbModelId        = -1;
        QString m_logMessage;
        bool m_modifiedAircraftDesignator = false;
        bool m_modifiedAircraftFamily     = false;
        bool m_modifiedAirlineDesignator  = false;
        bool m_modified = false;
        bool m_rerun    = false;
    };

    //! The model set values
    class BLACKMISC_EXPORT MSModelSet : public QObject
    {
        Q_OBJECT

        //! MSModelSet properties
        //! @{
        Q_PROPERTY(QString simulator  READ getSimulator  WRITE setSimulator  NOTIFY simulatorChanged)
        Q_PROPERTY(bool available     READ isAvailable   WRITE setAvailable  NOTIFY availabilityChanged)
        Q_PROPERTY(int inputAircraftAndAirlineCount READ getInputAircraftAndAirlineCount WRITE setInputAircraftAndAirlineCount  NOTIFY inputAircraftAndAirlineCountChanged)
        Q_PROPERTY(int modelSetSize   READ getModelSetSize)
        Q_PROPERTY(int countDifferentAirlines  READ countVtolAircraft)
        Q_PROPERTY(int countVtolAircraft       READ countCivilianAircraft)
        Q_PROPERTY(int countCivilianAircraft   READ countCivilianAircraft)
        Q_PROPERTY(int countMilitaryAircraft   READ countMilitaryAircraft)
        Q_PROPERTY(int countModelsWithColorLivery   READ countModelsWithColorLivery)
        Q_PROPERTY(int countModelsWithAirlineLivery READ countModelsWithAirlineLivery)
        //! @}

    public:
        //! Ctor
        Q_INVOKABLE MSModelSet() {}

        //! Get callsign
        const QString &getSimulator() const { return m_simulator; }

        //! Set callsign
        void setSimulator(const QString &simulator);

        //! Availablity flag
        //! @{
        bool isAvailable() const { return m_available; }
        void setAvailable(bool available);
        //! @}

        //! Input aircraft/airline available flag
        //! @{
        int getInputAircraftAndAirlineCount() const { return m_inputAircraftAndAirlineCount; }
        void setInputAircraftAndAirlineCount(int count);
        //! @}

        //! Model set functions as properties
        //! @{
        int getModelSetSize() const { return m_modelSet.size(); }
        int countDifferentAirlines() const { return m_modelSet.countDifferentAirlines(); }
        int countVtolAircraft() const { return m_modelSet.countVtolAircraft(); }
        int countCivilianAircraft() const { return m_modelSet.countCivilianAircraft(); }
        int countMilitaryAircraft() const { return m_modelSet.countMilitaryAircraft(); }
        int countModelsWithColorLivery() const { return m_modelSet.countModelsWithColorLivery(); }
        int countModelsWithAirlineLivery() const { return m_modelSet.countModelsWithAirlineLivery(); }
        //! @}

        //! Invokable model set functions as properties
        //! @{
        Q_INVOKABLE bool containsModelString(const QString &modelString) const { return m_modelSet.containsModelString(modelString, Qt::CaseInsensitive); }
        Q_INVOKABLE bool containsDbKey(int dbKey) const { return m_modelSet.containsDbKey(dbKey); }
        Q_INVOKABLE bool containsCombinedType(const QString &ct) const { return m_modelSet.containsCombinedType(ct); }
        //! @}

        //! Model string of model with closest color distance
        Q_INVOKABLE QString findCombinedTypeWithClosestColorLivery(const QString &combinedType, const QString &rgbColor) const;

        //! Model string of model with closest color distance
        Q_INVOKABLE QString findClosestCombinedTypeWithClosestColorLivery(const QString &combinedType, const QString &rgbColor) const;

        //! Model string of model with closest color distance
        Q_INVOKABLE QString findAircraftFamilyWithClosestColorLivery(const QString &family, const QString &rgbColor) const;

        // ------- NOT EXPOSED TO JAVASCRIPT -------

        //! Ctor
        MSModelSet(const BlackMisc::Simulation::CAircraftModelList &modelSet);

        //! Init by model set
        void initByModelSet(const BlackMisc::Simulation::CAircraftModelList &modelSet);

        //! Init by aircraft/airline
        void initByAircraftAndAirline(const BlackMisc::Aviation::CAircraftIcaoCode &aircraft, const BlackMisc::Aviation::CAirlineIcaoCode &airline);

        // ------- NOT EXPOSED TO JAVASCRIPT -------

    signals:
        //! Simulator changed
        void simulatorChanged();

        //! Has been modified
        void availabilityChanged();

        //! Aircraft and airline changed
        void inputAircraftAndAirlineCountChanged();

    private:
        QString m_simulator;
        bool m_available = false;
        int m_inputAircraftAndAirlineCount; //! values in set for airline and aircraft
        BlackMisc::Simulation::CAircraftModelList m_modelSet;
    };

} // namespace

#endif // guard
