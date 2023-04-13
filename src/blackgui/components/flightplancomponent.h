/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_FLIGHTPLANCOMPONENT_H
#define BLACKGUI_COMPONENTS_FLIGHTPLANCOMPONENT_H

#include "blackgui/overlaymessagesframe.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/simulation/data/lastmodel.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/aviation/flightplan.h"
#include "blackmisc/network/data/lastserver.h"
#include "blackmisc/network/user.h"
#include "blackmisc/directories.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/datacache.h"
#include "blackmisc/identifier.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/logcategories.h"

#include <QObject>
#include <QScopedPointer>
#include <QStringList>
#include <QTabWidget>
#include <QtGlobal>
#include <QFileDialog>
#include <QNetworkReply>

namespace Ui
{
    class CFlightPlanComponent;
}
namespace BlackGui::Components
{
    namespace FlightPlanSettings
    {
        //! Remarks history
        struct TRemarksHistory : public BlackMisc::TSettingTrait<QStringList>
        {
            //! \copydoc BlackMisc::TSettingTrait::key
            static const char *key() { return "flightplan/remarkshistory"; }

            //! \copydoc BlackMisc::TSettingTrait::humanReadable
            static const QString &humanReadable()
            {
                static const QString name("FP remarks history");
                return name;
            }
        };

        //! Additional remarks history
        struct TRemarksHistoryAdditional : public BlackMisc::TSettingTrait<QStringList>
        {
            //! \copydoc BlackMisc::TSettingTrait::key
            static const char *key() { return "flightplan/remarkshistoryadd"; }

            //! \copydoc BlackMisc::TSettingTrait::humanReadable
            static const QString &humanReadable()
            {
                static const QString name("FP remarks history (add)");
                return name;
            }
        };
    }

    class CStringListDialog;
    class CAltitudeDialog;
    class CSimBriefDownloadDialog;

    //! Flight plan widget
    class BLACKGUI_EXPORT CFlightPlanComponent : public COverlayMessagesTabWidget
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CFlightPlanComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CFlightPlanComponent() override;

        //! Login data were set
        void loginDataSet();

        //! Prefill with aircraft dara
        void fillWithFlightPlanData(const BlackMisc::Aviation::CFlightPlan &flightPlan);

        //! Get sent flight plan
        const BlackMisc::Aviation::CFlightPlan &getSentFlightPlan() const { return m_sentFlightPlan; }

        //! Log.categories
        static const QStringList &getLogCategories();

    private:
        static constexpr int OverlayTimeoutMs = 5000;
        QScopedPointer<Ui::CFlightPlanComponent> ui;
        CAltitudeDialog *m_altitudeDialog = nullptr;
        CStringListDialog *m_fpRemarksDialog = nullptr;
        CSimBriefDownloadDialog *m_simBriefDialog = nullptr;
        BlackMisc::Aviation::CFlightPlan m_sentFlightPlan; //!< my flight plan
        BlackMisc::Simulation::CAircraftModel m_model; //!< currently used model
        BlackMisc::CIdentifier m_identifier { "FlightPlanComponent", this }; //!< Flightplan identifier
        BlackMisc::CSetting<BlackMisc::Settings::TDirectorySettings> m_directories { this }; //!< the swift directories
        BlackMisc::CSetting<FlightPlanSettings::TRemarksHistory> m_remarksHistory { this }; //!< remarks history
        BlackMisc::CSetting<FlightPlanSettings::TRemarksHistoryAdditional> m_remarksHistoryAdditional { this }; //!< remarks history
        BlackMisc::CDataReadOnly<BlackMisc::Simulation::Data::TLastModel> m_lastAircraftModel { this }; //!< recently used aircraft model
        BlackMisc::CDataReadOnly<BlackMisc::Network::Data::TLastServer> m_lastServer { this }; //!< recently used server (VATSIM, other)

        //! Validate, generates status messages
        BlackMisc::CStatusMessageList validateAndInitializeFlightPlan(BlackMisc::Aviation::CFlightPlan &fligtPlan);

        //! Default value for ICAO airports
        static const QString &defaultIcao()
        {
            static const QString d("ICAO");
            return d;
        }

        //! Default value for time
        static const QString &defaultTime()
        {
            static const QString t("00:00");
            return t;
        }

        //! Identifier
        const BlackMisc::CIdentifier &flightPlanIdentifier() const { return m_identifier; }

        //! Prefill with own aircraft data
        void prefillWithOwnAircraftData();

        //! Prefill with aircraft data
        void prefillWithAircraftData(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, bool force = false);

        //! Prefill with user data
        void prefillWithUserData(const BlackMisc::Network::CUser &user);

        //! Set completers
        void initCompleters();

        //! File name for load/save
        QString getDefaultFilename(bool load);

        //! Call \sa buildRemarksString from combo box signal
        void currentTextChangedToBuildRemarks(const QString &text)
        {
            this->buildRemarksString();
            Q_UNUSED(text)
        }

        //! Voice combo boxes shall display the same
        void syncVoiceComboBoxes(const QString &text);

        //! Send flightplan
        void sendFlightPlan();

        //! Reset Flightplan
        void resetFlightPlan();

        //! Load from disk
        void loadFromDisk();

        //! Load template from disk
        void loadTemplateFromDisk();

        //! Save to disk
        void saveToDisk();

        //! Save FP to disk
        bool saveFPToDisk(const QString &fileName);

        //! Save template to disk
        void saveTemplateToDisk();

        //! Clear template
        void clearTemplate();

        //! Get the template name (full path and file)
        QString getTemplateName() const;

        //! Set SELCAL in own aircrafr
        void setSelcalInOwnAircraft();

        //! Load Flightplan
        void loadFlightPlanFromNetwork();

        //! Validate Flightplan
        void validateFlightPlan();

        //! Remarks
        void buildRemarksString();

        //! Copy over
        void copyRemarksConfirmed() { this->copyRemarks(true); }

        //! Copy over
        void copyRemarks(bool confirm = true);

        //! Show generator tab page
        void currentTabGenerator();

        //! GUI init complete
        void swiftWebDataRead();

        //! Build "H/B737/F"
        void buildPrefixIcaoSuffix();

        //! Prefix check box changed
        void prefixCheckBoxChanged();

        //! Aircraft type changed
        void aircraftTypeChanged();

        //! Sync.with simulator
        void syncWithSimulator();

        //! Get prefix
        QString getPrefix() const;

        //! Aircraft type as ICAO code
        BlackMisc::Aviation::CAircraftIcaoCode getAircraftIcaoCode() const;

        //! Get equipment code (1 char)
        QString getEquipmentSuffix() const;

        //! Something like "H/B737/F"
        QString getCombinedPrefixIcaoSuffix() const;

        //! Show tab of equipment codes
        void showEquipmentCodesTab();

        //! VFR rules?
        bool isVfr() const;

        //! Get the FP flight rules
        BlackMisc::Aviation::CFlightPlan::FlightRules getFlightRules() const;

        //! Override remarks message dialog
        bool overrideRemarks();

        //! Guess some FP values/setting
        void anticipateValues();

        //! Update the diretcory settings
        void updateDirectorySettings(const QString &fileOrDirectory);

        //! Altitude dialog
        void altitudeDialog();

        //! FP remarks history selection
        void remarksHistory();

        //! Update the remarks histories
        void updateRemarksHistories();

        //! Set remark values
        void setRemarksUIValues(const QString &remarks);

        //! Load from SimBrief
        void loadFromSimBrief();

        //! Response from SimBrief
        void handleSimBriefResponse(QNetworkReply *nwReplyPtr);

        //! Consolidate the new remarks list, latest on front
        static bool consolidateRemarks(QStringList &remarks, const QString &newRemarks);
    };
} // ns
#endif // guard
