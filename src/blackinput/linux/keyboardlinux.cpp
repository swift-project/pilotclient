/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "keyboardlinux.h"
#include "blackmisc/logmessage.h"
#include <QDebug>
#include <QHash>
#include <QSocketNotifier>
#include <linux/input.h>
#include <fcntl.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>

using namespace BlackMisc;
using namespace BlackMisc::Input;

namespace BlackInput
{
    // https://www.cl.cam.ac.uk/~mgk25/ucs/keysymdef.h
    static QHash<int, Input::KeyCode> keyMapping
    {
        { XK_0, Key_0 },
        { XK_1, Key_1 },
        { XK_2, Key_2 },
        { XK_3, Key_3 },
        { XK_4, Key_4 },
        { XK_5, Key_5 },
        { XK_6, Key_6 },
        { XK_7, Key_7 },
        { XK_8, Key_8 },
        { XK_9, Key_9 },
        { XK_a, Key_A },
        { XK_b, Key_B },
        { XK_c, Key_C },
        { XK_d, Key_D },
        { XK_e, Key_E },
        { XK_f, Key_F },
        { XK_g, Key_G },
        { XK_h, Key_H },
        { XK_i, Key_I },
        { XK_j, Key_J },
        { XK_k, Key_K },
        { XK_l, Key_L },
        { XK_m, Key_M },
        { XK_n, Key_N },
        { XK_o, Key_O },
        { XK_p, Key_P },
        { XK_q, Key_Q },
        { XK_r, Key_R },
        { XK_s, Key_S },
        { XK_t, Key_T },
        { XK_u, Key_U },
        { XK_v, Key_V },
        { XK_w, Key_W },
        { XK_x, Key_X },
        { XK_y, Key_Y },
        { XK_z, Key_Z },
        { XK_Shift_L, Key_ShiftLeft },
        { XK_Shift_R, Key_ShiftRight },
        { XK_Control_L, Key_ControlLeft },
        { XK_Control_R, Key_ControlRight },
        { XK_Alt_L, Key_AltLeft },
        { XK_Alt_R, Key_AltRight },
        { XK_KP_Add, Key_Plus },
        { XK_plus, Key_Plus },
        { XK_KP_Subtract, Key_Minus },
        { XK_minus, Key_Minus },
        { XK_period, Key_Period },
        { XK_KP_Divide, Key_Divide },
        { XK_KP_Multiply, Key_Multiply },
        { XK_BackSpace, Key_Back },
        { XK_Tab, Key_Tab },
        { XK_Escape, Key_Esc },
        { XK_space, Key_Space },
        { XK_dead_grave, Key_DeadGrave },
        { XK_comma, Key_Comma }

        /** fixme Missing ones
        Key_Insert,
        Key_Delete,
        CKeyboardKey(Key_NumpadEqual),
        CKeyboardKey(Key_OEM1),
        CKeyboardKey(Key_OEM2),
        CKeyboardKey(Key_OEM3),
        CKeyboardKey(Key_OEM4),
        CKeyboardKey(Key_OEM5),
        CKeyboardKey(Key_OEM6),
        CKeyboardKey(Key_OEM7),
        CKeyboardKey(Key_OEM8),
        CKeyboardKey(Key_OEM102)
        **/
    };

    CKeyboardLinux::CKeyboardLinux(QObject *parent) :
        IKeyboard(parent)
    {
        m_display = XOpenDisplay(nullptr);
    }

    CKeyboardLinux::~CKeyboardLinux()
    {
        if (m_display) XCloseDisplay(m_display);
    }

    bool CKeyboardLinux::init()
    {
        QString dir = QLatin1String("/dev/input");
        m_devInputWatcher = new QFileSystemWatcher(QStringList(dir), this);
        connect(m_devInputWatcher, &QFileSystemWatcher::directoryChanged, this, &CKeyboardLinux::deviceDirectoryChanged);
        deviceDirectoryChanged(dir);

        return true;
    }

    void CKeyboardLinux::deviceDirectoryChanged(const QString &dir)
    {
        QDir eventFiles(dir, QLatin1String("event*"), QDir::Name, QDir::System);

        foreach (QFileInfo fileInfo, eventFiles.entryInfoList())
        {
            QString path = fileInfo.absoluteFilePath();
            if (!m_keyboardDevices.contains(path))
                addRawInputDevice(path);
        }
    }

    void CKeyboardLinux::inputReadyRead(int)
    {
        struct input_event eventInput;

        QFile *fileInput = qobject_cast<QFile *>(sender()->parent());
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

            // The + 8 offset is required for XkbKeycodeToKeysym to output the correct Keysym
            int keyCode = eventInput.code + 8;
            keyEvent(keyCode, isPressed);
        }

        if (!found)
        {
            int fd = fileInput->handle();
            int version = 0;
            if ((ioctl(fd, EVIOCGVERSION, &version) < 0) || (((version >> 16) & 0xFF) < 1))
            {
                qWarning("CKeyboardLinux: Removing dead input device %s", qPrintable(fileInput->fileName()));
                m_keyboardDevices.remove(fileInput->fileName());
            }
        }
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

            uint8_t bitmask[EV_MAX / 8 + 1];
            memset(bitmask, 0, sizeof(bitmask));
            if (ioctl(fd, EVIOCGBIT(0, sizeof(bitmask)), &bitmask) < 0) { return; }

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
                BlackMisc::CLogMessage(this).warning(u"Device exclusively grabbed by someone else (X11 using exclusive-mode evdev?)") << deviceName;
            }
            else
            {
                ioctl(fd, EVIOCGRAB, 0);
                uint8_t keys[KEY_MAX / 8 + 1];
                if ((ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keys)), &keys) >= 0) && (keys[KEY_SPACE / 8] & (1 << (KEY_SPACE % 8))))
                {
                    BlackMisc::CLogMessage(this).info(u"Found keyboard: %1") << deviceName;

                    fcntl(inputFile->handle(), F_SETFL, O_NONBLOCK);
                    connect(new QSocketNotifier(inputFile->handle(), QSocketNotifier::Read, inputFile.data()), &QSocketNotifier::activated, this, &CKeyboardLinux::inputReadyRead);

                    m_keyboardDevices.insert(filePath, inputFile);
                }
            }
        }
    }

    void CKeyboardLinux::keyEvent(int keyCode, bool isPressed)
    {
        if (isMouseButton(keyCode)) { return; }

        BlackMisc::Input::CHotkeyCombination oldCombination(m_keyCombination);
        if (isPressed)
        {
            auto key = convertToKey(keyCode);
            if (key == Key_Unknown) { return; }

            m_keyCombination.addKeyboardKey(key);
        }
        else
        {
            auto key = convertToKey(keyCode);
            if (key == Key_Unknown) { return; }

            m_keyCombination.removeKeyboardKey(key);
        }

        if (oldCombination != m_keyCombination)
        {
            emit keyCombinationChanged(m_keyCombination);
        }
    }

    BlackMisc::Input::KeyCode CKeyboardLinux::convertToKey(int keyCode)
    {
        // The keycode received from kernel does not take keyboard layouts into account.
        // It always defaults to US keyboards. In contrast to kernel devices, X11 is aware
        // of user keyboard layouts. The magic below translates the key code
        // into the correct symbol via a X11 connection.
        // Summary of translations:
        // Kernel key code -> X11 key symbol -> swift key code

        auto keySym = XkbKeycodeToKeysym(m_display, keyCode, 0, 0);
        return keyMapping.value(keySym, Key_Unknown);
    }

    bool CKeyboardLinux::isModifier(int keyCode)
    {
        auto keySym = XkbKeycodeToKeysym(m_display, keyCode, 0, 0);
        switch (keySym)
        {
        case XK_Shift_L:
        case XK_Shift_R:
        case XK_Control_L:
        case XK_Control_R:
        case XK_Alt_L:
        case XK_Alt_R:
            return true;
        default: return false;
        }

        return false;
    }

    bool CKeyboardLinux::isMouseButton(int keyCode)
    {
        switch (keyCode)
        {
        case BTN_LEFT:
        case BTN_RIGHT:
        case BTN_MIDDLE:
            return true;
        default:
            return false;
        }
    }
}
