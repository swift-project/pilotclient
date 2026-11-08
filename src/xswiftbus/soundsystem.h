// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef SWIFT_SIM_XSWIFTBUS_SOUNDSYSTEM_H
#define SWIFT_SIM_XSWIFTBUS_SOUNDSYSTEM_H

//! \file

#include <string>
#include <unordered_map>

namespace XSwiftBus
{
    class CTraffic;

    /*!
     * XP12 network aircraft sound system. Plays looping engine/tire sounds and
     * one-shot gear/flap sounds for network aircraft using X-Plane's built-in
     * sound files. Uses the SDK400 PCM audio API via runtime symbol lookup,
     * so no compile-time SDK version bump is needed. Silently disables itself
     * on XP11 (functions not found).
     */
    class CSoundSystem
    {
    public:
        //! Resolve the XP12 sound functions and load the sound files
        void init();

        //! Release all channels and unresolve the functions
        void cleanup();

        //! Update all sound channels (called from the flight loop)
        void update();

        //! Remove sound state for a removed plane
        void removePlane(const std::string &callsign);

    private:
        //! Per-plane sound state
        struct PlaneSounds
        {
            void *engineChannel = nullptr;
            void *tireChannel = nullptr;
            float prevGearRatio = -1;
            float prevFlapRatio = -1;
        };

        //! Minimal PCM audio buffer decoded from a 16-bit WAV file
        struct PcmBuffer
        {
            void *data = nullptr;
            uint32_t sizeBytes = 0;
            int freqHz = 0;
            int numChannels = 0;
        };

        //! Load a WAV file into a PCM buffer, returns false on failure
        static bool loadWav(const std::string &filePath, PcmBuffer &buffer);

        //! Resolve the SDK400 sound functions from the host process
        bool resolveFunctions();

        std::unordered_map<std::string, PlaneSounds> m_planeSounds;
        bool m_enabled = false;
        bool m_functionsResolved = false;
        int m_skipCycle = 0;

        // PCM buffers loaded from X-Plane's stock sound files
        PcmBuffer m_engineHiBypass;
        PcmBuffer m_engineLoBypass;
        PcmBuffer m_engineTurboprop;
        PcmBuffer m_enginePropeller;
        PcmBuffer m_engineHelo;
        PcmBuffer m_reverseThrust;
        PcmBuffer m_tireRoll;
        PcmBuffer m_flapSound;
        PcmBuffer m_gearSound;

        // function pointer types matching the SDK400 signatures
        using T_XPLMPlayPCMOnBus = void *(*)(void *, uint32_t, int, int, int, int, int, void (*)(void *, int), void *);
        using T_XPLMStopAudio = int (*)(void *);
        using T_XPLMSetAudioPosition = int (*)(void *, float *, float *);
        using T_XPLMSetAudioFadeDistance = int (*)(void *, float, float);
        using T_XPLMSetAudioVolume = int (*)(void *, float);

        T_XPLMPlayPCMOnBus m_playPcmOnBus = nullptr;
        T_XPLMStopAudio m_stopAudio = nullptr;
        T_XPLMSetAudioPosition m_setAudioPosition = nullptr;
        T_XPLMSetAudioFadeDistance m_setAudioFadeDistance = nullptr;
        T_XPLMSetAudioVolume m_setAudioVolume = nullptr;
    };
} // namespace XSwiftBus

#endif // SWIFT_SIM_XSWIFTBUS_SOUNDSYSTEM_H
