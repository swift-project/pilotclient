/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_ICONLIST_H
#define BLACKMISC_ICONLIST_H

#include "valueobject.h"
#include "sequence.h"
#include "collection.h"
#include "icon.h"
#include "icons.h"

namespace BlackMisc
{

    /*!
     * Icon
     */
    class CIconList : public CSequence<CIcon>
    {
    public:
        //! Constructor
        CIconList() {}

        //! Construct from a base class object.
        CIconList(const CSequence<CIcon> &other);

        //! Find by index
        CIcon findByIndex(CIcons::IconIndex index) const;

        //! \copydoc CValueObject::asQVariant
        virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

        //! Register metadata of unit and quantity
        static void registerMetadata();

        //! All icons
        static const CIconList &allIcons()
        {
            static CIconList icons;
            if (icons.isEmpty())
            {
                icons.push_back(CIcon(CIcons::StandardIconAppAircrafts16, "aircrafts"));
                icons.push_back(CIcon(CIcons::StandardIconAppAircrafts16, "aircrafts"));
                icons.push_back(CIcon(CIcons::StandardIconAppAtc16, "ATC"));
                icons.push_back(CIcon(CIcons::StandardIconAppFlightPlan16, "flight plan"));
                icons.push_back(CIcon(CIcons::StandardIconAppLog16, "log"));
                icons.push_back(CIcon(CIcons::StandardIconAppMappings16, "mappings"));
                icons.push_back(CIcon(CIcons::StandardIconAppSettings16, "settings"));
                icons.push_back(CIcon(CIcons::StandardIconAppSimulator16, "simulator"));
                icons.push_back(CIcon(CIcons::StandardIconAppTextMessages16, "text messages"));
                icons.push_back(CIcon(CIcons::StandardIconAppUsers16, "users"));
                icons.push_back(CIcon(CIcons::StandardIconAppWeather16, "weather"));
                icons.push_back(CIcon(CIcons::StandardIconClose16, "close"));
                icons.push_back(CIcon(CIcons::StandardIconDockBottom16, "dock bottom"));
                icons.push_back(CIcon(CIcons::StandardIconDockTop16, "dock top"));
                icons.push_back(CIcon(CIcons::StandardIconEmpty, "empty"));
                icons.push_back(CIcon(CIcons::StandardIconEmpty16, "empty"));
                icons.push_back(CIcon(CIcons::StandardIconError16, "error"));
                icons.push_back(CIcon(CIcons::StandardIconFloatAll16, "float all"));
                icons.push_back(CIcon(CIcons::StandardIconFloatOne16, "floast one"));
                icons.push_back(CIcon(CIcons::StandardIconHeadingOne16, "heading"));
                icons.push_back(CIcon(CIcons::StandardIconInfo16, "info"));
                icons.push_back(CIcon(CIcons::StandardIconJoystick16, "joystick"));
                icons.push_back(CIcon(CIcons::StandardIconMonitorError16, "monitor error"));
                icons.push_back(CIcon(CIcons::StandardIconPaperPlane16, "plane"));
                icons.push_back(CIcon(CIcons::StandardIconPlugin16, "plugin"));
                icons.push_back(CIcon(CIcons::StandardIconRadar16, "radar"));
                icons.push_back(CIcon(CIcons::StandardIconResize16, "resize"));
                icons.push_back(CIcon(CIcons::StandardIconStatusBar16, "status bar"));
                icons.push_back(CIcon(CIcons::StandardIconSwift24, "swift"));
                icons.push_back(CIcon(CIcons::StandardIconSwift48, "swift"));
                icons.push_back(CIcon(CIcons::StandardIconSwiftNova24, "swift"));
                icons.push_back(CIcon(CIcons::StandardIconSwiftNova48, "swift"));
                icons.push_back(CIcon(CIcons::StandardIconTableRelationship16, "relationship"));
                icons.push_back(CIcon(CIcons::StandardIconTableSheet16, "table sheet"));
                icons.push_back(CIcon(CIcons::StandardIconText16, "text"));
                icons.push_back(CIcon(CIcons::StandardIconUnknown16, "unknwon"));
                icons.push_back(CIcon(CIcons::StandardIconUser16, "user"));
                icons.push_back(CIcon(CIcons::StandardIconUsers16, "users"));
                icons.push_back(CIcon(CIcons::StandardIconVolumeHigh16, "volume hight"));
                icons.push_back(CIcon(CIcons::StandardIconVolumeLow16, "volume low"));
                icons.push_back(CIcon(CIcons::StandardIconVolumeMuted16, "muted"));
                icons.push_back(CIcon(CIcons::StandardIconWarning16, "warning"));
                icons.push_back(CIcon(CIcons::StandardIconWeatherCloudy16, "cloudy"));
                icons.push_back(CIcon(CIcons::StandardIconWrench16, "wrench"));
                icons.push_back(CIcon(CIcons::StandardIconArrowMediumEast16, "arrow east"));
                icons.push_back(CIcon(CIcons::StandardIconArrowMediumWest16, "arrow west"));
                icons.push_back(CIcon(CIcons::StandardIconArrowMediumNorth16, "arrow north"));
                icons.push_back(CIcon(CIcons::StandardIconArrowMediumSouth16, "arrow south"));
                icons.push_back(CIcon(CIcons::NetworkCapabilityTextOnly, "text only"));
                icons.push_back(CIcon(CIcons::NetworkCapabilityUnknown, "unknown"));
                icons.push_back(CIcon(CIcons::NetworkCapabilityVoice, "voice"));
                icons.push_back(CIcon(CIcons::NetworkCapabilityVoiceReceiveOnly, "voice receive"));
                icons.push_back(CIcon(CIcons::NetworkRoleApproach, "approach"));
                icons.push_back(CIcon(CIcons::NetworkRoleC1, "C1"));
                icons.push_back(CIcon(CIcons::NetworkRoleC3, "C3"));
                icons.push_back(CIcon(CIcons::NetworkRoleCenter, "center"));
                icons.push_back(CIcon(CIcons::NetworkRoleDelivery, "delivery"));
                icons.push_back(CIcon(CIcons::NetworkRoleGround, "ground"));
                icons.push_back(CIcon(CIcons::NetworkRoleI1, "I1 (instructor)"));
                icons.push_back(CIcon(CIcons::NetworkRoleI3, "I3 (instructor)"));
                icons.push_back(CIcon(CIcons::NetworkRoleMnt, "Mentor"));
                icons.push_back(CIcon(CIcons::NetworkRoleObs, "observer"));
                icons.push_back(CIcon(CIcons::NetworkRolePilot, "pilot"));
                icons.push_back(CIcon(CIcons::NetworkRoleS1, "S1"));
                icons.push_back(CIcon(CIcons::NetworkRoleS2, "S2"));
                icons.push_back(CIcon(CIcons::NetworkRoleS3, "S3"));
                icons.push_back(CIcon(CIcons::NetworkRoleSup, "supervisor"));
                icons.push_back(CIcon(CIcons::NetworkRoleTower, "tower"));
                icons.push_back(CIcon(CIcons::NetworkRoleUnknown, "unknown"));

                icons.push_back(CIcon(CIcons::AviationAttitudeIndicator, "attitude indicator"));
                icons.push_back(CIcon(CIcons::GeoPosition, "geo position"));
                icons.push_back(CIcon(CIcons::NotSet, "?"));
            }
            return icons;
        }

        //! Icon for given index
        static const CIcon &iconForIndex(CIcons::IconIndex index)
        {
            auto foundRange = allIcons().findBy(&CIcon::getIndex, index);
            if (!foundRange.isEmpty()) { return foundRange.front(); }
            Q_ASSERT_X(false, "iconForIndex", "Missing index");
            return iconForIndex(CIcons::StandardIconUnknown16);
        }

        //! Icon for given index
        static const CIcon &iconForIndex(int index)
        {
            return iconForIndex(static_cast<CIcons::IconIndex>(index));
        }

    };
}

Q_DECLARE_METATYPE(BlackMisc::CIconList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::CIcon>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::CIcon>)

#endif // guard
