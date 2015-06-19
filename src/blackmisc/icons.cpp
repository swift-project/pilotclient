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
#include <QIcon>
#include <QPainter>

namespace BlackMisc
{

    const QPixmap &CIcons::info16()
    {
        static const QPixmap pm(":/pastel/icons/pastel/16/infomation.png");
        return pm;
    }

    const QPixmap &CIcons::warning16()
    {
        static const QPixmap pm(":/pastel/icons/pastel/16/bullet-error.png");
        return pm;
    }

    const QPixmap &CIcons::error16()
    {
        static const QPixmap pm(":/pastel/icons/pastel/16/close-red.png");
        return pm;
    }

    const QPixmap &CIcons::close16()
    {
        static const QPixmap pm(":/diagona/icons/diagona/icons/cross-button.png");
        return pm;
    }

    const QPixmap &CIcons::resize16()
    {
        static const QPixmap pm(":/diagona/icons/diagona/icons/arrow-resize-135.png");
        return pm;
    }

    const QPixmap &CIcons::resizeHorizontal16()
    {
        static const QPixmap pm(":/diagona/icons/diagona/icons/arrow-resize.png");
        return pm;
    }

    const QPixmap &CIcons::resizeVertical16()
    {
        static const QPixmap pm(":/diagona/icons/diagona/icons/arrow-resize-090.png");
        return pm;
    }

    const QPixmap &CIcons::refresh16()
    {
        static const QPixmap pm(":/pastel/icons/pastel/16/arrow-refresh.png");
        return pm;
    }

    const QPixmap &CIcons::text16()
    {
        static const QPixmap pm(":/pastel/icons/pastel/16/application-view-text.png");
        return pm;
    }

    const QPixmap &CIcons::globe16()
    {
        static const QPixmap pm(":/diagona/icons/diagona/icons/globe.png");
        return pm;
    }

    const QPixmap &CIcons::headingOne16()
    {
        static const QPixmap pm(":/pastel/icons/pastel/16/text-heading-1.png");
        return pm;
    }

    const QPixmap &CIcons::user16()
    {
        static const QPixmap pm(":/pastel/icons/pastel/16/user.png");
        return pm;
    }

    const QPixmap &CIcons::users16()
    {
        static const QPixmap pm(":/diagona/icons/diagona/icons/users.png");
        return pm;
    }

    const QPixmap &CIcons::viewMultiColumn()
    {
        static const QPixmap pm(":/pastel/icons/pastel/16/application-view-multicolumns.png");
        return pm;
    }

    const QPixmap &CIcons::viewTile()
    {
        static const QPixmap pm(":/pastel/icons/pastel/16/application-view-tile.png");
        return pm;
    }

    const QPixmap &CIcons::tick16()
    {
        static const QPixmap pm(":/diagona/icons/diagona/icons/tick.png");
        return pm;
    }

    const QPixmap &CIcons::tickRed16()
    {
        static const QPixmap pm(":/diagona/icons/diagona/icons/tick-red.png");
        return pm;
    }

    const QPixmap &CIcons::tickWhite16()
    {
        static const QPixmap pm(":/diagona/icons/diagona/icons/tick-white.png");
        return pm;
    }

    const QPixmap &CIcons::tickSmall16()
    {
        static const QPixmap pm(":/diagona/icons/diagona/icons/tick-small.png");
        return pm;
    }

    const QPixmap &CIcons::cross16()
    {
        static const QPixmap pm(":/diagona/icons/diagona/icons/cross.png");
        return pm;
    }

    const QPixmap &CIcons::crossWhite16()
    {
        static const QPixmap pm(":/diagona/icons/diagona/icons/cross-white.png");
        return pm;
    }

    const QPixmap &CIcons::crossCircle16()
    {
        static const QPixmap pm(":/diagona/icons/diagona/icons/cross-circle.png");
        return pm;
    }

    const QPixmap &CIcons::crossSmall16()
    {
        static const QPixmap pm(":/diagona/icons/diagona/icons/cross-small.png");
        return pm;
    }

    const QPixmap &CIcons::delete16()
    {
        static const QPixmap pm(":/pastel/icons/pastel/16/delete.png");
        return pm;
    }

    const QPixmap &CIcons::radio16()
    {
        static const QPixmap pm(":/diagona/icons/diagona/icons/radio.png");
        return pm;
    }

    const QPixmap &CIcons::speakerNetwork16()
    {
        static const QPixmap pm(":/diagona/icons/diagona/icons/speaker-network.png");
        return pm;
    }

    const QPixmap &CIcons::joystick16()
    {
        static const QPixmap pm(":/diagona/icons/diagona/icons/joystick.png");
        return pm;
    }

    const QPixmap &CIcons::lockOpen16()
    {
        static const QPixmap pm(":/pastel/icons/pastel/16/lock-open.png");
        return pm;
    }

    const QPixmap &CIcons::lockClosed16()
    {
        static const QPixmap pm(":/pastel/icons/pastel/16/lock.png");
        return pm;
    }

    const QPixmap &CIcons::plugin16()
    {
        static const QPixmap pm(":/pastel/icons/pastel/16/plugin.png");
        return pm;
    }

    const QPixmap &CIcons::dockTop16()
    {
        static const QPixmap pm(":/diagona/icons/diagona/icons/application-dock-090.png");
        return pm;
    }

    const QPixmap &CIcons::dockBottom16()
    {
        static const QPixmap pm(":/diagona/icons/diagona/icons/application-dock-270.png");
        return pm;
    }

    const QPixmap &CIcons::floatAll16()
    {
        static const QPixmap pm(":/pastel/icons/pastel/16/application-cascade.png");
        return pm;
    }

    const QPixmap &CIcons::floatOne16()
    {
        static const QPixmap pm(":/pastel/icons/pastel/16/application-double.png");
        return pm;
    }

    const QPixmap &CIcons::volumeHigh16()
    {
        static const QPixmap pm(":/diagona/icons/diagona/icons/speaker-volume.png");
        return pm;
    }

    const QPixmap &CIcons::volumeLow16()
    {
        static const QPixmap pm(":/diagona/icons/diagona/icons/speaker-volume-low.png");
        return pm;
    }

    const QPixmap &CIcons::volumeMuted16()
    {
        static const QPixmap pm(":/diagona/icons/diagona/icons/speaker-volume-control-mute.png");
        return pm;
    }

    const QPixmap &CIcons::wrench16()
    {
        static const QPixmap pm(":/pastel/icons/pastel/16/wrench.png");
        return pm;
    }

    const QPixmap &CIcons::radar16()
    {
        static const QPixmap pm(":/diagona/icons/diagona/icons/radar.png");
        return pm;
    }

    const QPixmap &CIcons::tableRelationship16()
    {
        static const QPixmap pm(":/pastel/icons/pastel/16/table-relationship.png");
        return pm;
    }

    const QPixmap &CIcons::paperPlane16()
    {
        static const QPixmap pm(":/diagona/icons/diagona/icons/paper-plane.png");
        return pm;
    }

    const QPixmap &CIcons::tableSheet16()
    {
        static const QPixmap pm(":/diagona/icons/diagona/icons/table-sheet.png");
        return pm;
    }

    const QPixmap &CIcons::weatherCloudy16()
    {
        static const QPixmap pm(":/diagona/icons/diagona/icons/weather-cloudy.png");
        return pm;
    }

    const QPixmap &CIcons::monitorError16()
    {
        static const QPixmap pm(":/pastel/icons/pastel/16/monitor-error.png");
        return pm;
    }

    const QPixmap &CIcons::statusBar16()
    {
        static const QPixmap pm(":/pastel/icons/pastel/16/application-statusbar.png");
        return pm;
    }

    const QPixmap &CIcons::unknown16()
    {
        static const QPixmap pm(":/pastel/icons/pastel/16/help.png");
        return pm;
    }

    const QPixmap &CIcons::empty()
    {
        static const QPixmap pm;
        return pm;
    }

    const QPixmap &CIcons::empty16()
    {
        static const QPixmap pm(16, 16);
        return pm;
    }

    const QPixmap &CIcons::arrowMediumNorth16()
    {
        static const QPixmap pm(":/diagona/icons/diagona/icons/arrow-090-medium.png");
        return pm;
    }

    const QPixmap &CIcons::arrowMediumSouth16()
    {
        static const QPixmap pm(":/diagona/icons/diagona/icons/arrow-270-medium.png");
        return pm;
    }

    const QPixmap &CIcons::arrowMediumEast16()
    {
        static const QPixmap pm(":/diagona/icons/diagona/icons/arrow-000-medium.png");
        return pm;
    }

    const QPixmap &CIcons::arrowMediumWest16()
    {
        static const QPixmap pm(":/diagona/icons/diagona/icons/arrow-180-medium.png");
        return pm;
    }

    const QPixmap &CIcons::swift24()
    {
        static const QPixmap pm(":/own/icons/own/swift/swift24.png");
        return pm;
    }

    const QPixmap &CIcons::swift48()
    {
        static const QPixmap pm(":/own/icons/own/swift/swift48.png");
        return pm;
    }

    const QPixmap &CIcons::swiftNova24()
    {
        static const QPixmap pm(":/own/icons/own/swift/swiftCartoonNova24.png");
        return pm;
    }

    const QPixmap &CIcons::swiftNova48()
    {
        static const QPixmap pm(":/own/icons/own/swift/swiftCartoonNova48.png");
        return pm;
    }

    const QPixmap &CIcons::swiftDatabase24()
    {
        static const QPixmap pm(":/own/icons/own/swift/swiftDatabase24.png");
        return pm;
    }

    const QPixmap &CIcons::swiftDatabase48()
    {
        static const QPixmap pm(":/own/icons/own/swift/swiftDatabase48.png");
        return pm;
    }

    const QPixmap &CIcons::appWeather16()
    {
        return weatherCloudy16();
    }

    const QPixmap &CIcons::appSettings16()
    {
        return wrench16();
    }

    const QPixmap &CIcons::appUsers16()
    {
        return users16();
    }

    const QPixmap &CIcons::appFlightPlan16()
    {
        return tableSheet16();
    }

    const QPixmap &CIcons::appCockpit16()
    {
        return radio16();
    }

    const QPixmap &CIcons::appSimulator16()
    {
        return joystick16();
    }

    const QPixmap &CIcons::appTextMessages16()
    {
        return text16();
    }

    const QPixmap &CIcons::appAtc16()
    {
        return radar16();
    }

    const QPixmap &CIcons::appAircrafts16()
    {
        return paperPlane16();
    }

    const QPixmap &CIcons::appMappings16()
    {
        return tableRelationship16();
    }

    const QPixmap &CIcons::appLog16()
    {
        return monitorError16();
    }

    const QPixmap &CIcons::appAudio16()
    {
        return speakerNetwork16();
    }

    const QPixmap &CIcons::appVoiceRooms16()
    {
        return tableRelationship16();
    }

    const QPixmap &CIcons::roleC1()
    {
        static const QPixmap pm(":/vatsim/icons/vatsim/C1.png");
        return pm;
    }

    const QPixmap &CIcons::roleC2()
    {
        static const QPixmap pm(":/vatsim/icons/vatsim/C2.png");
        return pm;
    }

    const QPixmap &CIcons::roleC3()
    {
        static const QPixmap pm(":/vatsim/icons/vatsim/C2.png");
        return pm;
    }

    const QPixmap &CIcons::roleS1()
    {
        static const QPixmap pm(":/vatsim/icons/vatsim/S1.png");
        return pm;
    }

    const QPixmap &CIcons::roleS2()
    {
        static const QPixmap pm(":/vatsim/icons/vatsim/S2.png");
        return pm;
    }

    const QPixmap &CIcons::roleS3()
    {
        static const QPixmap pm(":/vatsim/icons/vatsim/S3.png");
        return pm;
    }

    const QPixmap &CIcons::roleI1()
    {
        static const QPixmap pm(":/vatsim/icons/vatsim/I1.png");
        return pm;
    }

    const QPixmap &CIcons::roleI3()
    {
        static const QPixmap pm(":/vatsim/icons/vatsim/I3.png");
        return pm;
    }

    const QPixmap &CIcons::roleMnt()
    {
        static const QPixmap pm(":/vatsim/icons/vatsim/MNT.png");
        return pm;
    }

    const QPixmap &CIcons::roleObs()
    {
        static const QPixmap pm(":/vatsim/icons/vatsim/OBS.png");
        return pm;
    }

    const QPixmap &CIcons::roleSup()
    {
        static const QPixmap pm(":/vatsim/icons/vatsim/SUP.png");
        return pm;
    }

    const QPixmap &CIcons::rolePilot()
    {
        static const QPixmap pm(QPixmap(":/own/icons/own/pilot.png").scaledToWidth(16, Qt::SmoothTransformation));
        return pm;
    }

    const QPixmap &CIcons::roleApproach()
    {
        static const QPixmap pm(QPixmap(":/own/icons/own/app.jpg").scaledToWidth(16, Qt::SmoothTransformation));
        return pm;
    }

    const QPixmap &CIcons::roleGround()
    {
        static const QPixmap pm(QPixmap(":/own/icons/own/gnd.jpg").scaledToWidth(16, Qt::SmoothTransformation));
        return pm;
    }

    const QPixmap &CIcons::roleDelivery()
    {
        static const QPixmap pm(QPixmap(":/own/icons/own/del.jpg").scaledToWidth(16, Qt::SmoothTransformation));
        return pm;
    }

    const QPixmap &CIcons::roleFss()
    {
        return info16();
    }

    const QPixmap &CIcons::roleTower()
    {
        static const QPixmap pm(QPixmap(":/own/icons/own/twr.jpg").scaledToWidth(16, Qt::SmoothTransformation));
        return pm;
    }

    const QPixmap &CIcons::roleCenter()
    {
        static const QPixmap pm(QPixmap(":/own/icons/own/ctr.jpg").scaledToWidth(16, Qt::SmoothTransformation));
        return pm;
    }

    const QPixmap &CIcons::roleUnknown()
    {
        static const QPixmap pm(":/pastel/icons/pastel/16/help.png");
        return pm;
    }

    const QPixmap &CIcons::vatsimLogo16()
    {
        static const QPixmap pm(":/vatsim/icons/vatsim/VatsimLogo16.png");
        return pm;
    }

    const QPixmap &CIcons::vatsimLogo32()
    {
        static const QPixmap pm(":/vatsim/icons/vatsim/VatsimLogo32.png");
        return pm;
    }

    const QPixmap &CIcons::vatsimLogoWhite16()
    {
        static const QPixmap pm(":/vatsim/icons/vatsim/VatsimLogoWhite16.png");
        return pm;
    }

    const QPixmap &CIcons::vatsimLogoWhite32()
    {
        static const QPixmap pm(":/vatsim/icons/vatsim/VatsimLogoWhite32.png");
        return pm;
    }

    const QPixmap &CIcons::capabilityVoice()
    {
        static const QPixmap pm(":/diagona/icons/diagona/icons/headphone.png");
        return pm;
    }

    const QPixmap &CIcons::capabilityVoiceBackground()
    {
        static const QPixmap pm(changeResourceBackgroundColor(":/diagona/icons/diagona/icons/headphone.png", Qt::green));
        return pm;
    }

    const QPixmap &CIcons::capabilityVoiceReceiveOnly()
    {
        static const QPixmap pm(":/diagona/icons/diagona/icons/headphone.png");
        return pm;
    }

    const QPixmap &CIcons::capabilityVoiceReceiveOnlyBackground()
    {
        static const QPixmap pm(changeResourceBackgroundColor(":/diagona/icons/diagona/icons/headphone.png", Qt::yellow));
        return pm;
    }

    const QPixmap &CIcons::capabilityTextOnly()
    {
        return appTextMessages16();
    }

    const QPixmap &CIcons::capabilityUnknown()
    {
        return unknown16();
    }

    const QPixmap &CIcons::attitudeIndicator16()
    {
        static const QPixmap pm(":/own/icons/own/attitude_indicator_climbing_16.png");
        return pm;
    }

    const QPixmap &CIcons::metar()
    {
        return weatherCloudy16();
    }

    const QPixmap &CIcons::atis()
    {
        return text16();
    }

    const QPixmap &CIcons::geoPosition16()
    {
        return globe16();
    }

    const QPixmap &CIcons::pixmapByIndex(CIcons::IconIndex index)
    {
        switch (index)
        {
        case ApplicationIconAircrafts: return appAircrafts16();
        case ApplicationIconAtc: return appAtc16();
        case ApplicationIconAudio: return appAudio16();
        case ApplicationIconCockpit: return appCockpit16();
        case ApplicationIconFlightPlan: return appFlightPlan16();
        case ApplicationIconLog: return appLog16();
        case ApplicationIconMappings: return appMappings16();
        case ApplicationIconSettings: return appSettings16();
        case ApplicationIconSimulator: return appSimulator16();
        case ApplicationIconTextMessages: return appTextMessages16();
        case ApplicationIconWeather: return appWeather16();
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
        case NetworkRoleFss: return roleFss();
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
        case NetworkVatsimLogo: return vatsimLogo16();
        case NetworkVatsimLogoWhite: return vatsimLogoWhite16();
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
        case StandardIconCross16: return cross16();
        case StandardIconCrossCircle16: return crossCircle16();
        case StandardIconCrossSmall16: return crossSmall16();
        case StandardIconCrossWhite16: return crossWhite16();
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
        case StandardIconRadio16: return radio16();
        case StandardIconRefresh16: return refresh16();
        case StandardIconResize16: return resize16();
        case StandardIconResizeHorizontal16: return resizeHorizontal16();
        case StandardIconResizeVertical16: return resizeVertical16();
        case StandardIconSpeakerNetwork16: return speakerNetwork16();
        case StandardIconStatusBar16: return statusBar16();
        case StandardIconSwift24: return swift24();
        case StandardIconSwift48: return swift48();
        case StandardIconSwiftNova24: return swiftNova24();
        case StandardIconSwiftNova48: return swiftNova48();
        case StandardIconTableRelationship16: return tableRelationship16();
        case StandardIconTableSheet16: return tableSheet16();
        case StandardIconText16: return text16();
        case StandardIconTick16: return tick16();
        case StandardIconTickRed16: return tickRed16();
        case StandardIconTickSmall16: return tickSmall16();
        case StandardIconTickWhite16: return tickWhite16();
        case StandardIconUnknown16: return unknown16();
        case StandardIconUser16: return user16();
        case StandardIconUsers16: return users16();
        case StandardIconViewMulticolumn16: return viewMultiColumn();
        case StandardIconViewTile16: return viewTile();
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

    QImage CIcons::changeImageBackgroundColor(const QImage &imgSource, Qt::GlobalColor backgroundColor)
    {
        QImage destBackgroundImg(imgSource.size(), QImage::Format_RGB32);
        destBackgroundImg.fill(backgroundColor);
        QPainter p(&destBackgroundImg);
        p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
        p.drawImage(0, 0, imgSource);
        return destBackgroundImg;
    }

    QPixmap CIcons::changeResourceBackgroundColor(const QString &resource, Qt::GlobalColor backgroundColor)
    {
        QImage imgSource(resource);
        QImage destBackgroundImg(changeImageBackgroundColor(imgSource, backgroundColor));
        return QPixmap::fromImage(destBackgroundImg);
    }

    QIcon CIcons::changeIconBackgroundColor(const QIcon &icon, Qt::GlobalColor backgroundColor, const QSize &targetsize)
    {
        QImage imgSource(icon.pixmap(targetsize).toImage());
        QImage destBackgroundImg(changeImageBackgroundColor(imgSource, backgroundColor));
        return QIcon(QPixmap::fromImage(destBackgroundImg));
    }

} // namespace
