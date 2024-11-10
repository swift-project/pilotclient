// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_ICONS_H
#define SWIFT_MISC_ICONS_H

#include "misc/swiftmiscexport.h"

#include <QIcon>
#include <QImage>
#include <QMap>
#include <QMetaType>
#include <QPixmap>
#include <QString>
#include <Qt>

class QSize;

namespace swift::misc
{
    //! Standard icons
    class SWIFT_MISC_EXPORT CIcons
    {
    public:
        //! Constructor, use class static only
        CIcons() = delete;

        //! Index for each icon, allows to send them via DBus, efficiently store them, etc.
        enum IconIndex
        {
            // !! keep indexes alphabetically sorted and in sync with CIconList
            ApplicationAircraft = 0,
            ApplicationAircraftCategories,
            ApplicationAircraftIcao,
            ApplicationAirlineIcao,
            ApplicationAtc,
            ApplicationAudio,
            ApplicationCockpit,
            ApplicationCountries,
            ApplicationDatabase,
            ApplicationDbStash,
            ApplicationDistributors,
            ApplicationFlightPlan,
            ApplicationLiveries,
            ApplicationLog,
            ApplicationMappings,
            ApplicationModels,
            ApplicationSettings,
            ApplicationSimulator,
            ApplicationTerminal,
            ApplicationTextMessages,
            ApplicationWeather,
            ApplicationRadar,
            AviationAtis,
            AviationAttitudeIndicator,
            AviationMetar,
            GeoPosition,
            ModelExclude,
            ModelInclude,
            NetworkCapabilityTextOnly,
            NetworkCapabilityUnknown,
            NetworkCapabilityVoice,
            NetworkCapabilityVoiceBackground,
            NetworkCapabilityVoiceReceiveOnly,
            NetworkCapabilityVoiceReceiveOnlyBackground,
            NetworkRoleApproach,
            NetworkRoleC1,
            NetworkRoleC3,
            NetworkRoleCenter,
            NetworkRoleDelivery,
            NetworkRoleDeparture,
            NetworkRoleFss,
            NetworkRoleGround,
            NetworkRoleI1,
            NetworkRoleI3,
            NetworkRoleMnt, //!< Mentor
            NetworkRoleObs,
            NetworkRolePilot,
            NetworkRoleS1,
            NetworkRoleS2,
            NetworkRoleS3,
            NetworkRoleSup,
            NetworkRoleTower,
            NetworkRoleUnknown,
            NotSet,
            OSAll,
            OSLinux,
            OSMacOs,
            OSWindows,
            StandardIconAppAircraft16,
            StandardIconAppAtc16,
            StandardIconAppFlightPlan16,
            StandardIconAppLog16,
            StandardIconAppMappings16,
            StandardIconAppSettings16,
            StandardIconAppSimulator16,
            StandardIconAppTextMessages16,
            StandardIconAppUsers16,
            StandardIconAppWeather16,
            StandardIconArrowMediumEast16,
            StandardIconArrowMediumNorth16,
            StandardIconArrowMediumSouth16,
            StandardIconArrowMediumWest16,
            StandardIconClose16,
            StandardIconColor16,
            StandardIconColorPicker16,
            StandardIconColorSwatch16,
            StandardIconCopy16,
            StandardIconCross16,
            StandardIconCrossCircle16,
            StandardIconCrossSmall16,
            StandardIconCrossWhite16,
            StandardIconCut16,
            StandardIconDatabase16,
            StandardIconDatabaseAdd16,
            StandardIconDatabaseConnect16,
            StandardIconDatabaseDelete16,
            StandardIconDatabaseEdit16,
            StandardIconDatabaseError16,
            StandardIconDatabaseKey16,
            StandardIconDatabaseTable16,
            StandardIconDelete16,
            StandardIconDockBottom16,
            StandardIconDockTop16,
            StandardIconDragAndDrop16,
            StandardIconEmpty,
            StandardIconEmpty16,
            StandardIconError16,
            StandardIconFilter16,
            StandardIconFloatAll16,
            StandardIconFloatOne16,
            StandardIconFont16,
            StandardIconGlobe16,
            StandardIconHeadingOne16,
            StandardIconInfo16,
            StandardIconJoystick16,
            StandardIconLoad16,
            StandardIconLockClosed16,
            StandardIconLockOpen16,
            StandardIconMonitorError16,
            StandardIconPaintCan16,
            StandardIconPaperPlane16,
            StandardIconPaste16,
            StandardIconPlugin16,
            StandardIconRadar16,
            StandardIconRadio16,
            StandardIconRefresh16,
            StandardIconResize16,
            StandardIconResizeHorizontal16,
            StandardIconResizeVertical16,
            StandardIconSave16,
            StandardIconSpeakerNetwork16,
            StandardIconStatusBar16,
            StandardIconTableRelationship16,
            StandardIconTableSheet16,
            StandardIconText16,
            StandardIconTick16,
            StandardIconTickRed16,
            StandardIconTickSmall16,
            StandardIconTickWhite16,
            StandardIconUnknown16,
            StandardIconUser16,
            StandardIconUsers16,
            StandardIconViewMulticolumn16,
            StandardIconViewTile16,
            StandardIconVolumeHigh16,
            StandardIconVolumeLow16,
            StandardIconVolumeMuted16,
            StandardIconWarning16,
            StandardIconWeatherCloudy16,
            StandardIconWrench16,
            StandardIconZoomIn16,
            StandardIconZoomOut16,
            StandardIconBuilding16,
            Swift16,
            Swift24,
            Swift48,
            Swift64,
            Swift128,
            Swift256,
            Swift1024,
            SwiftDatabase16,
            SwiftDatabase24,
            SwiftDatabase48,
            SwiftDatabase64,
            SwiftDatabase128,
            SwiftDatabase256,
            SwiftDatabase1024,
            SwiftCore16,
            SwiftCore24,
            SwiftCore48,
            SwiftCore64,
            SwiftCore128,
            SwiftCore256,
            SwiftCore512,
            SwiftCore1024,
            SwiftLauncher16,
            SwiftLauncher24,
            SwiftLauncher48,
            SwiftLauncher64,
            SwiftLauncher128,
            SwiftLauncher256,
            SwiftLauncher512,
            SwiftLauncher1024,

            // ---------------------------
            // Icon is not from resources, but generated at runtime
            // ---------------------------
            IconIsGenerated,

            // ---------------------------
            // Icon is not from resources, but a file
            // ---------------------------
            IconIsFile
        };

        // -------------------------------------------------------------
        // Standard pixmaps
        // -------------------------------------------------------------

        //! Info
        static const QPixmap &info16();

        //! Warning
        static const QPixmap &warning16();

        //! Error
        static const QPixmap &error16();

        //! Close
        static const QPixmap &close16();

        //! Disk
        static const QPixmap &disk16();

        //! Load
        static const QPixmap &load16();

        //! Save
        static const QPixmap &save16();

        //! Color
        static const QPixmap &color16();

        //! Color picker
        static const QPixmap &colorPicker16();

        //! Color swatch
        static const QPixmap &colorSwatch16();

        //! Resize
        static const QPixmap &resize16();

        //! Resize horizontally
        static const QPixmap &resizeHorizontal16();

        //! Resize vertically
        static const QPixmap &resizeVertical16();

        //! Refresh
        static const QPixmap &refresh16();

        //! Text
        static const QPixmap &text16();

        //! Globe
        static const QPixmap &globe16();

        //! Heading 1
        static const QPixmap &headingOne16();

        //! User
        static const QPixmap &user16();

        //! Users
        static const QPixmap &users16();

        //! View multicolumn
        static const QPixmap &viewMultiColumn();

        //! View tile
        static const QPixmap &viewTile();

        //! Tick
        static const QPixmap &tick16();

        //! Tick
        static const QPixmap &tickRed16();

        //! Tick
        static const QPixmap &tickWhite16();

        //! Tick
        static const QPixmap &tickSmall16();

        //! Cross
        static const QPixmap &cross16();

        //! Cross
        static const QPixmap &crossWhite16();

        //! Cross
        static const QPixmap &crossCircle16();

        //! Cross
        static const QPixmap &crossSmall16();

        //! Delete
        static const QPixmap &delete16();

        //! Radio
        static const QPixmap &radio16();

        //! Speaker (network)
        static const QPixmap &speakerNetwork16();

        //! Joystick
        static const QPixmap &joystick16();

        //! Lock
        static const QPixmap &lockOpen16();

        //! Lock
        static const QPixmap &lockClosed16();

        //! Paint can
        static const QPixmap &paintCan16();

        //! Plugin
        static const QPixmap &plugin16();

        //! Docking
        static const QPixmap &dockTop16();

        //! Docking
        static const QPixmap &dockBottom16();

        //! Drag and drop
        static const QPixmap &dragAndDrop16();

        //! Float all
        static const QPixmap &floatAll16();

        //! Float all
        static const QPixmap &floatOne16();

        //! Sound volume high
        static const QPixmap &volumeHigh16();

        //! Sound volume low
        static const QPixmap &volumeLow16();

        //! Sound volume muted
        static const QPixmap &volumeMuted16();

        //! Wrench
        static const QPixmap &wrench16();

        //! Radar
        static const QPixmap &radar16();

        //! Radar
        static const QPixmap &tableRelationship16();

        //! Paper plane
        static const QPixmap &paperPlane16();

        //! Table sheet
        static const QPixmap &tableSheet16();

        //! Clouds
        static const QPixmap &weatherCloudy16();

        //! Monitor error
        static const QPixmap &monitorError16();

        //! Status bar
        static const QPixmap &statusBar16();

        //! Unknown
        static const QPixmap &unknown16();

        //! Empty icon
        static const QPixmap &empty();

        //! Empty icon
        static const QPixmap &empty16();

        //! Filter
        static const QPixmap &filter16();

        //! Font
        static const QPixmap &font16();

        //! Folder
        static const QPixmap &folder16();

        //! Folder edit
        static const QPixmap &folderEdit16();

        //! Arrow
        static const QPixmap &arrowMediumNorth16();

        //! Arrow
        static const QPixmap &arrowMediumSouth16();

        //! Arrow
        static const QPixmap &arrowMediumEast16();

        //! Arrow
        static const QPixmap &arrowMediumWest16();

        //! Copy
        static const QPixmap &copy16();

        //! Paste
        static const QPixmap &paste16();

        //! Cut
        static const QPixmap &cut16();

        //! Zoom in
        static const QPixmap &zoomIn16();

        //! Zoom out
        static const QPixmap &zoomOut16();

        //! Building
        static const QPixmap &building16();

        // --------------------------------------------------
        // -- Database
        // --------------------------------------------------

        //! Database
        static const QPixmap &database16();

        //! Database add
        static const QPixmap &databaseAdd16();

        //! Database connect
        static const QPixmap &databaseConnect16();

        //! Database delete
        static const QPixmap &databaseDelete16();

        //! Database edit
        static const QPixmap &databaseEdit16();

        //! Database error
        static const QPixmap &databaseError16();

        //! Database key
        static const QPixmap &databaseKey16();

        //! Database table
        static const QPixmap &databaseTable16();

        // --------------------------------------------------
        // -- OS icons
        // --------------------------------------------------

        //! All OS
        static const QPixmap &osAll();

        //! Linux
        static const QPixmap &osLinux();

        //! MacOS
        static const QPixmap &osMac();

        //! Windows
        static const QPixmap &osWindows();

        // --------------------------------------------------
        // -- Preloader icons
        // --------------------------------------------------

        //! Preloader
        static const QPixmap &preloader64();

        //! Preloader
        static const QPixmap &preloader32();

        //! Preloader
        static const QPixmap &preloader16();

        // --------------------------------------------------
        // -- swift
        // --------------------------------------------------

        //! @{
        //! swift icon
        static const QPixmap &swift16();
        static const QPixmap &swift24();
        static const QPixmap &swift48();
        static const QPixmap &swift64();
        static const QPixmap &swift128();
        static const QPixmap &swift256();
        static const QPixmap &swift1024();
        static const QPixmap &swiftDatabase16();
        static const QPixmap &swiftDatabase24();
        static const QPixmap &swiftDatabase48();
        static const QPixmap &swiftDatabase64();
        static const QPixmap &swiftDatabase128();
        static const QPixmap &swiftDatabase256();
        static const QPixmap &swiftDatabase1024();
        static const QPixmap &swiftCore16();
        static const QPixmap &swiftCore24();
        static const QPixmap &swiftCore48();
        static const QPixmap &swiftCore64();
        static const QPixmap &swiftCore128();
        static const QPixmap &swiftCore256();
        static const QPixmap &swiftCore1024();
        static const QPixmap &swiftLauncher16();
        static const QPixmap &swiftLauncher24();
        static const QPixmap &swiftLauncher48();
        static const QPixmap &swiftLauncher64();
        static const QPixmap &swiftLauncher128();
        static const QPixmap &swiftLauncher256();
        static const QPixmap &swiftLauncher1024();
        static const QPixmap &swiftMap16();
        static const QPixmap &swiftMap24();
        static const QPixmap &swiftMap48();
        static const QPixmap &swiftMap64();
        static const QPixmap &swiftMap128();
        static const QPixmap &swiftMap256();
        static const QPixmap &swiftMap1024();

        //! @}

        // --------------------------------------------------
        // -- application icons
        // --------------------------------------------------

        //! Application weather
        static const QPixmap &appWeather16();

        //! Application settings
        static const QPixmap &appSettings16();

        //! Application users
        static const QPixmap &appUsers16();

        //! Application flight plan
        static const QPixmap &appFlightPlan16();

        //! Application cockpit
        static const QPixmap &appCockpit16();

        //! Application simulator
        static const QPixmap &appSimulator16();

        //! Terminal application
        static const QPixmap &appTerminal16();

        //! Application text messages
        static const QPixmap &appTextMessages16();

        //! Application ATC
        static const QPixmap &appAtc16();

        //! Application aircrafts
        static const QPixmap &appAircraft16();

        //! Application mappings
        static const QPixmap &appMappings16();

        //! Application interpolation
        static const QPixmap &appInterpolation16();

        //! Application log/status messages
        static const QPixmap &appLog16();

        //! Application audio
        static const QPixmap &appAudio16();

        //! Voice rooms
        static const QPixmap &appVoiceRooms16();

        //! App.database
        static const QPixmap &appDatabase16();

        //! Aircraft ICAO
        static const QPixmap &appAircraftIcao16();

        //! Airline ICAO
        static const QPixmap &appAirlineIcao16();

        //! Liveries
        static const QPixmap &appLiveries16();

        //! Models
        static const QPixmap &appModels16();

        //! Countries
        static const QPixmap &appCountries16();

        //! Distributors
        static const QPixmap &appDistributors16();

        //! Categories
        static const QPixmap &appAircraftCategories16();

        //! Stash
        static const QPixmap &appDbStash16();

        //! Radar
        static const QPixmap &appRadar16();

        // -------------------------------------------------------------
        // Network and aviation
        // -------------------------------------------------------------

        //! C1
        static const QPixmap &roleC1();

        //! C2
        static const QPixmap &roleC2();

        //! C3
        static const QPixmap &roleC3();

        //! S1
        static const QPixmap &roleS1();

        //! S2
        static const QPixmap &roleS2();

        //! S3
        static const QPixmap &roleS3();

        //! I1
        static const QPixmap &roleI1();

        //! I3
        static const QPixmap &roleI3();

        //! MNT
        static const QPixmap &roleMnt();

        //! OBS
        static const QPixmap &roleObs();

        //! SUP
        static const QPixmap &roleSup();

        //! Pilot
        static const QPixmap &rolePilot();

        //! Approach
        static const QPixmap &roleApproach();

        //! Ground
        static const QPixmap &roleGround();

        //! Delivery
        static const QPixmap &roleDelivery();

        //! Departure
        static const QPixmap &roleDeparture();

        //! FSS (flight service staion)
        static const QPixmap &roleFss();

        //! Tower
        static const QPixmap &roleTower();

        //! Center
        static const QPixmap &roleCenter();

        //! Unknown
        static const QPixmap &roleUnknown();

        //! Full voice capability
        static const QPixmap &capabilityVoice();

        //! Full voice capability
        static const QPixmap &capabilityVoiceBackground();

        //! Voice receive only
        static const QPixmap &capabilityVoiceReceiveOnly();

        //! Voice receive only
        static const QPixmap &capabilityVoiceReceiveOnlyBackground();

        //! Text only
        static const QPixmap &capabilityTextOnly();

        //! Text only
        static const QPixmap &capabilityUnknown();

        //! Attitude indicator
        static const QPixmap &attitudeIndicator16();

        //! METAR
        static const QPixmap &metar();

        //! ATIS
        static const QPixmap &atis();

        //! Geo position
        static const QPixmap &geoPosition16();

        // -------------------------------------------------------------
        // Mapping
        // -------------------------------------------------------------

        //! Model include
        static const QPixmap &modelInclude16();

        //! Model exclude
        static const QPixmap &modelExclude16();

        //! Model Converter X
        static const QPixmap &modelConverterX();

        // -------------------------------------------------------------
        // By index
        // -------------------------------------------------------------

        //! Pixmap by given index
        static const QPixmap &pixmapByIndex(IconIndex index);

        //! Pixmap by given index rotated
        static QPixmap pixmapByIndex(IconIndex index, int rotateDegrees);

        // -------------------------------------------------------------
        // By file from swift resource directory
        // -------------------------------------------------------------

        //! Pixmap by given index
        static const QPixmap &pixmapByResourceFileName(const QString &relativeFileName, QString &fullFilePath);

        // -------------------------------------------------------------
        // Utility functions
        // -------------------------------------------------------------

        //! Change color of resource
        static QPixmap changeResourceBackgroundColor(const QString &resource, Qt::GlobalColor backgroundColor);

        //! Change color of icon
        static QIcon changeIconBackgroundColor(const QIcon &icon, Qt::GlobalColor backgroundColor, QSize targetsize);

        //! Change image background color
        static QImage changeImageBackgroundColor(const QImage &imgSource, Qt::GlobalColor backgroundColor);

        //! Rotate by pixmap
        static QPixmap rotate(int rotateDegrees, const QPixmap &original);

        // -------------------------------------------------------------
        // Convert enum to QVariant containing CIcon or QPixmap
        // (function definitions in icon.cpp)
        // -------------------------------------------------------------

        //! Variant containing CIcon
        static QVariant toVariant(IconIndex icon);

        //! Variant containing QPixmap
        static QVariant toVariantPixmap(IconIndex icon);

    private:
        //! File cache for the loaded files
        static QMap<QString, QPixmap> &getResourceFileCache();
    };
}

Q_DECLARE_METATYPE(swift::misc::CIcons::IconIndex)

#endif // guard
