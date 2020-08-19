/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/iconlist.h"
#include "blackmisc/containerbase.h"
#include "blackmisc/dbus.h"
#include "blackmisc/range.h"
#include "blackmisc/variant.h"
#include <QDBusMetaType>

namespace BlackMisc
{
    CIconList::CIconList(const CSequence<CIcon> &other) :
        CSequence<CIcon>(other)
    { }

    CIcon CIconList::findByIndex(CIcons::IconIndex index) const
    {
        return this->findBy(&CIcon::getIndex, index).frontOrDefault();
    }

    void CIconList::registerMetadata()
    {
        qRegisterMetaType<BlackMisc::CSequence<CIcon>>();
        qDBusRegisterMetaType<BlackMisc::CSequence<CIcon>>();
        qRegisterMetaTypeStreamOperators<BlackMisc::CSequence<CIcon>>();
        qRegisterMetaType<BlackMisc::CCollection<CIcon>>();
        qDBusRegisterMetaType<BlackMisc::CCollection<CIcon>>();
        qRegisterMetaTypeStreamOperators<BlackMisc::CCollection<CIcon>>();
        qRegisterMetaType<CIconList>();
        qDBusRegisterMetaType<CIconList>();
        qRegisterMetaTypeStreamOperators<CIconList>();
        registerMetaValueType<CIconList>();
    }

    const CIconList &CIconList::allIcons()
    {
        // keep indexes alphabetically sorted and in sync with CIcons
        // req. for "iconByIndex"
        static const CIconList icons(
        {
            CIcon(CIcons::ApplicationAircraft, "aircraft"),
            CIcon(CIcons::ApplicationAircraftCategories, "categories"),
            CIcon(CIcons::ApplicationAircraftIcao, "ICAO aircraft"),
            CIcon(CIcons::ApplicationAirlineIcao, "ICAO airlines"),
            CIcon(CIcons::ApplicationAtc, "ATC"),
            CIcon(CIcons::ApplicationAudio, "audio"),
            CIcon(CIcons::ApplicationCockpit, "cockpit"),
            CIcon(CIcons::ApplicationCountries, "countries"),
            CIcon(CIcons::ApplicationDatabase, "database"),
            CIcon(CIcons::ApplicationDbStash, "DB stash"),
            CIcon(CIcons::ApplicationDistributors, "distributors"),
            CIcon(CIcons::ApplicationFlightPlan, "flightplan"),
            CIcon(CIcons::ApplicationLiveries, "liveries"),
            CIcon(CIcons::ApplicationLog, "log"),
            CIcon(CIcons::ApplicationMappings, "mappings"),
            CIcon(CIcons::ApplicationModels, "models"),
            CIcon(CIcons::ApplicationSettings, "settings"),
            CIcon(CIcons::ApplicationSimulator, "simulator"),
            CIcon(CIcons::ApplicationTerminal, "terminal"),
            CIcon(CIcons::ApplicationTextMessages, "text messages"),
            CIcon(CIcons::ApplicationWeather, "weather"),
            CIcon(CIcons::ApplicationRadar, "radar"),
            CIcon(CIcons::AviationAtis, "ATIS"),
            CIcon(CIcons::AviationAttitudeIndicator, "attitude indicator"),
            CIcon(CIcons::AviationMetar, "METAR"),
            CIcon(CIcons::GeoPosition, "geo position"),
            CIcon(CIcons::ModelExclude, "exclude model"),
            CIcon(CIcons::ModelInclude, "include model"),
            CIcon(CIcons::NetworkCapabilityTextOnly, "text only"),
            CIcon(CIcons::NetworkCapabilityUnknown, "unknown"),
            CIcon(CIcons::NetworkCapabilityVoice, "voice"),
            CIcon(CIcons::NetworkCapabilityVoiceBackground, "voice"),
            CIcon(CIcons::NetworkCapabilityVoiceReceiveOnly, "voice receive"),
            CIcon(CIcons::NetworkCapabilityVoiceReceiveOnlyBackground, "voice receive"),
            CIcon(CIcons::NetworkRoleApproach, "ATC approach"),
            CIcon(CIcons::NetworkRoleC1, "C1"),
            CIcon(CIcons::NetworkRoleC3, "C3"),
            CIcon(CIcons::NetworkRoleCenter, "ATC center"),
            CIcon(CIcons::NetworkRoleDelivery, "ATC delivery"),
            CIcon(CIcons::NetworkRoleDeparture, "ATC departure"),
            CIcon(CIcons::NetworkRoleFss, "FSS"),
            CIcon(CIcons::NetworkRoleGround, "ATC ground"),
            CIcon(CIcons::NetworkRoleI1, "I1 (instructor)"),
            CIcon(CIcons::NetworkRoleI3, "I3 (instructor)"),
            CIcon(CIcons::NetworkRoleMnt, "Mentor"),
            CIcon(CIcons::NetworkRoleObs, "observer"),
            CIcon(CIcons::NetworkRolePilot, "pilot"),
            CIcon(CIcons::NetworkRoleS1, "S1"),
            CIcon(CIcons::NetworkRoleS2, "S2"),
            CIcon(CIcons::NetworkRoleS3, "S3"),
            CIcon(CIcons::NetworkRoleSup, "supervisor"),
            CIcon(CIcons::NetworkRoleTower, "ATC tower"),
            CIcon(CIcons::NetworkRoleUnknown, "unknown"),
            CIcon(CIcons::NetworkVatsimLogo, "VATSIM"),
            CIcon(CIcons::NetworkVatsimLogoWhite, "VATSIM"),
            CIcon(CIcons::NotSet, "?"),
            CIcon(CIcons::OSAll, "All OS"),
            CIcon(CIcons::OSLinux, "Linux"),
            CIcon(CIcons::OSMacOs, "Mac OS"),
            CIcon(CIcons::OSWindows, "Windows"),
            CIcon(CIcons::SimulatorFG, "FG"),
            CIcon(CIcons::SimulatorFG16, "FG"),
            CIcon(CIcons::SimulatorFS9, "FS9"),
            CIcon(CIcons::SimulatorFS916, "FS9"),
            CIcon(CIcons::SimulatorFSX, "FSX"),
            CIcon(CIcons::SimulatorFSX16, "FSX"),
            CIcon(CIcons::SimulatorP3D, "P3D"),
            CIcon(CIcons::SimulatorP3D16, "P3D"),
            CIcon(CIcons::SimulatorXPlane, "XPlane"),
            CIcon(CIcons::SimulatorXPlane16, "XPlane"),
            CIcon(CIcons::StandardIconAppAircraft16, "aircraft"),
            CIcon(CIcons::StandardIconAppAtc16, "ATC"),
            CIcon(CIcons::StandardIconAppFlightPlan16, "flight plan"),
            CIcon(CIcons::StandardIconAppLog16, "log"),
            CIcon(CIcons::StandardIconAppMappings16, "mappings"),
            CIcon(CIcons::StandardIconAppSettings16, "settings"),
            CIcon(CIcons::StandardIconAppSimulator16, "simulator"),
            CIcon(CIcons::StandardIconAppTextMessages16, "text messages"),
            CIcon(CIcons::StandardIconAppUsers16, "users"),
            CIcon(CIcons::StandardIconAppWeather16, "weather"),
            CIcon(CIcons::StandardIconArrowMediumEast16, "arrow east"),
            CIcon(CIcons::StandardIconArrowMediumNorth16, "arrow north"),
            CIcon(CIcons::StandardIconArrowMediumSouth16, "arrow south"),
            CIcon(CIcons::StandardIconArrowMediumWest16, "arrow west"),
            CIcon(CIcons::StandardIconClose16, "close"),
            CIcon(CIcons::StandardIconColor16, "color"),
            CIcon(CIcons::StandardIconColorPicker16, "color picker"),
            CIcon(CIcons::StandardIconColorSwatch16, "color swatch"),
            CIcon(CIcons::StandardIconCopy16, "copy"),
            CIcon(CIcons::StandardIconCross16, "cross"),
            CIcon(CIcons::StandardIconCrossCircle16, "cross circle"),
            CIcon(CIcons::StandardIconCrossSmall16, "cross (small)"),
            CIcon(CIcons::StandardIconCrossWhite16, "cross white"),
            CIcon(CIcons::StandardIconCut16, "cut"),
            CIcon(CIcons::StandardIconDatabase16, "database"),
            CIcon(CIcons::StandardIconDatabaseAdd16, "add to database"),
            CIcon(CIcons::StandardIconDatabaseConnect16, "connect database"),
            CIcon(CIcons::StandardIconDatabaseDelete16, "delete from database"),
            CIcon(CIcons::StandardIconDatabaseEdit16, "edit database"),
            CIcon(CIcons::StandardIconDatabaseError16, "error"),
            CIcon(CIcons::StandardIconDatabaseKey16, "key"),
            CIcon(CIcons::StandardIconDatabaseTable16, "table"),
            CIcon(CIcons::StandardIconDelete16, "delete"),
            CIcon(CIcons::StandardIconDockBottom16, "dock bottom"),
            CIcon(CIcons::StandardIconDockTop16, "dock top"),
            CIcon(CIcons::StandardIconDragAndDrop16, "drag and drop"),
            CIcon(CIcons::StandardIconEmpty, "empty"),
            CIcon(CIcons::StandardIconEmpty16, "empty"),
            CIcon(CIcons::StandardIconError16, "error"),
            CIcon(CIcons::StandardIconFilter16, "filter"),
            CIcon(CIcons::StandardIconFloatAll16, "float all"),
            CIcon(CIcons::StandardIconFloatOne16, "float one"),
            CIcon(CIcons::StandardIconFont16, "font"),
            CIcon(CIcons::StandardIconGlobe16, "globe"),
            CIcon(CIcons::StandardIconHeadingOne16, "heading"),
            CIcon(CIcons::StandardIconInfo16, "info"),
            CIcon(CIcons::StandardIconJoystick16, "joystick"),
            CIcon(CIcons::StandardIconLoad16, "load open"),
            CIcon(CIcons::StandardIconLockClosed16, "lock"),
            CIcon(CIcons::StandardIconLockOpen16, "lock open"),
            CIcon(CIcons::StandardIconMonitorError16, "monitor error"),
            CIcon(CIcons::StandardIconPaintCan16, "paint can"),
            CIcon(CIcons::StandardIconPaperPlane16, "plane"),
            CIcon(CIcons::StandardIconPaste16, "paste"),
            CIcon(CIcons::StandardIconPlugin16, "plugin"),
            CIcon(CIcons::StandardIconRadar16, "radar"),
            CIcon(CIcons::StandardIconRadio16, "radio"),
            CIcon(CIcons::StandardIconRefresh16, "refresh"),
            CIcon(CIcons::StandardIconResize16, "resize"),
            CIcon(CIcons::StandardIconResizeHorizontal16, "resize (horizontal)"),
            CIcon(CIcons::StandardIconResizeVertical16, "resize (vertical)"),
            CIcon(CIcons::StandardIconSave16, "save"),
            CIcon(CIcons::StandardIconSpeakerNetwork16, "speaker (network)"),
            CIcon(CIcons::StandardIconStatusBar16, "status bar"),
            CIcon(CIcons::StandardIconTableRelationship16, "relationship"),
            CIcon(CIcons::StandardIconTableSheet16, "table sheet"),
            CIcon(CIcons::StandardIconText16, "text"),
            CIcon(CIcons::StandardIconTick16, "tick"),
            CIcon(CIcons::StandardIconTickRed16, "tick red"),
            CIcon(CIcons::StandardIconTickSmall16, "tick (small)"),
            CIcon(CIcons::StandardIconTickWhite16, "tick white"),
            CIcon(CIcons::StandardIconUnknown16, "unknwon"),
            CIcon(CIcons::StandardIconUser16, "user"),
            CIcon(CIcons::StandardIconUsers16, "users"),
            CIcon(CIcons::StandardIconViewMulticolumn16, "view multicolumn"),
            CIcon(CIcons::StandardIconViewTile16, "view tile"),
            CIcon(CIcons::StandardIconVolumeHigh16, "volume hight"),
            CIcon(CIcons::StandardIconVolumeLow16, "volume low"),
            CIcon(CIcons::StandardIconVolumeMuted16, "muted"),
            CIcon(CIcons::StandardIconWarning16, "warning"),
            CIcon(CIcons::StandardIconWeatherCloudy16, "cloudy"),
            CIcon(CIcons::StandardIconWrench16, "wrench"),
            CIcon(CIcons::StandardIconBuilding16, "building"),
            CIcon(CIcons::Swift16, "swift"),
            CIcon(CIcons::Swift24, "swift"),
            CIcon(CIcons::Swift48, "swift"),
            CIcon(CIcons::Swift64, "swift"),
            CIcon(CIcons::Swift128, "swift"),
            CIcon(CIcons::Swift256, "swift"),
            CIcon(CIcons::Swift1024, "swift"),
            CIcon(CIcons::SwiftDatabase16, "swift DB"),
            CIcon(CIcons::SwiftDatabase24, "swift DB"),
            CIcon(CIcons::SwiftDatabase48, "swift DB"),
            CIcon(CIcons::SwiftDatabase64, "swift DB"),
            CIcon(CIcons::SwiftDatabase128, "swift DB"),
            CIcon(CIcons::SwiftDatabase256, "swift DB"),
            CIcon(CIcons::SwiftDatabase1024, "swift DB"),
            CIcon(CIcons::SwiftCore16, "swift core"),
            CIcon(CIcons::SwiftCore24, "swift core"),
            CIcon(CIcons::SwiftCore48, "swift core"),
            CIcon(CIcons::SwiftCore64, "swift core"),
            CIcon(CIcons::SwiftCore128, "swift core"),
            CIcon(CIcons::SwiftCore256, "swift core"),
            CIcon(CIcons::SwiftCore512, "swift core"),
            CIcon(CIcons::SwiftCore1024, "swift core"),
            CIcon(CIcons::SwiftLauncher16, "swift launcher"),
            CIcon(CIcons::SwiftLauncher24, "swift launcher"),
            CIcon(CIcons::SwiftLauncher48, "swift launcher"),
            CIcon(CIcons::SwiftLauncher64, "swift launcher"),
            CIcon(CIcons::SwiftLauncher128, "swift launcher"),
            CIcon(CIcons::SwiftLauncher256, "swift launcher"),
            CIcon(CIcons::SwiftLauncher512, "swift launcher"),
            CIcon(CIcons::SwiftLauncher1024, "swift launcher")
        });
        return icons;
    }
} // ns
