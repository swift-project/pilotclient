/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \cond PRIVATE

#include "blackmisc/directoryutils.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/icons.h"
#include "blackmisc/threadutils.h"

#include <QIcon>
#include <QImage>
#include <QMap>
#include <QMatrix>
#include <QPainter>
#include <QSize>
#include <QtGlobal>

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

    const QPixmap &CIcons::color16()
    {
        static const QPixmap pm(":/diagona/icons/diagona/icons/color.png");
        return pm;
    }

    const QPixmap &CIcons::disk16()
    {
        static const QPixmap pm(":/pastel/icons/pastel/16/disk.png");
        return pm;
    }

    const QPixmap &CIcons::load16()
    {
        return disk16();
    }

    const QPixmap &CIcons::save16()
    {
        return disk16();
    }

    const QPixmap &CIcons::colorPicker16()
    {
        static const QPixmap pm(":/diagona/icons/diagona/icons/ui-color-picker.png");
        return pm;
    }

    const QPixmap &CIcons::colorSwatch16()
    {
        static const QPixmap pm(":/diagona/icons/diagona/icons/color-swatches.png");
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

    const QPixmap &CIcons::paintCan16()
    {
        static const QPixmap pm(":/pastel/icons/pastel/16/paintcan.png");
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

    const QPixmap &CIcons::dragAndDrop16()
    {
        static const QPixmap pm(":/diagona/icons/diagona/icons/document-insert.png");
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

    QPixmap transparentPixmapImpl(QSize size)
    {
        // http://curtis.humphreyonline.us/code/qt-code/making-a-qpixmap-transparent-wrong-way-right-way
        QPixmap pix(size);
        pix.fill(Qt::transparent);

        QPainter painter;
        painter.begin(&pix);
        painter.drawPixmap(pix.rect(), pix);
        painter.end();
        return pix;
    }

    const QPixmap &CIcons::empty16()
    {
        static const QPixmap pm(transparentPixmapImpl(QSize(16, 16)));
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

    const QPixmap &CIcons::copy16()
    {
        static const QPixmap pm(":/diagona/icons/diagona/icons/document-copy.png");
        return pm;
    }

    const QPixmap &CIcons::paste16()
    {
        static const QPixmap pm(":/diagona/icons/diagona/icons/clipboard-paste.png");
        return pm;
    }

    const QPixmap &CIcons::cut16()
    {
        static const QPixmap pm(":/diagona/icons/diagona/icons/scissors.png");
        return pm;
    }

    const QPixmap &CIcons::building16()
    {
        static const QPixmap pm(":/diagona/icons/diagona/icons/building.png");
        return pm;
    }

    const QPixmap &CIcons::filter16()
    {
        return tableSheet16();
    }

    const QPixmap &CIcons::font16()
    {
        static const QPixmap pm(":/pastel/icons/pastel/16/font.png");
        return pm;
    }

    const QPixmap &CIcons::folder16()
    {
        static const QPixmap pm(":/pastel/icons/pastel/16/folder.png");
        return pm;
    }

    const QPixmap &CIcons::folderEdit16()
    {
        static const QPixmap pm(":/pastel/icons/pastel/16/folder-edit.png");
        return pm;
    }

    const QPixmap &CIcons::database16()
    {
        static const QPixmap pm(":/pastel/icons/pastel/16/database.png");
        return pm;
    }

    const QPixmap &CIcons::databaseAdd16()
    {
        static const QPixmap pm(":/pastel/icons/pastel/16/database-add.png");
        return pm;
    }

    const QPixmap &CIcons::databaseConnect16()
    {
        static const QPixmap pm(":/pastel/icons/pastel/16/database-conenct.png");
        return pm;
    }

    const QPixmap &CIcons::databaseDelete16()
    {
        static const QPixmap pm(":/pastel/icons/pastel/16/database-delete.png");
        return pm;
    }

    const QPixmap &CIcons::databaseEdit16()
    {
        static const QPixmap pm(":/pastel/icons/pastel/16/database-edit.png");
        return pm;
    }

    const QPixmap &CIcons::databaseError16()
    {
        static const QPixmap pm(":/pastel/icons/pastel/16/database-error.png");
        return pm;
    }

    const QPixmap &CIcons::databaseKey16()
    {
        static const QPixmap pm(":/pastel/icons/pastel/16/database-key.png");
        return pm;
    }

    const QPixmap &CIcons::databaseTable16()
    {
        static const QPixmap pm(":/pastel/icons/pastel/16/database-table.png");
        return pm;
    }

    const QPixmap &CIcons::osAll()
    {
        return empty16();
    }

    const QPixmap &CIcons::osLinux()
    {
        static const QPixmap pm(":/os/icons/os/linux_24.png");
        return pm;
    }

    const QPixmap &CIcons::osMac()
    {
        static const QPixmap pm(":/os/icons/os/macos_24.png");
        return pm;
    }

    const QPixmap &CIcons::osWindows()
    {
        static const QPixmap pm(":/os/icons/os/windows_24.png");
        return pm;
    }

    const QPixmap &CIcons::preloader64()
    {
        static const QPixmap pm(":/preloaders/icons/preloaders/FillingRing64.gif");
        return pm;
    }

    const QPixmap &CIcons::preloader32()
    {
        static const QPixmap pm(":/preloaders/icons/preloaders/FillingRing32.gif");
        return pm;
    }

    const QPixmap &CIcons::preloader16()
    {
        static const QPixmap pm(":/preloaders/icons/preloaders/FillingRing16.gif");
        return pm;
    }

    const QPixmap &CIcons::simulatorFSX()
    {
        static const QPixmap pm(":/simulators/icons/simulators/FSX.png");
        return pm;
    }

    const QPixmap &CIcons::simulatorFSX16()
    {
        static const QPixmap pm(simulatorFSX().scaled(32, 16, Qt::KeepAspectRatio));
        return pm;
    }

    const QPixmap &CIcons::simulatorP3D()
    {
        static const QPixmap pm(":/simulators/icons/simulators/Prepar3D.png");
        return pm;
    }

    const QPixmap &CIcons::simulatorP3D16()
    {
        static const QPixmap pm(simulatorP3D().scaled(32, 16, Qt::KeepAspectRatio));
        return pm;
    }

    const QPixmap &CIcons::simulatorFS9()
    {
        static const QPixmap pm(":/simulators/icons/simulators/FS9.png");
        return pm;
    }

    const QPixmap &CIcons::simulatorFS916()
    {
        static const QPixmap pm(simulatorFS9().scaled(32, 16, Qt::KeepAspectRatio));
        return pm;
    }

    const QPixmap &CIcons::simulatorFG()
    {
        static const QPixmap pm(":/simulators/icons/simulators/FG.png");
        return pm;
    }

    const QPixmap &CIcons::simulatorFG16()
    {
        static const QPixmap pm(simulatorFG().scaled(32, 16, Qt::KeepAspectRatio));
        return pm;
    }

    const QPixmap &CIcons::simulatorXPlane()
    {
        static const QPixmap pm(":/simulators/icons/simulators/FS9.png");
        return pm;
    }

    const QPixmap &CIcons::simulatorXPlane16()
    {
        static const QPixmap pm(simulatorXPlane().scaled(32, 16, Qt::KeepAspectRatio));
        return pm;
    }

    const QPixmap &CIcons::swift16()
    {
        static const QPixmap pm(":/own/icons/own/swift3D/sw3DBlue-16.png");
        return pm;
    }

    const QPixmap &CIcons::swift24()
    {
        static const QPixmap pm(":/own/icons/own/swift3D/sw3DBlue-24.png");
        return pm;
    }

    const QPixmap &CIcons::swift48()
    {
        static const QPixmap pm(":/own/icons/own/swift3D/sw3DBlue-48.png");
        return pm;
    }

    const QPixmap &CIcons::swift64()
    {
        static const QPixmap pm(":/own/icons/own/swift3D/sw3DBlue-64.png");
        return pm;
    }

    const QPixmap &CIcons::swift128()
    {
        static const QPixmap pm(":/own/icons/own/swift3D/sw3DBlue-128.png");
        return pm;
    }

    const QPixmap &CIcons::swift256()
    {
        static const QPixmap pm(":/own/icons/own/swift3D/sw3DBlue-256.png");
        return pm;
    }

    const QPixmap &CIcons::swift1024()
    {
        static const QPixmap pm(":/own/icons/own/swift3D/sw3DBlue-1024.png");
        return pm;
    }

    const QPixmap &CIcons::swiftDatabase16()
    {
        static const QPixmap pm(":/own/icons/own/swift3D/sw3DGreen-16.png");
        return pm;
    }

    const QPixmap &CIcons::swiftDatabase24()
    {
        static const QPixmap pm(":/own/icons/own/swift3D/sw3DGreen-24.png");
        return pm;
    }

    const QPixmap &CIcons::swiftDatabase48()
    {
        static const QPixmap pm(":/own/icons/own/swift3D/sw3DGreen-48.png");
        return pm;
    }

    const QPixmap &CIcons::swiftDatabase64()
    {
        static const QPixmap pm(":/own/icons/own/swift3D/sw3DGreen-64.png");
        return pm;
    }

    const QPixmap &CIcons::swiftDatabase128()
    {
        static const QPixmap pm(":/own/icons/own/swift3D/sw3DGreen-128.png");
        return pm;
    }

    const QPixmap &CIcons::swiftDatabase256()
    {
        static const QPixmap pm(":/own/icons/own/swift3D/sw3DGreen-256.png");
        return pm;
    }

    const QPixmap &CIcons::swiftDatabase1024()
    {
        static const QPixmap pm(":/own/icons/own/swift3D/sw3DGreen-1024.png");
        return pm;
    }

    const QPixmap &CIcons::swiftCore16()
    {
        static const QPixmap pm(":/own/icons/own/swift3D/sw3DOrange-16.png");
        return pm;
    }

    const QPixmap &CIcons::swiftCore24()
    {
        static const QPixmap pm(":/own/icons/own/swift3D/sw3DOrange-24.png");
        return pm;
    }

    const QPixmap &CIcons::swiftCore48()
    {
        static const QPixmap pm(":/own/icons/own/swift3D/sw3DOrange-48.png");
        return pm;
    }

    const QPixmap &CIcons::swiftCore64()
    {
        static const QPixmap pm(":/own/icons/own/swift3D/sw3DOrange-64.png");
        return pm;
    }

    const QPixmap &CIcons::swiftCore128()
    {
        static const QPixmap pm(":/own/icons/own/swift3D/sw3DOrange-128.png");
        return pm;
    }

    const QPixmap &CIcons::swiftCore256()
    {
        static const QPixmap pm(":/own/icons/own/swift3D/sw3DOrange-256.png");
        return pm;
    }

    const QPixmap &CIcons::swiftCore1024()
    {
        static const QPixmap pm(":/own/icons/own/swift3D/sw3DOrange-1024.png");
        return pm;
    }

    const QPixmap &CIcons::swiftLauncher16()
    {
        static const QPixmap pm(":/own/icons/own/swift3D/sw3DBlack-16.png");
        return pm;
    }

    const QPixmap &CIcons::swiftLauncher24()
    {
        static const QPixmap pm(":/own/icons/own/swift3D/sw3DBlack-24.png");
        return pm;
    }

    const QPixmap &CIcons::swiftLauncher48()
    {
        static const QPixmap pm(":/own/icons/own/swift3D/sw3DBlack-48.png");
        return pm;
    }

    const QPixmap &CIcons::swiftLauncher64()
    {
        static const QPixmap pm(":/own/icons/own/swift3D/sw3DBlack-64.png");
        return pm;
    }

    const QPixmap &CIcons::swiftLauncher128()
    {
        static const QPixmap pm(":/own/icons/own/swift3D/sw3DBlack-128.png");
        return pm;
    }

    const QPixmap &CIcons::swiftLauncher256()
    {
        static const QPixmap pm(":/own/icons/own/swift3D/sw3DBlack-256.png");
        return pm;
    }

    const QPixmap &CIcons::swiftLauncher1024()
    {
        static const QPixmap pm(":/own/icons/own/swift3D/sw3DBlack-1024.png");
        return pm;
    }

    const QPixmap &CIcons::swiftMap16()
    {
        static const QPixmap pm(":/own/icons/own/swift3D/sw3DYellow-16.png");
        return pm;
    }

    const QPixmap &CIcons::swiftMap24()
    {
        static const QPixmap pm(":/own/icons/own/swift3D/sw3DYellow-24.png");
        return pm;
    }

    const QPixmap &CIcons::swiftMap48()
    {
        static const QPixmap pm(":/own/icons/own/swift3D/sw3DYellow-48.png");
        return pm;
    }

    const QPixmap &CIcons::swiftMap64()
    {
        static const QPixmap pm(":/own/icons/own/swift3D/sw3DYellow-64.png");
        return pm;
    }

    const QPixmap &CIcons::swiftMap128()
    {
        static const QPixmap pm(":/own/icons/own/swift3D/sw3DYellow-128.png");
        return pm;
    }

    const QPixmap &CIcons::swiftMap256()
    {
        static const QPixmap pm(":/own/icons/own/swift3D/sw3DYellow-512.png");
        return pm;
    }

    const QPixmap &CIcons::swiftMap1024()
    {
        static const QPixmap pm(":/own/icons/own/swift3D/sw3DYellow-1024.png");
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

    const QPixmap &CIcons::appTerminal16()
    {
        static const QPixmap pm(":/pastel/icons/pastel/16/monitor-terminal.png");
        return pm;
    }

    const QPixmap &CIcons::appTextMessages16()
    {
        return text16();
    }

    const QPixmap &CIcons::appAtc16()
    {
        return building16();
    }

    const QPixmap &CIcons::appAircraft16()
    {
        return paperPlane16();
    }

    const QPixmap &CIcons::appMappings16()
    {
        return tableRelationship16();
    }

    const QPixmap &CIcons::appInterpolation16()
    {
        return paperPlane16();
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

    const QPixmap &CIcons::appDatabase16()
    {
        return database16();
    }

    const QPixmap &CIcons::appAircraftIcao16()
    {
        return paperPlane16();
    }

    const QPixmap &CIcons::appAirlineIcao16()
    {
        return paperPlane16();
    }

    const QPixmap &CIcons::appLiveries16()
    {
        return paintCan16();
    }

    const QPixmap &CIcons::appModels16()
    {
        return appAircraft16();
    }

    const QPixmap &CIcons::appCountries16()
    {
        return globe16();
    }

    const QPixmap &CIcons::appDistributors16()
    {
        return users16();
    }

    const QPixmap &CIcons::appAircraftCategories16()
    {
        return folderEdit16();
    }

    const QPixmap &CIcons::appDbStash16()
    {
        return databaseAdd16();
    }

    const QPixmap &CIcons::appRadar16()
    {
        return radar16();
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

    const QPixmap &CIcons::roleDeparture()
    {
        // currently same icons as APP
        static const QPixmap pm(QPixmap(":/own/icons/own/app.jpg").scaledToWidth(16, Qt::SmoothTransformation));
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
        return tableSheet16();
    }

    const QPixmap &CIcons::geoPosition16()
    {
        return globe16();
    }

    const QPixmap &CIcons::modelInclude16()
    {
        static const QPixmap pm(":/diagona/icons/diagona/icons/paper-plane--plus.png");
        return pm;
    }

    const QPixmap &CIcons::modelExclude16()
    {
        // Alternative: static const QPixmap pm(":/diagona/icons/diagona/icons/paper-plane--minus.png");
        return cross16();
    }

    const QPixmap &CIcons::modelConverterX()
    {
        static const QPixmap pm(":/misc/icons/misc/mcx.png");
        return pm;
    }

    const QPixmap &CIcons::pixmapByIndex(CIcons::IconIndex index)
    {
        switch (index)
        {
        case ApplicationAircraft: return appAircraft16();
        case ApplicationAircraftCategories: return appAircraftCategories16();
        case ApplicationAircraftIcao: return appAircraftIcao16();
        case ApplicationAirlineIcao: return appAirlineIcao16();
        case ApplicationAtc: return appAtc16();
        case ApplicationAudio: return appAudio16();
        case ApplicationCockpit: return appCockpit16();
        case ApplicationCountries: return appCountries16();
        case ApplicationDatabase: return appDatabase16();
        case ApplicationDbStash: return appDbStash16();
        case ApplicationDistributors: return appDistributors16();
        case ApplicationFlightPlan: return appFlightPlan16();
        case ApplicationLiveries: return appLiveries16();
        case ApplicationLog: return appLog16();
        case ApplicationMappings: return appMappings16();
        case ApplicationModels: return appModels16();
        case ApplicationSettings: return appSettings16();
        case ApplicationSimulator: return appSimulator16();
        case ApplicationTextMessages: return appTextMessages16();
        case ApplicationWeather: return appWeather16();
        case ApplicationRadar: return appRadar16();
        case AviationAtis: return atis();
        case AviationAttitudeIndicator: return attitudeIndicator16();
        case AviationMetar: return metar();
        case GeoPosition: return geoPosition16();
        case ModelExclude: return modelExclude16();
        case ModelInclude: return modelInclude16();
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
        case NetworkRoleDeparture: return roleDeparture();
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
        case OSAll: return osAll();
        case OSLinux: return osLinux();
        case OSMacOs: return osMac();
        case OSWindows: return osWindows();
        case SimulatorFG: return simulatorFG();
        case SimulatorFG16: return simulatorFG16();
        case SimulatorFS9: return simulatorFS9();
        case SimulatorFS916: return simulatorFS916();
        case SimulatorFSX: return simulatorFSX();
        case SimulatorFSX16: return simulatorFSX16();
        case SimulatorP3D: return simulatorP3D();
        case SimulatorP3D16: return simulatorP3D16();
        case SimulatorXPlane: return simulatorXPlane();
        case SimulatorXPlane16: return simulatorXPlane16();
        case StandardIconAppAircraft16: return appAircraft16();
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
        case StandardIconColor16: return color16();
        case StandardIconColorPicker16: return colorPicker16();
        case StandardIconColorSwatch16: return colorSwatch16();
        case StandardIconCopy16: return copy16();
        case StandardIconCross16: return cross16();
        case StandardIconCrossCircle16: return crossCircle16();
        case StandardIconCrossSmall16: return crossSmall16();
        case StandardIconCrossWhite16: return crossWhite16();
        case StandardIconCut16: return cut16();
        case StandardIconDatabase16: return database16();
        case StandardIconDatabaseAdd16: return databaseAdd16();
        case StandardIconDatabaseConnect16: return databaseConnect16();
        case StandardIconDatabaseDelete16: return databaseDelete16();
        case StandardIconDatabaseEdit16: return databaseEdit16();
        case StandardIconDatabaseError16: return databaseError16();
        case StandardIconDatabaseKey16: return databaseKey16();
        case StandardIconDatabaseTable16: return databaseTable16();
        case StandardIconDelete16: return delete16();
        case StandardIconDockBottom16: return dockBottom16();
        case StandardIconDockTop16: return dockTop16();
        case StandardIconDragAndDrop16: return dragAndDrop16();
        case StandardIconEmpty16: return empty16();
        case StandardIconEmpty: return empty();
        case StandardIconError16: return error16();
        case StandardIconFloatAll16: return floatAll16();
        case StandardIconFloatOne16: return floatOne16();
        case StandardIconFont16: return font16();
        case StandardIconGlobe16: return globe16();
        case StandardIconHeadingOne16: return headingOne16();
        case StandardIconInfo16: return info16();
        case StandardIconJoystick16: return joystick16();
        case StandardIconLoad16: return load16();
        case StandardIconLockClosed16: return lockClosed16();
        case StandardIconLockOpen16: return lockOpen16();
        case StandardIconMonitorError16: return monitorError16();
        case StandardIconPaintCan16: return paintCan16();
        case StandardIconPaperPlane16: return paperPlane16();
        case StandardIconPaste16: return paste16();
        case StandardIconPlugin16: return plugin16();
        case StandardIconRadar16: return radar16();
        case StandardIconRadio16: return radio16();
        case StandardIconRefresh16: return refresh16();
        case StandardIconResize16: return resize16();
        case StandardIconResizeHorizontal16: return resizeHorizontal16();
        case StandardIconResizeVertical16: return resizeVertical16();
        case StandardIconSave16: return save16();
        case StandardIconSpeakerNetwork16: return speakerNetwork16();
        case StandardIconStatusBar16: return statusBar16();
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
        case StandardIconBuilding16: return building16();
        case Swift1024: return swift1024();
        case Swift128: return swift128();
        case Swift16: return swift16();
        case Swift24: return swift24();
        case Swift256: return swift256();
        case Swift48: return swift48();
        case Swift64: return swift64();
        case SwiftCore1024: return swiftCore1024();
        case SwiftCore128: return swiftCore128();
        case SwiftCore16: return swiftCore16();
        case SwiftCore24: return swiftCore24();
        case SwiftCore256: return swiftCore256();
        case SwiftCore48: return swiftCore48();
        case SwiftCore64: return swiftCore64();
        case SwiftDatabase1024: return swiftDatabase1024();
        case SwiftDatabase128: return swiftDatabase128();
        case SwiftDatabase16: return swiftDatabase16();
        case SwiftDatabase24: return swiftDatabase24();
        case SwiftDatabase256: return swiftDatabase256();
        case SwiftDatabase48: return swiftDatabase48();
        case SwiftDatabase64: return swiftDatabase64();
        case SwiftLauncher1024: return swiftLauncher1024();
        case SwiftLauncher128: return swiftLauncher128();
        case SwiftLauncher16: return swiftLauncher16();
        case SwiftLauncher24: return swiftLauncher24();
        case SwiftLauncher256: return swiftLauncher256();
        case SwiftLauncher48: return swiftLauncher48();
        case SwiftLauncher64: return swiftLauncher64();
        default: return empty();
        }
    }

    QPixmap CIcons::pixmapByIndex(CIcons::IconIndex index, int rotateDegrees)
    {
        if (rotateDegrees == 0) { return pixmapByIndex(index); }
        return rotate(rotateDegrees, pixmapByIndex(index));
    }

    const QPixmap &CIcons::pixmapByResourceFileName(const QString &relativeFileName, QString &fullFilePath)
    {
        //! \fixme KB 20170701 noticed the "cache" is not threadsafe. However, there has never be an issue so far. Added thread assert.
        Q_ASSERT_X(!relativeFileName.isEmpty(), Q_FUNC_INFO, "missing filename");
        Q_ASSERT_X(CThreadUtils::isCurrentThreadApplicationThread(), Q_FUNC_INFO, "not thread safe");

        fullFilePath = CFileUtils::appendFilePaths(CDirectoryUtils::imagesDirectory(), relativeFileName);
        if (!getResourceFileCache().contains(relativeFileName))
        {
            QPixmap pm;
            const bool s = pm.load(fullFilePath);
            CIcons::getResourceFileCache().insert(relativeFileName, s ? pm : CIcons::empty());
        }
        return CIcons::getResourceFileCache()[relativeFileName];
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

    QPixmap CIcons::rotate(int rotateDegrees, const QPixmap &original)
    {
        QMatrix rm;
        rm.rotate(rotateDegrees);
        QPixmap rotated = original.transformed(rm);
        int xoffset = (rotated.width() - original.width()) / 2;
        int yoffset = (rotated.height() - original.height()) / 2;
        rotated = rotated.copy(xoffset, yoffset, original.width(), original.height());
        return rotated;
    }

    QMap<QString, QPixmap> &CIcons::getResourceFileCache()
    {
        static QMap<QString, QPixmap> cache;
        return cache;
    }

    QPixmap CIcons::changeResourceBackgroundColor(const QString &resource, Qt::GlobalColor backgroundColor)
    {
        QImage imgSource(resource);
        QImage destBackgroundImg(changeImageBackgroundColor(imgSource, backgroundColor));
        return QPixmap::fromImage(destBackgroundImg);
    }

    QIcon CIcons::changeIconBackgroundColor(const QIcon &icon, Qt::GlobalColor backgroundColor, QSize targetsize)
    {
        QImage imgSource(icon.pixmap(targetsize).toImage());
        QImage destBackgroundImg(changeImageBackgroundColor(imgSource, backgroundColor));
        return QIcon(QPixmap::fromImage(destBackgroundImg));
    }
} // namespace

//! \endcond
