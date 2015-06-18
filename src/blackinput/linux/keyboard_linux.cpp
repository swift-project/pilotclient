/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "keyboard_linux.h"
#include "keymapping_linux.h"
#include "blackmisc/logmessage.h"
#include <QDebug>
#include <QFileSystemWatcher>
#include <QDir>
#include <QFile>
#include <QSocketNotifier>
#include <linux/input.h>
#include <fcntl.h>

using namespace BlackMisc::Hardware;

namespace BlackInput
{
    CKeyboardLinux::CKeyboardLinux(QObject *parent) :
        IKeyboard(parent),
        m_ignoreNextKey(false),
        m_mode(Mode_Nominal)
    {
    }

    CKeyboardLinux::~CKeyboardLinux()
    {
    }

    bool CKeyboardLinux::init()
    {
        QString dir = QLatin1String("/dev/input");
        m_devInputWatcher = new QFileSystemWatcher(QStringList(dir), this);
        connect(m_devInputWatcher, &QFileSystemWatcher::directoryChanged, this, &CKeyboardLinux::deviceDirectoryChanged);
        deviceDirectoryChanged(dir);

        return true;
    }

    void CKeyboardLinux::setKeysToMonitor(const CKeyboardKeyList &keylist)
    {
        m_listMonitoredKeys = keylist;
    }

    void CKeyboardLinux::startCapture(bool ignoreNextKey)
    {
        m_mode = Mode_Capture;
        m_ignoreNextKey = ignoreNextKey;
        m_pressedKey.setKeyObject(CKeyboardKey());
    }

    void CKeyboardLinux::triggerKey(const CKeyboardKey &key, bool isPressed)
    {
        if(!isPressed) emit keyUp(key);
        else emit keyDown(key);
    }

    void CKeyboardLinux::deviceDirectoryChanged(const QString &dir)
    {
        QDir eventFiles(dir, QLatin1String("event*"), 0, QDir::System);

        foreach (QFileInfo fileInfo, eventFiles.entryInfoList())
        {
            QString path = fileInfo.absoluteFilePath();
            if(!m_keyboardDevices.contains(path) )
                addRawInputDevice(path);
        }
    }

    void CKeyboardLinux::inputReadyRead(int)
    {
        struct input_event eventInput;

        QFile *fileInput=qobject_cast<QFile *>(sender()->parent());
        if (!fileInput)
            return;

        bool found = false;

        while (fileInput->read(reinterpret_cast<char *>(&eventInput), sizeof(eventInput)) == sizeof(eventInput))
        {
            found = true;
            if (eventInput.type != EV_KEY)
                continue;
            bool isPressed = false;
            switch (eventInput.value)
            {
                case 0:
                    isPressed = false;
                    break;
                case 1:
                    isPressed = true;
                    break;
                default:
                    continue;
            }
            int keyCode = eventInput.code;
            keyEvent(keyCode, isPressed);
        }

        if (!found) {
            int fd = fileInput->handle();
            int version = 0;
            if ((ioctl(fd, EVIOCGVERSION, &version) < 0) || (((version >> 16) & 0xFF) < 1)) {
                qWarning("CKeyboardLinux: Removing dead input device %s", qPrintable(fileInput->fileName()));
                m_keyboardDevices.remove(fileInput->fileName());
            }
        }
    }

    void CKeyboardLinux::sendCaptureNotification(const CKeyboardKey &key, bool isFinished)
    {
        if (isFinished)
            emit keySelectionFinished(key);
        else
            emit keySelectionChanged(key);
    }

    void CKeyboardLinux::addRawInputDevice(const QString &filePath)
    {
        QSharedPointer<QFile> inputFile(new QFile(filePath));
        if (inputFile->open(QIODevice::ReadOnly))
        {
            int fd = inputFile->handle();
            if (fd < 0) { return; }

            int version = 0;
            if (ioctl(fd, EVIOCGVERSION, &version) < 0) { return; }

            char deviceName[255];
            if (ioctl(fd, EVIOCGNAME(sizeof(deviceName)), deviceName) < 0) { return; }

            uint8_t bitmask[EV_MAX/8 + 1];
            memset(bitmask, 0, sizeof(bitmask));
            if (ioctl(fd, EVIOCGBIT(0,sizeof(bitmask)), &bitmask) < 0) { return; }

            // Keyboards support EV_SYN and EV_KEY
            // but do NOT support EV_REL and EV_ABS
            if (!(bitmask[EV_SYN / 8] & (1 << (EV_SYN % 8))) &&
                !(bitmask[EV_KEY / 8] & (1 << (EV_KEY % 8))) &&
                (bitmask[EV_REL / 8] & (1 << (EV_REL % 8))) &&
                (bitmask[EV_ABS / 8] & (1 << (EV_ABS % 8))))
            {
                return;
            }

            // Is it grabbed by someone else?
            if ((ioctl(fd, EVIOCGRAB, 1) < 0))
            {
                BlackMisc::CLogMessage(this).warning("Device exclusively grabbed by someone else (X11 using exclusive-mode evdev?)") << deviceName;
            }
            else
            {
                ioctl(fd, EVIOCGRAB, 0);
                uint8_t keys[KEY_MAX/8 + 1];
                if ((ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keys)), &keys) >= 0) && (keys[KEY_SPACE / 8] & (1 << (KEY_SPACE % 8))))
                {
                    BlackMisc::CLogMessage(this).info("Found keyboard: %1") << deviceName;

                    fcntl(inputFile->handle(), F_SETFL, O_NONBLOCK);
                    connect(new QSocketNotifier(inputFile->handle(), QSocketNotifier::Read, inputFile.data()), &QSocketNotifier::activated, this, &CKeyboardLinux::inputReadyRead);

                    m_keyboardDevices.insert(filePath, inputFile);
                }
            }
        }
    }

    void CKeyboardLinux::keyEvent(int virtualKeyCode, bool isPressed)
    {
        if (CKeyMappingLinux::isMouseButton(virtualKeyCode))
            return;

        BlackMisc::Hardware::CKeyboardKey lastPressedKey = m_pressedKey;
        if (m_ignoreNextKey)
        {
            m_ignoreNextKey = false;
            return;
        }

        bool isFinished = false;
        if (isPressed)
        {
            if (CKeyMappingLinux::isModifier(virtualKeyCode))
                m_pressedKey.addModifier(CKeyMappingLinux::convertToModifier(virtualKeyCode));
            else
            {
                m_pressedKey.setKey(CKeyMappingLinux::convertToKey(virtualKeyCode));
            }
        }
        else
        {
            if (CKeyMappingLinux::isModifier(virtualKeyCode))
                m_pressedKey.removeModifier(CKeyMappingLinux::convertToModifier(virtualKeyCode));
            else
            {
                m_pressedKey.setKey(Qt::Key_unknown);
            }

            isFinished = true;
        }

        if (lastPressedKey == m_pressedKey)
            return;

#ifdef DEBUG_KEYBOARD
        qDebug() << "Virtual key: " << virtualKeyCode;
#endif
        if (m_mode == Mode_Capture)
        {
            if (isFinished)
            {
                sendCaptureNotification(lastPressedKey, true);
                m_mode = Mode_Nominal;
            }
            else
            {
                sendCaptureNotification(m_pressedKey, false);
            }
        }
        else
        {
            if (m_listMonitoredKeys.contains(lastPressedKey)) emit keyUp(lastPressedKey);
            if (m_listMonitoredKeys.contains(m_pressedKey)) emit keyDown(m_pressedKey);
        }
    }
}
