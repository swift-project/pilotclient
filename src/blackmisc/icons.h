/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project:
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
            NotSet,
            StandardIconInfo16,
            StandardIconWarning16,
            StandardIconGlobe16,
            StandardIconError16,
            StandardIconClose16,
            StandardIconResize16,
            StandardIconText16,
            StandardIconHeadingOne16,
            StandardIconUser16,
            StandardIconUsers16,
            StandardIconJoystick16,
            StandardIconPlugin16,
            StandardIconDockTop16,
            StandardIconDockBottom16,
            StandardIconFloatAll16,
            StandardIconFloatOne16,
            StandardIconVolumeHigh16,
            StandardIconVolumeLow16,
            StandardIconVolumeMuted16,
            StandardIconWrench16,
            StandardIconRadar16,
            StandardIconTableRelationship16,
            StandardIconPaperPlane16,
            StandardIconTableSheet16,
            StandardIconWeatherCloudy16,
            StandardIconMonitorError16,
            StandardIconStatusBar16,
            StandardIconUnknown16,
            StandardIconArrowMediumNorth16,
            StandardIconArrowMediumSouth16,
            StandardIconArrowMediumEast16,
            StandardIconArrowMediumWest16,
            StandardIconEmpty,
            StandardIconEmpty16,
            StandardIconSwift24,
            StandardIconSwift48,
            StandardIconSwiftNova24,
            StandardIconSwiftNova48,
            StandardIconAppWeather16,
            StandardIconAppSettings16,
            StandardIconAppUsers16,
            StandardIconAppFlightPlan16,
            StandardIconAppSimulator16,
            StandardIconAppTextMessages16,
            StandardIconAppAtc16,
            StandardIconAppAircrafts16,
            StandardIconAppMappings16,
            StandardIconAppLog16,
            NetworkRoleC1,
            NetworkRoleC3,
            NetworkRoleI1,
            NetworkRoleI3,
            NetworkRoleS1,
            NetworkRoleS2,
            NetworkRoleS3,
            NetworkRoleMnt,
            NetworkRoleSup,
            NetworkRoleObs,
            NetworkRolePilot,
            NetworkRoleApproach,
            NetworkRoleGround,
            NetworkRoleDelivery,
            NetworkRoleTower,
            NetworkRoleCenter,
            NetworkRoleUnknown,
            NetworkCapabilityVoice,
            NetworkCapabilityVoiceReceiveOnly,
            NetworkCapabilityTextOnly,
            NetworkCapabilityUnknown,
            AviationAttitudeIndicator,
            GeoPosition
        };

        // -------------------------------------------------------------
        // Standard icons
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

        //! Voice receive only
        static const QPixmap &capabilityVoiceReceiveOnly()
        {
            static const QPixmap pm(":/diagona/icons/diagona/icons/headphone.png");
            return pm;
        }

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

        //! Geo position
        static const QPixmap &geoPosition16()
        {
            return globe16();
        }

        // -------------------------------------------------------------
        // By index
        // -------------------------------------------------------------

        //! Pixmap by given index
        static const QPixmap &pixmapByIndex(IconIndex index)
        {
            switch (index)
            {
            case NotSet: return empty();
            case StandardIconInfo16: return info16();
            case StandardIconWarning16: return warning16();
            case StandardIconError16: return error16();
            case StandardIconClose16: return close16();
            case StandardIconResize16: return resize16();
            case StandardIconText16: return text16();
            case StandardIconGlobe16: return globe16();
            case StandardIconHeadingOne16: return headingOne16();
            case StandardIconUser16: return user16();
            case StandardIconUsers16: return users16();
            case StandardIconJoystick16: return joystick16();
            case StandardIconPlugin16: return plugin16();
            case StandardIconDockTop16: return dockTop16();
            case StandardIconDockBottom16: return dockBottom16();
            case StandardIconFloatAll16: return floatAll16();
            case StandardIconFloatOne16: return floatOne16();
            case StandardIconVolumeHigh16: return volumeHigh16();
            case StandardIconVolumeLow16: return volumeLow16();
            case StandardIconVolumeMuted16: return volumeMuted16();
            case StandardIconWrench16: return wrench16();
            case StandardIconRadar16: return radar16();
            case StandardIconTableRelationship16: return tableRelationship16();
            case StandardIconPaperPlane16: return paperPlane16();
            case StandardIconTableSheet16: return tableSheet16();
            case StandardIconWeatherCloudy16: return weatherCloudy16();
            case StandardIconMonitorError16: return monitorError16();
            case StandardIconStatusBar16: return statusBar16();
            case StandardIconUnknown16: return unknown16();
            case StandardIconArrowMediumNorth16: return arrowMediumNorth16();
            case StandardIconArrowMediumSouth16: return arrowMediumSouth16();
            case StandardIconArrowMediumWest16: return arrowMediumWest16();
            case StandardIconArrowMediumEast16: return arrowMediumEast16();
            case StandardIconEmpty: return empty();
            case StandardIconEmpty16: return empty16();
            case StandardIconSwift24: return swift24();
            case StandardIconSwift48: return swift48();
            case StandardIconSwiftNova24: return swiftNova24();
            case StandardIconSwiftNova48: return swiftNova48();
            case StandardIconAppWeather16: return appWeather16();
            case StandardIconAppSettings16: return appSettings16();
            case StandardIconAppUsers16: return appUsers16();
            case StandardIconAppFlightPlan16: return appFlightPlan16();
            case StandardIconAppSimulator16: return appSimulator16();
            case StandardIconAppTextMessages16: return appTextMessages16();
            case StandardIconAppAtc16: return appAtc16();
            case StandardIconAppAircrafts16: return appAircrafts16();
            case StandardIconAppMappings16: return appMappings16();
            case StandardIconAppLog16: return appLog16();
            case NetworkRoleC1: return roleC1();
            case NetworkRoleC3: return roleC3();
            case NetworkRoleI1: return roleI1();
            case NetworkRoleI3: return roleI3();
            case NetworkRoleS1: return roleS1();
            case NetworkRoleS2: return roleS2();
            case NetworkRoleS3: return roleS3();
            case NetworkRoleMnt: return roleMnt();
            case NetworkRoleSup: return roleSup();
            case NetworkRoleObs: return roleObs();
            case NetworkRolePilot: return rolePilot();
            case NetworkRoleApproach: return roleApproach();
            case NetworkRoleGround: return roleGround();
            case NetworkRoleDelivery: return roleDelivery();
            case NetworkRoleTower: return roleTower();
            case NetworkRoleCenter: return roleCenter();
            case NetworkRoleUnknown: return roleUnknown();
            case NetworkCapabilityVoice: return capabilityVoice();
            case NetworkCapabilityVoiceReceiveOnly: return capabilityVoiceReceiveOnly();
            case NetworkCapabilityTextOnly: return capabilityTextOnly();
            case NetworkCapabilityUnknown: return capabilityUnknown();
            case AviationAttitudeIndicator: return attitudeIndicator16();
            case GeoPosition: return geoPosition16();
            default: return empty();
            }
        }

        //! Pixmap by given index rotated
        static QPixmap pixmapByIndex(IconIndex index, int rotateDegrees)
        {
            const QPixmap original = pixmapByIndex(index);
            if (rotateDegrees == 0) return original;
            QMatrix rm;
            rm.rotate(rotateDegrees);
            QPixmap rotated = original.transformed(rm);
            int xoffset = (rotated.width() - original.width()) / 2;
            int yoffset = (rotated.height() - original.height()) / 2;
            rotated = rotated.copy(xoffset, yoffset, original.width(), original.height());
            return rotated;
        }
    };
}
#endif // guard
