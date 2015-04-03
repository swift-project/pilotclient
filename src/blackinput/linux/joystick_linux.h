/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKINPUT_JOYSTICKLINUX_H
#define BLACKINPUT_JOYSTICKLINUX_H

//! \file

#include "blackinput/joystick.h"
#include "blackmisc/hardware/joystickbutton.h"
#include "blackmisc/collection.h"
#include <QMap>
#include <QString>

class QFile;
class QFileSystemWatcher;
class QSignalMapper;

namespace BlackInput
{
    //! Linux implemenation of IJoystick
    //! \sa https://www.kernel.org/doc/Documentation/input/joystick-api.txt
    class CJoystickLinux : public IJoystick
    {
        Q_OBJECT

    public:

        //! Copy Constructor
        CJoystickLinux(CJoystickLinux const &) = delete;

        //! Assignment operator
        CJoystickLinux &operator=(CJoystickLinux const &) = delete;

        //! \brief Destructor
        virtual ~CJoystickLinux() = default;

        //! \copydoc IJoystick::startCapture()
        virtual void startCapture() override;

        //! \copydoc IJoystick::triggerButton()
        virtual void triggerButton(const BlackMisc::Hardware::CJoystickButton button, bool isPressed) override;

    private:

        friend class IJoystick;

        //! Removes all joysticks that are no longer present.
        void cleanupJoysticks();

        //! Adds new joystick input for reading
        void addJoystickDevice(const QString &path);

        //! Constructor
        CJoystickLinux(QObject *parent = nullptr);

    private slots:

        //! Slot for handling directory changes
        //! \param path Watched directory path.
        void ps_directoryChanged(QString path);

        //! Slot for reading the device handle
        //! \param object QFile that has data to be read.
        void ps_readInput(QObject *object);

    private:

        IJoystick::Mode m_mode = ModeNominal; //!< Current working mode
        QSignalMapper *m_mapper = nullptr; //!< Maps device handles
        QMap<QString, QFile *> m_joysticks; //!< All read joysticks, file path <-> file instance pairs
        QFileSystemWatcher *m_inputWatcher = nullptr;
    };

} // namespace BlackInput

#endif // BLACKINPUT_JOYSTICKLINUX_H
