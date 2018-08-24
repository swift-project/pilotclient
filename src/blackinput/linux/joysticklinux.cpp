/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/logmessage.h"
#include "joysticklinux.h"
#include <QFile>
#include <QFileSystemWatcher>
#include <QSocketNotifier>
#include <QSignalMapper>
#include <QDir>
#include <linux/joystick.h>
#include <unistd.h>
#include <fcntl.h>

using namespace BlackMisc;
using namespace BlackMisc::Input;

namespace
{
    inline QString inputDevicesDir()
    {
        return QStringLiteral("/dev/input/");
    }
}

namespace BlackInput
{
    CJoystickLinux::CJoystickLinux(QObject *parent) :
        IJoystick(parent),
        m_mapper(new QSignalMapper(this)),
        m_inputWatcher(new QFileSystemWatcher(this))
    {
        connect(m_mapper, static_cast<void (QSignalMapper::*)(QObject *)>(&QSignalMapper::mapped), this, &CJoystickLinux::ps_readInput);

        m_inputWatcher->addPath(inputDevicesDir());
        connect(m_inputWatcher, &QFileSystemWatcher::directoryChanged, this, &CJoystickLinux::ps_directoryChanged);
        ps_directoryChanged(inputDevicesDir());
    }

    void CJoystickLinux::cleanupJoysticks()
    {
        for (auto it = m_joysticks.begin(); it != m_joysticks.end();)
        {
            if (!it.value()->exists())
            {
                it.value()->deleteLater();
                it = m_joysticks.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    void CJoystickLinux::addJoystickDevice(const QString &path)
    {
        Q_ASSERT(!m_joysticks.contains(path));

        QFile *joystick = new QFile(path, this);
        if (joystick->open(QIODevice::ReadOnly))
        {
            char name[256];
            if (ioctl(joystick->handle(), JSIOCGNAME(sizeof(name)), name) < 0)
            {
                strncpy(name, "Unknown", sizeof(name));
            }

            CLogMessage(this).info("Found joystick: %1") << name;

            fcntl(joystick->handle(), F_SETFL, O_NONBLOCK);

            /* Forward */
            struct js_event event;
            while (joystick->read(reinterpret_cast<char *>(&event), sizeof(event)) == sizeof(event)) {}

            QSocketNotifier *notifier = new QSocketNotifier(joystick->handle(), QSocketNotifier::Read, joystick);
            m_mapper->setMapping(notifier, joystick);
            connect(notifier, &QSocketNotifier::activated, m_mapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
            notifier->setEnabled(true);

            m_joysticks.insert(path, joystick);
        }
        else
        {
            joystick->deleteLater();
        }
    }

    void CJoystickLinux::ps_directoryChanged(QString path)
    {
        cleanupJoysticks();

        QDir dir(path, QLatin1String("js*"), QDir::Name, QDir::System);
        for (const auto &entry : dir.entryInfoList())
        {
            QString f = entry.absoluteFilePath();
            if (!m_joysticks.contains(f))
            {
                addJoystickDevice(f);
            }
        }
    }

    void CJoystickLinux::ps_readInput(QObject *object)
    {
        QFile *joystick = qobject_cast<QFile *>(object);
        Q_ASSERT(joystick);


        struct js_event event;
        while (joystick->read(reinterpret_cast<char *>(&event), sizeof(event)) == sizeof(event))
        {
            BlackMisc::Input::CHotkeyCombination oldCombination(m_buttonCombination);
            switch (event.type & ~JS_EVENT_INIT)
            {
            case JS_EVENT_BUTTON:
                if (event.value)
                {
                    m_buttonCombination.addJoystickButton(event.number);
                }
                else
                {
                    m_buttonCombination.removeJoystickButton(event.number);
                }

                if (oldCombination != m_buttonCombination)
                {
                    emit buttonCombinationChanged(m_buttonCombination);
                }
                break;
            }
        }
    }
} // ns
