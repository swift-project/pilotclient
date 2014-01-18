/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "voice_vatlib.h"
#include <QDebug>

using namespace BlackMisc::Voice;

namespace BlackCore
{

    CVoiceVatlib *CVoiceVatlib::CKeyboard::s_voice = nullptr;

#if defined(Q_OS_WIN)

    HHOOK CVoiceVatlib::CKeyboard::s_keyboardHook = nullptr;

    /*
     * Keyboard handling itself
     */
    LRESULT CALLBACK CVoiceVatlib::CKeyboard::keyboardProcedure(int nCode, WPARAM wParam, LPARAM lParam)
    {
        if (nCode < 0)  // do not process message
            return CallNextHookEx(CVoiceVatlib::CKeyboard::s_keyboardHook, nCode, wParam, lParam);

        // precheck
        if (!CVoiceVatlib::CKeyboard::s_keyboardHook || !CVoiceVatlib::CKeyboard::s_voice)
            return CallNextHookEx(CVoiceVatlib::CKeyboard::s_keyboardHook, nCode, wParam, lParam);

        // Check for a key down press
        if (nCode == HC_ACTION)
        {
            KBDLLHOOKSTRUCT *pKeyboard = (KBDLLHOOKSTRUCT *)lParam;
            // http://msdn.microsoft.com/en-us/library/windows/desktop/ms644967(v=vs.85).aspx
            // http://stackoverflow.com/questions/18917716/windows-how-to-query-state-of-modifier-keys-within-low-level-keyboard-hook
            if (wParam == WM_KEYDOWN)
            {
                if (pKeyboard->vkCode == VK_LCONTROL) CVoiceVatlib::CKeyboard::s_voice->m_pushToTalk = true;
            }
            else if (wParam == WM_KEYUP)
            {
                if (pKeyboard->vkCode == VK_LCONTROL) CVoiceVatlib::CKeyboard::s_voice->m_pushToTalk = false;
            }
        }
        return CallNextHookEx(CVoiceVatlib::CKeyboard::s_keyboardHook, nCode, wParam, lParam); ; // not processed flag
    }

    /*
     * Handle PTT
     */
    void CVoiceVatlib::handlePushToTalk()
    {
        if (!this->m_voice) return;
        CVoiceRoomList rooms = this->getComVoiceRoomsWithAudioStatus();
        CVoiceRoom room1 = rooms[0];
        CVoiceRoom room2 = rooms[1];
        if (room1.isConnected())
        {
            if (this->m_pushToTalk && !this->m_voice->IsAudioPlaying(IVoice::COM1))
                this->startTransmitting(IVoice::COM1);
            else if (!this->m_pushToTalk)
                this->stopTransmitting(IVoice::COM1);
        }
        if (room2.isConnected())
        {
            if (this->m_pushToTalk && !this->m_voice->IsAudioPlaying(IVoice::COM2))
                this->startTransmitting(IVoice::COM2);
            else if (!this->m_pushToTalk)
                this->stopTransmitting(IVoice::COM2);
        }
    }

#else
    void CVoiceVatlib::handlePushToTalk() {}
#endif

} // namespace
