// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "joysticklinux.h"

#include <fcntl.h>
#include <linux/joystick.h>
#include <unistd.h>

#include <QDir>
#include <QFile>
#include <QFileSystemWatcher>
#include <QSignalMapper>
#include <QSocketNotifier>

#include "misc/logmessage.h"

using namespace swift::misc;
using namespace swift::misc::input;

namespace
{
    inline QString inputDevicesDir()
    {
        return QStringLiteral("/dev/input/");
    }
} // namespace

namespace swift::input
{
    CJoystickDevice::CJoystickDevice(const QString &path, QFile *fd, QObject *parent)
        : QObject(parent), m_path(path), m_fd(fd)
    {
        m_fd->setParent(this);
        char deviceName[256];
        if (ioctl(m_fd->handle(), JSIOCGNAME(sizeof(deviceName)), deviceName) < 0)
        {
            strncpy(deviceName, "Unknown", sizeof(deviceName));
        }

        CLogMessage(this).info(u"Found joystick: %1") << deviceName;

        fcntl(m_fd->handle(), F_SETFL, O_NONBLOCK);

        /* Forward */
        struct js_event event;
        while (m_fd->read(reinterpret_cast<char *>(&event), sizeof(event)) == sizeof(event)) {}
        QSocketNotifier *notifier = new QSocketNotifier(m_fd->handle(), QSocketNotifier::Read, m_fd);
        connect(notifier, &QSocketNotifier::activated, this, &CJoystickDevice::processInput);
        m_name = QString(deviceName);
    }

    CJoystickDevice::~CJoystickDevice()
    {
        if (m_fd)
        {
            m_fd->close();
            m_fd->deleteLater();
        }
    }

    void CJoystickDevice::processInput()
    {
        struct js_event event;
        while (m_fd->read(reinterpret_cast<char *>(&event), sizeof(event)) == sizeof(event))
        {
            switch (event.type & ~JS_EVENT_INIT)
            {
            case JS_EVENT_BUTTON:
                if (event.value) { emit buttonChanged(m_name, event.number, true); }
                else { emit buttonChanged(m_name, event.number, false); }
                break;
            }
        }
    }

    CJoystickLinux::CJoystickLinux(QObject *parent) : IJoystick(parent),
                                                      m_inputWatcher(new QFileSystemWatcher(this))
    {
        m_inputWatcher->addPath(inputDevicesDir());
        connect(m_inputWatcher, &QFileSystemWatcher::directoryChanged, this, &CJoystickLinux::reloadDevices);
        reloadDevices(inputDevicesDir());
    }

    CJoystickButtonList CJoystickLinux::getAllAvailableJoystickButtons() const
    {
        // We don't know which buttons are available yet.
        return {};
    }

    void CJoystickLinux::cleanupJoysticks()
    {
        for (auto it = m_joystickDevices.begin(); it != m_joystickDevices.end();)
        {
            // Remove all joysticks that do not exist anymore (/dev/input/js* removed).
            if (!(*it)->isAttached())
            {
                CJoystickDevice *joystickDevice = *it;
                it = m_joystickDevices.erase(it);
                joystickDevice->deleteLater();
            }
            else
            {
                ++it;
            }
        }
    }

    void CJoystickLinux::addJoystickDevice(const QString &path)
    {
        QFile *fd = new QFile(path);
        if (fd->open(QIODevice::ReadOnly))
        {
            CJoystickDevice *joystickDevice = new CJoystickDevice(path, fd, this);
            connect(joystickDevice, &CJoystickDevice::buttonChanged, this, &CJoystickLinux::joystickButtonChanged);
            m_joystickDevices.push_back(joystickDevice);
        }
        else
        {
            swift::misc::CLogMessage(this).error(u"Failed to open joystick device %1: %2") << fd->fileName() << fd->errorString();
            fd->close();
            fd->deleteLater();
        }
    }

    void CJoystickLinux::joystickButtonChanged(const QString &name, int index, bool isPressed)
    {
        swift::misc::input::CHotkeyCombination oldCombination(m_buttonCombination);
        if (isPressed)
        {
            m_buttonCombination.addJoystickButton({ name, index });
        }
        else
        {
            m_buttonCombination.removeJoystickButton({ name, index });
        }

        if (oldCombination != m_buttonCombination)
        {
            emit buttonCombinationChanged(m_buttonCombination);
        }
    }

    void CJoystickLinux::reloadDevices(QString path)
    {
        cleanupJoysticks();

        QDir dir(path, QLatin1String("js*"), QDir::Name, QDir::System);
        for (const auto &entry : dir.entryInfoList())
        {
            QString f = entry.absoluteFilePath();
            auto it = std::find_if(m_joystickDevices.begin(), m_joystickDevices.end(), [path](const CJoystickDevice *device) {
                return device->getPath() == path;
            });
            if (it == m_joystickDevices.end())
            {
                addJoystickDevice(f);
            }
        }
    }

} // namespace swift::input
