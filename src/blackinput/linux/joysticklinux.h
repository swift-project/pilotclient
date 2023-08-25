// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef BLACKINPUT_JOYSTICKLINUX_H
#define BLACKINPUT_JOYSTICKLINUX_H

//! \file

#include "blackinput/joystick.h"
#include "blackmisc/input/joystickbutton.h"
#include "blackmisc/collection.h"
#include <QMap>
#include <QString>

class QFile;
class QFileSystemWatcher;
class QSignalMapper;

namespace BlackInput
{
    //! Linux Joystick device
    class CJoystickDevice : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor
        CJoystickDevice(const QString &path, QFile *fd, QObject *parent);
        ~CJoystickDevice();

        //! Get device name
        QString getName() const { return m_name; }

        //! Get device path, e.g. /dev/input/js0
        QString getPath() const { return m_path; }

        //! Is joystick device still attached?
        bool isAttached() const { return m_fd->exists(); }

    signals:
        //! Joystick button changed
        void buttonChanged(const QString &name, int index, bool isPressed);

    private:
        //! Slot for reading the device handle
        void processInput();

        QString m_name; //!< Device name
        QString m_path; //!< Device path, e.g. /dev/input/js0
        QFile *m_fd = nullptr; //!< Linux file descriptor
    };

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

        //! \copydoc BlackInput::IJoystick::getAllAvailableJoystickButtons()
        virtual BlackMisc::Input::CJoystickButtonList getAllAvailableJoystickButtons() const override;

    private:
        friend class IJoystick;

        //! Removes all joysticks that are no longer present.
        void cleanupJoysticks();

        //! Adds new joystick input for reading
        void addJoystickDevice(const QString &path);

        void joystickButtonChanged(const QString &name, int index, bool isPressed);

        //! Constructor
        CJoystickLinux(QObject *parent = nullptr);

        //! Slot for handling directory changes
        //! \param path Watched directory path.
        void reloadDevices(QString path);

    private:
        BlackMisc::Input::CHotkeyCombination m_buttonCombination;
        QVector<CJoystickDevice *> m_joystickDevices; //!< All joystick devices
        QFileSystemWatcher *m_inputWatcher = nullptr;
    };

} // namespace BlackInput

#endif // BLACKINPUT_JOYSTICKLINUX_H
