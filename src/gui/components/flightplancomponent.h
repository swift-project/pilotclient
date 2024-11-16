// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_FLIGHTPLANCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_FLIGHTPLANCOMPONENT_H

#include <QFileDialog>
#include <QListWidget>
#include <QNetworkReply>
#include <QObject>
#include <QScopedPointer>
#include <QStringList>
#include <QTabWidget>
#include <QtGlobal>

#include "gui/overlaymessagesframe.h"
#include "gui/swiftguiexport.h"
#include "misc/aviation/comnavequipment.h"
#include "misc/aviation/flightplan.h"
#include "misc/aviation/ssrequipment.h"
#include "misc/datacache.h"
#include "misc/directories.h"
#include "misc/identifier.h"
#include "misc/logcategories.h"
#include "misc/network/data/lastserver.h"
#include "misc/network/user.h"
#include "misc/settingscache.h"
#include "misc/simulation/data/lastmodel.h"
#include "misc/simulation/simulatedaircraft.h"
#include "misc/statusmessagelist.h"

namespace Ui
{
    class CFlightPlanComponent;
}
namespace swift::gui::components
{
    namespace FlightPlanSettings
    {
        //! Remarks history
        struct TRemarksHistory : public swift::misc::TSettingTrait<QStringList>
        {
            //! \copydoc swift::misc::TSettingTrait::key
            static const char *key() { return "flightplan/remarkshistory"; }

            //! \copydoc swift::misc::TSettingTrait::humanReadable
            static const QString &humanReadable()
            {
                static const QString name("FP remarks history");
                return name;
            }
        };

        //! Additional remarks history
        struct TRemarksHistoryAdditional : public swift::misc::TSettingTrait<QStringList>
        {
            //! \copydoc swift::misc::TSettingTrait::key
            static const char *key() { return "flightplan/remarkshistoryadd"; }

            //! \copydoc swift::misc::TSettingTrait::humanReadable
            static const QString &humanReadable()
            {
                static const QString name("FP remarks history (add)");
                return name;
            }
        };
    } // namespace FlightPlanSettings

    class CStringListDialog;
    class CAltitudeDialog;
    class CSimBriefDownloadDialog;

    //! Flight plan widget
    class SWIFT_GUI_EXPORT CFlightPlanComponent : public COverlayMessagesTabWidget
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
        void fillWithFlightPlanData(const swift::misc::aviation::CFlightPlan &flightPlan);

        //! Get sent flight plan
        const swift::misc::aviation::CFlightPlan &getSentFlightPlan() const { return m_sentFlightPlan; }

        //! Log.categories
        static const QStringList &getLogCategories();

    private:
        static constexpr int OverlayTimeoutMs = 5000;
        QScopedPointer<Ui::CFlightPlanComponent> ui;
        CAltitudeDialog *m_altitudeDialog = nullptr;
        CStringListDialog *m_fpRemarksDialog = nullptr;
        CSimBriefDownloadDialog *m_simBriefDialog = nullptr;
        QMenu *m_navComEquipmentMenu = nullptr;
        QMenu *m_ssrEquipmentMenu = nullptr;
        swift::misc::aviation::CFlightPlan m_sentFlightPlan; //!< my flight plan
        swift::misc::simulation::CAircraftModel m_model; //!< currently used model
        swift::misc::aviation::CComNavEquipment m_navComEquipment; //!< current NAV/COM equipment
        swift::misc::aviation::CSsrEquipment m_ssrEquipment; //!< current SSR equipment
        swift::misc::CIdentifier m_identifier { "FlightPlanComponent", this }; //!< Flightplan identifier
        swift::misc::CSetting<swift::misc::settings::TDirectorySettings> m_directories { this }; //!< the swift directories
        swift::misc::CSetting<FlightPlanSettings::TRemarksHistory> m_remarksHistory { this }; //!< remarks history
        swift::misc::CSetting<FlightPlanSettings::TRemarksHistoryAdditional> m_remarksHistoryAdditional { this }; //!< remarks history
        swift::misc::CDataReadOnly<swift::misc::simulation::data::TLastModel> m_lastAircraftModel { this }; //!< recently used aircraft model
        swift::misc::CDataReadOnly<swift::misc::network::data::TLastServer> m_lastServer { this }; //!< recently used server (VATSIM, other)

        struct WakeTurbulenceEntry
        {
            QString m_name; //!< Name displayed to the user
            swift::misc::aviation::CWakeTurbulenceCategory m_wtc; //!< Wake turbulence category
        };

        const QList<WakeTurbulenceEntry> m_wakeTurbulenceCategories = {
            { "Light", swift::misc::aviation::CWakeTurbulenceCategory::LIGHT },
            { "Medium", swift::misc::aviation::CWakeTurbulenceCategory::MEDIUM },
            { "Heavy", swift::misc::aviation::CWakeTurbulenceCategory::HEAVY },
            { "Super", swift::misc::aviation::CWakeTurbulenceCategory::SUPER }
        }; //!< Mapping CWakeTurbulenceCategory <=> UI name that is shown to the user

        //! Validate, generates status messages
        swift::misc::CStatusMessageList validateAndInitializeFlightPlan(swift::misc::aviation::CFlightPlan &fligtPlan);

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
        const swift::misc::CIdentifier &flightPlanIdentifier() const { return m_identifier; }

        //! Prefill with own aircraft data
        void prefillWithOwnAircraftData();

        //! Prefill with aircraft data
        void prefillWithAircraftData(const swift::misc::simulation::CSimulatedAircraft &aircraft, bool force = false);

        //! Prefill with user data
        void prefillWithUserData(const swift::misc::network::CUser &user);

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

        //! Aircraft type changed
        void aircraftTypeChanged();

        //! Sync.with simulator
        void syncWithSimulator();

        //! Aircraft type as ICAO code
        swift::misc::aviation::CAircraftIcaoCode getAircraftIcaoCode() const;

        //! Show tab of equipment codes
        void showEquipmentCodesTab();

        //! VFR rules?
        bool isVfr() const;

        //! Get the FP flight rules
        swift::misc::aviation::CFlightPlan::FlightRules getFlightRules() const;

        //! Override remarks message dialog
        bool overrideRemarks();

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

        //! Setup NAV/COM context menu
        void setupNavComContextMenu();

        //! Setup SSR context menu
        void setupSsrContextMenu();

        //! Update the current NAV/COM equipment from the menu selection
        void updateNavComEquipmentFromSelection();

        //! Update the current SSR equipment from the menu selection
        void updateSsrEquipmentFromSelection();

        //! Update the selection and label according to current NAV/COM equipment
        void updateNavComEquipmentUi();

        //! Update the selection and label according to current SSR equipment
        void updateSsrEquipmentUi();

        //! Update the wake turbulence category selector
        void updateWakeTurbulenceCategorySelector(const swift::misc::aviation::CWakeTurbulenceCategory &wtc);

        //! Get the selected wake turbulence category
        swift::misc::aviation::CWakeTurbulenceCategory getSelectedWakeTurbulenceCategory() const;

        //! Helper to get the QListWidget from a given NAV/COM or SSR menu
        static QListWidget *getMenuEquipmentList(QMenu *menu);

        //! Helper to mark all options in \p enabledOptions as "selected" in the QListWidget, contained in the NAV/COM or SSR \p menu
        static void updateListSelection(QMenu *menu, const QStringList &enabledOptions);

        //! Consolidate the new remarks list, latest on front
        static bool consolidateRemarks(QStringList &remarks, const QString &newRemarks);
    };
} // namespace swift::gui::components
#endif // guard
