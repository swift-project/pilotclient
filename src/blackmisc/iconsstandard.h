/* Copyright (C) 2013
 * Swift Project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKMISC_ICONSSTANDARD_H
#define BLACKMISC_ICONSSTANDARD_H

#include <QPixmap>

//! \file

namespace BlackMisc
{
    //! Standard icons
    class CIconsStandard
    {
    private:
        //! Constructor, use class static only
        CIconsStandard();

    public:

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
        static const QPixmap &volumneHigh16()
        {
            static const QPixmap pm(":/diagona/icons/diagona/icons/speaker-volume.png");
            return pm;
        }

        //! Sound volume low
        static const QPixmap &volumneLow16()
        {
            static const QPixmap pm(":/diagona/icons/diagona/icons/speaker-volume-low.png");
            return pm;
        }

        //! Sound volume muted
        static const QPixmap &volumneMuted16()
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
        static const QPixmap &appFlightplan16()
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

    };
}
#endif // guard
