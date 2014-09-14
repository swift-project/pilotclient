/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project:
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_ICONS_H
#define BLACKMISC_ICONS_H

#include <QPixmap>

namespace BlackMisc
{
    //! Standard icons
    class CIcons
    {
    public:

        //! Constructor, use class static only
        CIcons() = delete;

        //! Index for each icon, allows to send them via DBus, efficiently store them, etc.
        enum IconIndex
        {
            // keep indexes alphabetically sorted and in sync with CIconList
            AviationAttitudeIndicator,
            AviationAtis,
            AviationMetar,
            GeoPosition,
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
            NetworkRoleGround,
            NetworkRoleI1,
            NetworkRoleI3,
            NetworkRoleMnt,
            NetworkRoleObs,
            NetworkRolePilot,
            NetworkRoleS1,
            NetworkRoleS2,
            NetworkRoleS3,
            NetworkRoleSup,
            NetworkRoleTower,
            NetworkRoleUnknown,
            NotSet,
            StandardIconAppAircrafts16,
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
            StandardIconCross,
            StandardIconCrossCircle,
            StandardIconCrossSmall,
            StandardIconCrossWhite,
            StandardIconDelete16,
            StandardIconDockBottom16,
            StandardIconDockTop16,
            StandardIconEmpty,
            StandardIconEmpty16,
            StandardIconError16,
            StandardIconFloatAll16,
            StandardIconFloatOne16,
            StandardIconGlobe16,
            StandardIconHeadingOne16,
            StandardIconInfo16,
            StandardIconJoystick16,
            StandardIconMonitorError16,
            StandardIconPaperPlane16,
            StandardIconPlugin16,
            StandardIconRadar16,
            StandardIconRefresh16,
            StandardIconResize16,
            StandardIconStatusBar16,
            StandardIconSwift24,
            StandardIconSwift48,
            StandardIconSwiftNova24,
            StandardIconSwiftNova48,
            StandardIconTableRelationship16,
            StandardIconTableSheet16,
            StandardIconText16,
            StandardIconTick,
            StandardIconTickRed,
            StandardIconTickSmall,
            StandardIconTickWhite,
            StandardIconUnknown16,
            StandardIconUser16,
            StandardIconUsers16,
            StandardIconVolumeHigh16,
            StandardIconVolumeLow16,
            StandardIconVolumeMuted16,
            StandardIconWarning16,
            StandardIconWeatherCloudy16,
            StandardIconWrench16
        };

        // -------------------------------------------------------------
        // Standard pixmaps
        // -------------------------------------------------------------

        //! Info
        static const QPixmap &info16()
        {
            static const QPixmap pm(":/pastel/icons/pastel/16/infomation.png");
            return pm;
        }

        //! Warning
        static const QPixmap &warning16()
        {
            static const QPixmap pm(":/pastel/icons/pastel/16/bullet-error.png");
            return pm;
        }

        //! Error
        static const QPixmap &error16()
        {
            static const QPixmap pm(":/pastel/icons/pastel/16/close-red.png");
            return pm;
        }

        //! Close
        static const QPixmap &close16()
        {
            static const QPixmap pm(":/diagona/icons/diagona/icons/cross-button.png");
            return pm;
        }

        //! Resize
        static const QPixmap &resize16()
        {
            static const QPixmap pm(":/diagona/icons/diagona/icons/arrow-resize-135.png");
            return pm;
        }

        //! Resize
        static const QPixmap &refresh16()
        {
            static const QPixmap pm(":/pastel/icons/pastel/16/arrow-refresh.png");
            return pm;
        }

        //! Text
        static const QPixmap &text16()
        {
            static const QPixmap pm(":/pastel/icons/pastel/16/application-view-text.png");
            return pm;
        }

        //! Globe
        static const QPixmap &globe16()
        {
            static const QPixmap pm(":/diagona/icons/diagona/icons/globe.png");
            return pm;
        }

        //! Heading 1
        static const QPixmap &headingOne16()
        {
            static const QPixmap pm(":/pastel/icons/pastel/16/text-heading-1.png");
            return pm;
        }

        //! User
        static const QPixmap &user16()
        {
            static const QPixmap pm(":/pastel/icons/pastel/16/user.png");
            return pm;
        }

        //! Users
        static const QPixmap &users16()
        {
            static const QPixmap pm(":/diagona/icons/diagona/icons/users.png");
            return pm;
        }

        //! Tick
        static const QPixmap &tick16()
        {
            static const QPixmap pm(":/diagona/icons/diagona/icons/tick.png");
            return pm;
        }

        //! Tick
        static const QPixmap &tickRed16()
        {
            static const QPixmap pm(":/diagona/icons/diagona/icons/tick-red.png");
            return pm;
        }

        //! Tick
        static const QPixmap &tickWhite16()
        {
            static const QPixmap pm(":/diagona/icons/diagona/icons/tick-white.png");
            return pm;
        }

        //! Tick
        static const QPixmap &tickSmall16()
        {
            static const QPixmap pm(":/diagona/icons/diagona/icons/tick-small.png");
            return pm;
        }

        //! Cross
        static const QPixmap &cross16()
        {
            static const QPixmap pm(":/diagona/icons/diagona/icons/cross.png");
            return pm;
        }

        //! Cross
        static const QPixmap &crossWhite16()
        {
            static const QPixmap pm(":/diagona/icons/diagona/icons/cross-white.png");
            return pm;
        }

        //! Cross
        static const QPixmap &crossCircle16()
        {
            static const QPixmap pm(":/diagona/icons/diagona/icons/cross-circle.png");
            return pm;
        }

        //! Cross
        static const QPixmap &crossSmall16()
        {
            static const QPixmap pm(":/diagona/icons/diagona/icons/cross-small.png");
            return pm;
        }

        //! Delete
        static const QPixmap &delete16()
        {
            static const QPixmap pm(":/pastel/icons/pastel/16/delete.png");
            return pm;
        }

        //! Joystick
        static const QPixmap &joystick16()
        {
            static const QPixmap pm(":/diagona/icons/diagona/icons/joystick.png");
            return pm;
        }

        //! Plugin
        static const QPixmap &plugin16()
        {
            static const QPixmap pm(":/pastel/icons/pastel/16/plugin.png");
            return pm;
        }

        //! Docking
        static const QPixmap &dockTop16()
        {
            static const QPixmap pm(":/diagona/icons/diagona/icons/application-dock-090.png");
            return pm;
        }

        //! Docking
        static const QPixmap &dockBottom16()
        {
            static const QPixmap pm(":/diagona/icons/diagona/icons/application-dock-270.png");
            return pm;
        }

        //! Float all
        static const QPixmap &floatAll16()
        {
            static const QPixmap pm(":/pastel/icons/pastel/16/application-cascade.png");
            return pm;
        }

        //! Float all
        static const QPixmap &floatOne16()
        {
            static const QPixmap pm(":/pastel/icons/pastel/16/application-double.png");
            return pm;
        }

        //! Sound volume high
        static const QPixmap &volumeHigh16()
        {
            static const QPixmap pm(":/diagona/icons/diagona/icons/speaker-volume.png");
            return pm;
        }

        //! Sound volume low
        static const QPixmap &volumeLow16()
        {
            static const QPixmap pm(":/diagona/icons/diagona/icons/speaker-volume-low.png");
            return pm;
        }

        //! Sound volume muted
        static const QPixmap &volumeMuted16()
        {
            static const QPixmap pm(":/diagona/icons/diagona/icons/speaker-volume-control-mute.png");
            return pm;
        }

        //! Wrench
        static const QPixmap &wrench16()
        {
            static const QPixmap pm(":/pastel/icons/pastel/16/wrench.png");
            return pm;
        }

        //! Radar
        static const QPixmap &radar16()
        {
            static const QPixmap pm(":/diagona/icons/diagona/icons/radar.png");
            return pm;
        }

        //! Radar
        static const QPixmap &tableRelationship16()
        {
            static const QPixmap pm(":/pastel/icons/pastel/16/table-relationship.png");
            return pm;
        }

        //! Paper plane
        static const QPixmap &paperPlane16()
        {
            static const QPixmap pm(":/diagona/icons/diagona/icons/paper-plane.png");
            return pm;
        }

        //! Table sheet
        static const QPixmap &tableSheet16()
        {
            static const QPixmap pm(":/diagona/icons/diagona/icons/table-sheet.png");
            return pm;
        }

        //! Clouds
        static const QPixmap &weatherCloudy16()
        {
            static const QPixmap pm(":/diagona/icons/diagona/icons/weather-cloudy.png");
            return pm;
        }

        //! Monitor error
        static const QPixmap &monitorError16()
        {
            static const QPixmap pm(":/pastel/icons/pastel/16/monitor-error.png");
            return pm;
        }

        //! Status bar
        static const QPixmap &statusBar16()
        {
            static const QPixmap pm(":/pastel/icons/pastel/16/application-statusbar.png");
            return pm;
        }

        //! Unknown
        static const QPixmap &unknown16()
        {
            static const QPixmap pm(":/pastel/icons/pastel/16/help.png");
            return pm;
        }

        //! Empty icon
        static const QPixmap &empty()
        {
            static const QPixmap pm;
            return pm;
        }

        //! Empty icon
        static const QPixmap &empty16()
        {
            static const QPixmap pm(16, 16);
            return pm;
        }

        //! Arrow
        static const QPixmap &arrowMediumNorth16()
        {
            static const QPixmap pm(":/diagona/icons/diagona/icons/arrow-090-medium.png");
            return pm;
        }

        //! Arrow
        static const QPixmap &arrowMediumSouth16()
        {
            static const QPixmap pm(":/diagona/icons/diagona/icons/arrow-270-medium.png");
            return pm;
        }

        //! Arrow
        static const QPixmap &arrowMediumEast16()
        {
            static const QPixmap pm(":/diagona/icons/diagona/icons/arrow-000-medium.png");
            return pm;
        }

        //! Arrow
        static const QPixmap &arrowMediumWest16()
        {
            static const QPixmap pm(":/diagona/icons/diagona/icons/arrow-180-medium.png");
            return pm;
        }

        //! swift icon
        static const QPixmap &swift24()
        {
            static const QPixmap pm(":/own/icons/own/swift/swift24.png");
            return pm;
        }

        //! swift icon
        static const QPixmap &swift48()
        {
            static const QPixmap pm(":/own/icons/own/swift/swift48.png");
            return pm;
        }

        //! swift icon (nova)
        static const QPixmap &swiftNova24()
        {
            static const QPixmap pm(":/own/icons/own/swift/swiftCartoonNova24.png");
            return pm;
        }

        //! swift icon (nova)
        static const QPixmap &swiftNova48()
        {
            static const QPixmap pm(":/own/icons/own/swift/swiftCartoonNova48.png");
            return pm;
        }

        //! Application weather
        static const QPixmap &appWeather16()
        {
            return weatherCloudy16();
        }

        //! Application settings
        static const QPixmap &appSettings16()
        {
            return wrench16();
        }

        //! Application users
        static const QPixmap &appUsers16()
        {
            return users16();
        }

        //! Application flight plan
        static const QPixmap &appFlightPlan16()
        {
            return tableSheet16();
        }

        //! Application simulator
        static const QPixmap &appSimulator16()
        {
            return joystick16();
        }

        //! Application text messages
        static const QPixmap &appTextMessages16()
        {
            return text16();
        }

        //! Application ATC
        static const QPixmap &appAtc16()
        {
            return radar16();
        }

        //! Application aircrafts
        static const QPixmap &appAircrafts16()
        {
            return paperPlane16();
        }

        //! Application mappings
        static const QPixmap &appMappings16()
        {
            return tableRelationship16();
        }

        //! Application log/status messages
        static const QPixmap &appLog16()
        {
            return monitorError16();
        }

        // -------------------------------------------------------------
        // Network and aviation
        // -------------------------------------------------------------

        //! C1
        static const QPixmap &roleC1()
        {
            static const QPixmap pm(":/vatsim/icons/vatsim/C1.png");
            return pm;
        }

        //! C2
        static const QPixmap &roleC2()
        {
            static const QPixmap pm(":/vatsim/icons/vatsim/C2.png");
            return pm;
        }

        //! C3
        static const QPixmap &roleC3()
        {
            static const QPixmap pm(":/vatsim/icons/vatsim/C2.png");
            return pm;
        }

        //! S1
        static const QPixmap &roleS1()
        {
            static const QPixmap pm(":/vatsim/icons/vatsim/S1.png");
            return pm;
        }

        //! S2
        static const QPixmap &roleS2()
        {
            static const QPixmap pm(":/vatsim/icons/vatsim/S2.png");
            return pm;
        }

        //! S3
        static const QPixmap &roleS3()
        {
            static const QPixmap pm(":/vatsim/icons/vatsim/S3.png");
            return pm;
        }

        //! I1
        static const QPixmap &roleI1()
        {
            static const QPixmap pm(":/vatsim/icons/vatsim/I1.png");
            return pm;
        }

        //! I3
        static const QPixmap &roleI3()
        {
            static const QPixmap pm(":/vatsim/icons/vatsim/I3.png");
            return pm;
        }

        //! MNT
        static const QPixmap &roleMnt()
        {
            static const QPixmap pm(":/vatsim/icons/vatsim/MNT.png");
            return pm;
        }

        //! OBS
        static const QPixmap &roleObs()
        {
            static const QPixmap pm(":/vatsim/icons/vatsim/OBS.png");
            return pm;
        }

        //! SUP
        static const QPixmap &roleSup()
        {
            static const QPixmap pm(":/vatsim/icons/vatsim/SUP.png");
            return pm;
        }

        //! Pilot
        static const QPixmap &rolePilot()
        {
            static const QPixmap pm(QPixmap(":/own/icons/own/pilot.png").scaledToWidth(16, Qt::SmoothTransformation));
            return pm;
        }

        //! Approach
        static const QPixmap &roleApproach()
        {
            static const QPixmap pm(QPixmap(":/own/icons/own/app.jpg").scaledToWidth(16, Qt::SmoothTransformation));
            return pm;
        }

        //! Ground
        static const QPixmap &roleGround()
        {
            static const QPixmap pm(QPixmap(":/own/icons/own/gnd.jpg").scaledToWidth(16, Qt::SmoothTransformation));
            return pm;
        }

        //! Delivery
        static const QPixmap &roleDelivery()
        {
            static const QPixmap pm(QPixmap(":/own/icons/own/del.jpg").scaledToWidth(16, Qt::SmoothTransformation));
            return pm;
        }

        //! Tower
        static const QPixmap &roleTower()
        {
            static const QPixmap pm(QPixmap(":/own/icons/own/twr.jpg").scaledToWidth(16, Qt::SmoothTransformation));
            return pm;
        }

        //! Center
        static const QPixmap &roleCenter()
        {
            static const QPixmap pm(QPixmap(":/own/icons/own/ctr.jpg").scaledToWidth(16, Qt::SmoothTransformation));
            return pm;
        }

        //! Unknown
        static const QPixmap &roleUnknown()
        {
            static const QPixmap pm(":/pastel/icons/pastel/16/help.png");
            return pm;
        }

        //! Full voice capability
        static const QPixmap &capabilityVoice()
        {
            static const QPixmap pm(":/diagona/icons/diagona/icons/headphone.png");
            return pm;
        }

        //! Full voice capability
        static const QPixmap &capabilityVoiceBackground();

        //! Voice receive only
        static const QPixmap &capabilityVoiceReceiveOnly()
        {
            static const QPixmap pm(":/diagona/icons/diagona/icons/headphone.png");
            return pm;
        }

        //! Voice receive only
        static const QPixmap &capabilityVoiceReceiveOnlyBackground();

        //! Text only
        static const QPixmap &capabilityTextOnly()
        {
            return CIcons::appTextMessages16();
        }

        //! Text only
        static const QPixmap &capabilityUnknown()
        {
            return CIcons::unknown16();
        }

        //! Attitude indicator
        static const QPixmap &attitudeIndicator16()
        {
            static const QPixmap pm(":/own/icons/own/attitude_indicator_climbing_16.png");
            return pm;
        }

        //! METAR
        static const QPixmap &metar()
        {
            return weatherCloudy16();
        }

        //! ATIS
        static const QPixmap &atis()
        {
            return text16();
        }

        //! Geo position
        static const QPixmap &geoPosition16()
        {
            return globe16();
        }

        // -------------------------------------------------------------
        // By index
        // -------------------------------------------------------------

        //! Pixmap by given index
        static const QPixmap &pixmapByIndex(IconIndex index);

        //! Pixmap by given index rotated
        static QPixmap pixmapByIndex(IconIndex index, int rotateDegrees);

        //! Change color of resource
        static QPixmap changeBackground(const QString resource, Qt::GlobalColor backgroundColor);
    };
}
#endif // guard
