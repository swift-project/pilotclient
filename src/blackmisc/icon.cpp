// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/icon.h"
#include "blackmisc/sequence.h"

#include <QStringBuilder>
#include <QtGlobal>
#include <QDir>

namespace BlackMisc
{
    CIcon::CIcon(CIcons::IconIndex index) : CIcon(iconByIndex(index)) {}

    CIcon::CIcon(CIcons::IconIndex index, const QString &descriptiveText) : m_index(index), m_descriptiveText(descriptiveText) {}

    // CIcon::CIcon(const QPixmap &pixmap, const QString &descriptiveText) :
    //     m_index(CIcons::IconIsGenerated), m_descriptiveText(descriptiveText), m_pixmap(pixmap)
    //{ }

    // CIcon::CIcon(const QString &resourceFilePath, const QString &descriptiveText) :
    //     m_index(CIcons::IconIsFile), m_descriptiveText(descriptiveText)
    //{
    //     QString fullPath;
    //     m_pixmap = CIcons::pixmapByResourceFileName(QDir::cleanPath(resourceFilePath), fullPath);
    //     m_fileResourcePath = fullPath;
    // }

    CIcons::IconIndex CIcon::getIndex() const
    {
        return m_index;
    }

    bool CIcon::isIndexBased() const
    {
        return m_index < CIcons::IconIsGenerated;
    }

    bool CIcon::isGenerated() const
    {
        return this->getIndex() == CIcons::IconIsGenerated;
    }

    bool CIcon::isFileBased() const
    {
        return this->getIndex() == CIcons::IconIsFile;
    }

    bool CIcon::isSet() const
    {
        return (m_index != CIcons::NotSet);
    }

    QPixmap CIcon::toPixmap() const
    {
        if (this->isSet())
        {
            if (this->isGenerated()) { return m_pixmap; }
            if (this->isFileBased()) { return m_pixmap; }
            return CIcons::pixmapByIndex(this->getIndex(), m_rotateDegrees);
        }
        else
        {
            return CIcons::pixmapByIndex(this->getIndex());
        }
    }

    QIcon CIcon::toQIcon() const
    {
        return QIcon(toPixmap());
    }

    QString CIcon::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        const QString s = m_descriptiveText % u' ' % QString::number(m_index);
        return s;
    }

    const CIcon &CIcon::iconByIndex(CIcons::IconIndex index)
    {
        return iconByIndex(static_cast<int>(index));
    }

    const CIcon &CIcon::iconByIndex(int index)
    {
        // changed to index / at based approach during #322 (after Sleepy profiling)
        // this seems to be faster as the findBy approach previously used, but required synced indexes
        Q_ASSERT_X(index >= 0 && index < CIcon::allIcons().size(), "iconForIndex", "wrong index");
        return CIcon::allIcons()[index];
    }

    const CSequence<CIcon> &CIcon::allIcons()
    {
        // keep indexes alphabetically sorted and in sync with CIcons
        // req. for "iconByIndex"
        static const CSequence<CIcon> icons(
            { CIcon(CIcons::ApplicationAircraft, "aircraft"),
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
              CIcon(CIcons::NotSet, "?"),
              CIcon(CIcons::OSAll, "All OS"),
              CIcon(CIcons::OSLinux, "Linux"),
              CIcon(CIcons::OSMacOs, "Mac OS"),
              CIcon(CIcons::OSWindows, "Windows"),
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
              CIcon(CIcons::StandardIconZoomIn16, "zoom in"),
              CIcon(CIcons::StandardIconZoomOut16, "zoom out"),
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
              CIcon(CIcons::SwiftLauncher1024, "swift launcher") });
        return icons;
    }

    // CIcons methods defined here to circumvent cyclic dependency
    QVariant CIcons::toVariant(IconIndex icon)
    {
        return QVariant::fromValue(CIcon(icon));
    }
    QVariant CIcons::toVariantPixmap(IconIndex icon)
    {
        return QVariant::fromValue(CIcon(icon).toPixmap());
    }
} // namespace
