// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef SWIFT_INPUT_JOYSTICKLINUX_H
#define SWIFT_INPUT_JOYSTICKLINUX_H

//! \file

#include "input/joystick.h"
#include "misc/input/joystickbutton.h"
#include "misc/collection.h"
#include <QMap>
#include <QString>

class QFile;
class QFileSystemWatcher;
class QSignalMapper;

namespace swift::input
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

        //! \copydoc swift::input::IJoystick::getAllAvailableJoystickButtons()
        virtual swift::misc::input::CJoystickButtonList getAllAvailableJoystickButtons() const override;

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
        swift::misc::input::CHotkeyCombination m_buttonCombination;
        QVector<CJoystickDevice *> m_joystickDevices; //!< All joystick devices
        QFileSystemWatcher *m_inputWatcher = nullptr;
    };

} // namespace swift::input

#endif // SWIFT_INPUT_JOYSTICKLINUX_H
