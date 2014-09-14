/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "icons.h"
#include <QImage>
#include <QPainter>

namespace BlackMisc
{

    const QPixmap &CIcons::capabilityVoiceBackground()
    {
        static const QPixmap pm(changeBackground(":/diagona/icons/diagona/icons/headphone.png", Qt::green));
        return pm;
    }

    const QPixmap &CIcons::capabilityVoiceReceiveOnlyBackground()
    {
        static const QPixmap pm(changeBackground(":/diagona/icons/diagona/icons/headphone.png", Qt::yellow));
        return pm;
    }

    const QPixmap &CIcons::pixmapByIndex(CIcons::IconIndex index)
    {
        switch (index)
        {
        case AviationAtis: return atis();
        case AviationAttitudeIndicator: return attitudeIndicator16();
        case AviationMetar: return metar();
        case GeoPosition: return geoPosition16();
        case NetworkCapabilityTextOnly: return capabilityTextOnly();
        case NetworkCapabilityUnknown: return capabilityUnknown();
        case NetworkCapabilityVoice: return capabilityVoice();
        case NetworkCapabilityVoiceBackground: return capabilityVoiceBackground();
        case NetworkCapabilityVoiceReceiveOnly: return capabilityVoiceReceiveOnly();
        case NetworkCapabilityVoiceReceiveOnlyBackground: return capabilityVoiceReceiveOnlyBackground();
        case NetworkRoleApproach: return roleApproach();
        case NetworkRoleC1: return roleC1();
        case NetworkRoleC3: return roleC3();
        case NetworkRoleCenter: return roleCenter();
        case NetworkRoleDelivery: return roleDelivery();
        case NetworkRoleGround: return roleGround();
        case NetworkRoleI1: return roleI1();
        case NetworkRoleI3: return roleI3();
        case NetworkRoleMnt: return roleMnt();
        case NetworkRoleObs: return roleObs();
        case NetworkRolePilot: return rolePilot();
        case NetworkRoleS1: return roleS1();
        case NetworkRoleS2: return roleS2();
        case NetworkRoleS3: return roleS3();
        case NetworkRoleSup: return roleSup();
        case NetworkRoleTower: return roleTower();
        case NetworkRoleUnknown: return roleUnknown();
        case NotSet: return empty();
        case StandardIconAppAircrafts16: return appAircrafts16();
        case StandardIconAppAtc16: return appAtc16();
        case StandardIconAppFlightPlan16: return appFlightPlan16();
        case StandardIconAppLog16: return appLog16();
        case StandardIconAppMappings16: return appMappings16();
        case StandardIconAppSettings16: return appSettings16();
        case StandardIconAppSimulator16: return appSimulator16();
        case StandardIconAppTextMessages16: return appTextMessages16();
        case StandardIconAppUsers16: return appUsers16();
        case StandardIconAppWeather16: return appWeather16();
        case StandardIconArrowMediumEast16: return arrowMediumEast16();
        case StandardIconArrowMediumNorth16: return arrowMediumNorth16();
        case StandardIconArrowMediumSouth16: return arrowMediumSouth16();
        case StandardIconArrowMediumWest16: return arrowMediumWest16();
        case StandardIconClose16: return close16();
        case StandardIconCross: return cross16();
        case StandardIconCrossCircle: return crossCircle16();
        case StandardIconCrossSmall: return crossSmall16();
        case StandardIconCrossWhite: return crossWhite16();
        case StandardIconDelete16: return delete16();
        case StandardIconDockBottom16: return dockBottom16();
        case StandardIconDockTop16: return dockTop16();
        case StandardIconEmpty16: return empty16();
        case StandardIconEmpty: return empty();
        case StandardIconError16: return error16();
        case StandardIconFloatAll16: return floatAll16();
        case StandardIconFloatOne16: return floatOne16();
        case StandardIconGlobe16: return globe16();
        case StandardIconHeadingOne16: return headingOne16();
        case StandardIconInfo16: return info16();
        case StandardIconJoystick16: return joystick16();
        case StandardIconMonitorError16: return monitorError16();
        case StandardIconPaperPlane16: return paperPlane16();
        case StandardIconPlugin16: return plugin16();
        case StandardIconRadar16: return radar16();
        case StandardIconRefresh16: return refresh16();
        case StandardIconResize16: return resize16();
        case StandardIconStatusBar16: return statusBar16();
        case StandardIconSwift24: return swift24();
        case StandardIconSwift48: return swift48();
        case StandardIconSwiftNova24: return swiftNova24();
        case StandardIconSwiftNova48: return swiftNova48();
        case StandardIconTableRelationship16: return tableRelationship16();
        case StandardIconTableSheet16: return tableSheet16();
        case StandardIconText16: return text16();
        case StandardIconTick: return tick16();
        case StandardIconTickRed: return tickRed16();
        case StandardIconTickSmall: return tickSmall16();
        case StandardIconTickWhite: return tickWhite16();
        case StandardIconUnknown16: return unknown16();
        case StandardIconUser16: return user16();
        case StandardIconUsers16: return users16();
        case StandardIconVolumeHigh16: return volumeHigh16();
        case StandardIconVolumeLow16: return volumeLow16();
        case StandardIconVolumeMuted16: return volumeMuted16();
        case StandardIconWarning16: return warning16();
        case StandardIconWeatherCloudy16: return weatherCloudy16();
        case StandardIconWrench16: return wrench16();
        default: return empty();
        }
    }

    QPixmap CIcons::pixmapByIndex(CIcons::IconIndex index, int rotateDegrees)
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

    QPixmap CIcons::changeBackground(const QString resource, Qt::GlobalColor backgroundColor)
    {
        QImage resSource(resource);
        QImage destBackground(resSource.size(), QImage::Format_RGB32);
        destBackground.fill(backgroundColor);
        QPainter p(&destBackground);
        p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
        p.drawImage(0, 0, resSource);
        return QPixmap::fromImage(destBackground);
    }

} // namespace
