/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "iconlist.h"

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
        qRegisterMetaType<BlackMisc::CCollection<CIcon>>();
        qDBusRegisterMetaType<BlackMisc::CCollection<CIcon>>();
        qRegisterMetaType<CIconList>();
        qDBusRegisterMetaType<CIconList>();
        registerMetaValueType<CIconList>();
    }

    const CIconList &CIconList::allIcons()
    {
        static const CIconList icons(
        {
            // keep indexes alphabetically sorted and in sync with CIcons
            CIcon(CIcons::ApplicationIconAircrafts, "aircrafts"),
            CIcon(CIcons::ApplicationIconAtc, "ATC"),
            CIcon(CIcons::ApplicationIconAudio, "audio"),
            CIcon(CIcons::ApplicationIconCockpit, "cockpit"),
            CIcon(CIcons::ApplicationIconFlightPlan, "flightplan"),
            CIcon(CIcons::ApplicationIconLog, "log"),
            CIcon(CIcons::ApplicationIconMappings, "mappings"),
            CIcon(CIcons::ApplicationIconSettings, "settings"),
            CIcon(CIcons::ApplicationIconSimulator, "simulator"),
            CIcon(CIcons::ApplicationIconTextMessages, "text messages"),
            CIcon(CIcons::ApplicationIconWeather, "weather"),
            CIcon(CIcons::AviationAtis, "ATIS"),
            CIcon(CIcons::AviationAttitudeIndicator, "attitude indicator"),
            CIcon(CIcons::AviationMetar, "METAR"),
            CIcon(CIcons::GeoPosition, "geo position"),
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
            CIcon(CIcons::NotSet, "?"),
            CIcon(CIcons::StandardIconAppAircrafts16, "aircrafts"),
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
            CIcon(CIcons::StandardIconCross16, "cross"),
            CIcon(CIcons::StandardIconCrossCircle16, "cross circle"),
            CIcon(CIcons::StandardIconCrossSmall16, "cross (small)"),
            CIcon(CIcons::StandardIconCrossWhite16, "cross white"),
            CIcon(CIcons::StandardIconDelete16, "delete"),
            CIcon(CIcons::StandardIconDockBottom16, "dock bottom"),
            CIcon(CIcons::StandardIconDockTop16, "dock top"),
            CIcon(CIcons::StandardIconEmpty, "empty"),
            CIcon(CIcons::StandardIconEmpty16, "empty"),
            CIcon(CIcons::StandardIconError16, "error"),
            CIcon(CIcons::StandardIconFloatAll16, "float all"),
            CIcon(CIcons::StandardIconFloatOne16, "floast one"),
            CIcon(CIcons::StandardIconGlobe16, "globe"),
            CIcon(CIcons::StandardIconHeadingOne16, "heading"),
            CIcon(CIcons::StandardIconInfo16, "info"),
            CIcon(CIcons::StandardIconJoystick16, "joystick"),
            CIcon(CIcons::StandardIconLockClosed16, "lock"),
            CIcon(CIcons::StandardIconLockOpen16, "lock open"),
            CIcon(CIcons::StandardIconMonitorError16, "monitor error"),
            CIcon(CIcons::StandardIconPaperPlane16, "plane"),
            CIcon(CIcons::StandardIconPlugin16, "plugin"),
            CIcon(CIcons::StandardIconRadar16, "radar"),
            CIcon(CIcons::StandardIconRadio16, "radio"),
            CIcon(CIcons::StandardIconRefresh16, "refresh"),
            CIcon(CIcons::StandardIconResize16, "resize"),
            CIcon(CIcons::StandardIconResizeHorizontal16, "resize (horizontal)"),
            CIcon(CIcons::StandardIconResizeVertical16, "resize (vertical)"),
            CIcon(CIcons::StandardIconSpeakerNetwork16, "speaker (network)"),
            CIcon(CIcons::StandardIconStatusBar16, "status bar"),
            CIcon(CIcons::StandardIconSwift24, "swift"),
            CIcon(CIcons::StandardIconSwift48, "swift"),
            CIcon(CIcons::StandardIconSwiftNova24, "swift"),
            CIcon(CIcons::StandardIconSwiftNova48, "swift"),
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
            CIcon(CIcons::StandardIconWrench16, "wrench")
        });
        return icons;
    }

    const CIcon &CIconList::iconByIndex(CIcons::IconIndex index)
    {
        return iconByIndex(static_cast<int>(index));
    }

    const CIcon &CIconList::iconByIndex(int index)
    {
        // changed to index / at based approach during #322 (after Sleepy profiling)
        // this seems to be faster as the findBy approach previously used, but required synced indexes
        Q_ASSERT_X(index >= 0 && index < allIcons().size(), "iconForIndex", "wrong index");
        return allIcons()[index];
    }
}
