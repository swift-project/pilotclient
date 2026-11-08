// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "soundsystem.h"

#include "traffic.h"

#include <XPLM/XPLMGraphics.h>
#include <XPLM/XPLMUtilities.h>

#include <algorithm>
#include <cmath>
#include <cstring>

#ifdef IBM
#    include <Windows.h>
#else
#    include <dlfcn.h>
#endif

namespace XSwiftBus
{
    namespace
    {
        void *hostSymbol(const char *name)
        {
#ifdef IBM
            return reinterpret_cast<void *>(GetProcAddress(GetModuleHandleA(nullptr), name));
#else
            return dlsym(RTLD_DEFAULT, name);
#endif
        }
    } // namespace

    void CSoundSystem::init()
    {
        if (m_functionsResolved) { return; }
        if (!this->resolveFunctions()) { return; } // XP11 or older
        m_functionsResolved = true;

        char xplaneDir[512] = {};
        XPLMGetSystemPath(xplaneDir);
        const std::string sep =
#ifdef IBM
            "\\";
#else
            "/";
#endif
        const std::string soundsDir = std::string(xplaneDir) + "Resources" + sep + "sounds" + sep;
        const std::string engineDir = soundsDir + "engine" + sep;
        const std::string contactDir = soundsDir + "contact" + sep;
        const std::string systemsDir = soundsDir + "systems" + sep;

        this->loadWav(engineDir + "ENGINE_HI_BYPASS_JET_out.wav", m_engineHiBypass);
        this->loadWav(engineDir + "ENGINE_LO_BYPASS_JET_out.wav", m_engineLoBypass);
        this->loadWav(engineDir + "ENGINE_TURBOPROP_out.wav", m_engineTurboprop);
        this->loadWav(engineDir + "PROPELLER_OF_AIRPLANE_out.wav", m_enginePropeller);
        this->loadWav(engineDir + "PROPELLER_OF_HELO_out.wav", m_engineHelo);
        this->loadWav(engineDir + "REVERSE_THRUST_out.wav", m_reverseThrust);
        this->loadWav(contactDir + "roll_runway.wav", m_tireRoll);
        this->loadWav(systemsDir + "flap.wav", m_flapSound);
        this->loadWav(systemsDir + "gear.wav", m_gearSound);

        m_enabled = m_engineHiBypass.data || m_engineLoBypass.data || m_enginePropeller.data || m_engineTurboprop.data;
    }

    void CSoundSystem::cleanup()
    {
        if (m_stopAudio)
        {
            for (const auto &pair : m_planeSounds)
            {
                if (pair.second.engineChannel) { m_stopAudio(pair.second.engineChannel); }
                if (pair.second.tireChannel) { m_stopAudio(pair.second.tireChannel); }
            }
        }
        m_planeSounds.clear();
        m_enabled = false;
        m_functionsResolved = false;
    }

    bool CSoundSystem::resolveFunctions()
    {
        m_playPcmOnBus = reinterpret_cast<T_XPLMPlayPCMOnBus>(hostSymbol("XPLMPlayPCMOnBus"));
        if (!m_playPcmOnBus) { return false; }
        m_stopAudio = reinterpret_cast<T_XPLMStopAudio>(hostSymbol("XPLMStopAudio"));
        m_setAudioPosition = reinterpret_cast<T_XPLMSetAudioPosition>(hostSymbol("XPLMSetAudioPosition"));
        m_setAudioFadeDistance = reinterpret_cast<T_XPLMSetAudioFadeDistance>(hostSymbol("XPLMSetAudioFadeDistance"));
        m_setAudioVolume = reinterpret_cast<T_XPLMSetAudioVolume>(hostSymbol("XPLMSetAudioVolume"));
        return true;
    }

    bool CSoundSystem::loadWav(const std::string &filePath, PcmBuffer &buffer)
    {
        FILE *file = fopen(filePath.c_str(), "rb");
        if (!file) { return false; }

        char header[12] = {};
        if (fread(header, 1, 12, file) != 12 || std::memcmp(header, "RIFF", 4) != 0 ||
            std::memcmp(header + 8, "WAVE", 4) != 0)
        {
            fclose(file);
            return false;
        }

        int freqHz = 0;
        int numChannels = 0;
        int bitsPerSample = 0;
        std::vector<char> pcmData;

        char chunkHeader[8] = {};
        while (fread(chunkHeader, 1, 8, file) == 8)
        {
            const uint32_t chunkSize = static_cast<uint32_t>(static_cast<uint8_t>(chunkHeader[4])) |
                                       (static_cast<uint32_t>(static_cast<uint8_t>(chunkHeader[5])) << 8) |
                                       (static_cast<uint32_t>(static_cast<uint8_t>(chunkHeader[6])) << 16) |
                                       (static_cast<uint32_t>(static_cast<uint8_t>(chunkHeader[7])) << 24);
            if (std::memcmp(chunkHeader, "fmt ", 4) == 0)
            {
                char fmtData[16] = {};
                if (fread(fmtData, 1, 16, file) != 16) { break; }
                numChannels = static_cast<uint8_t>(fmtData[2]) | (static_cast<uint8_t>(fmtData[3]) << 8);
                std::memcpy(&freqHz, fmtData + 4, 4);
                bitsPerSample = static_cast<uint8_t>(fmtData[14]) | (static_cast<uint8_t>(fmtData[15]) << 8);
                if (chunkSize > 16) { fseek(file, static_cast<long>(chunkSize - 16), SEEK_CUR); }
            }
            else if (std::memcmp(chunkHeader, "data", 4) == 0)
            {
                pcmData.resize(chunkSize);
                if (fread(pcmData.data(), 1, chunkSize, file) != chunkSize) { pcmData.clear(); }
                break;
            }
            else
            {
                fseek(file, static_cast<long>(chunkSize), SEEK_CUR);
            }
        }
        fclose(file);

        if (pcmData.empty() || bitsPerSample != 16 || numChannels < 1) { return false; }
        buffer.data = new char[pcmData.size()];
        std::memcpy(buffer.data, pcmData.data(), pcmData.size());
        buffer.sizeBytes = static_cast<uint32_t>(pcmData.size());
        buffer.freqHz = freqHz;
        buffer.numChannels = numChannels;
        return true;
    }

    void CSoundSystem::removePlane(const std::string &callsign)
    {
        const auto it = m_planeSounds.find(callsign);
        if (it == m_planeSounds.end()) { return; }
        if (m_stopAudio)
        {
            if (it->second.engineChannel) { m_stopAudio(it->second.engineChannel); }
            if (it->second.tireChannel) { m_stopAudio(it->second.tireChannel); }
        }
        m_planeSounds.erase(it);
    }

    void CSoundSystem::update()
    {
        if (!m_enabled || !m_playPcmOnBus) { return; }

        CTraffic *traffic = CTraffic::instance();
        if (!traffic) { return; }

        const bool doExpensive = (m_skipCycle % 10) == 0;
        m_skipCycle++;

        for (const auto &pair : traffic->getPlanesById())
        {
            const CTraffic::Plane *plane = pair.second;
            if (!plane || !plane->flightModelFilled) { continue; }

            PlaneSounds &sounds = m_planeSounds[plane->callsign];

            // engine sound
            {
                const float thrust = plane->surfaces.thrust;
                if (thrust > 0.01f)
                {
                    if (!sounds.engineChannel)
                    {
                        const PcmBuffer *pcm = &m_engineLoBypass;
                        const char engType = plane->classification.size() > 2 ? plane->classification[2] : '\0';
                        const bool isHeli = !plane->classification.empty() && plane->classification[0] == 'H';
                        if (isHeli) { pcm = &m_engineHelo; }
                        else if (engType == 'T') { pcm = &m_engineTurboprop; }
                        else if (engType == 'P') { pcm = &m_enginePropeller; }
                        else if (plane->classification.size() > 1 && plane->classification[1] == '1')
                        {
                            pcm = &m_engineHiBypass;
                        }
                        if (pcm->data)
                        {
                            sounds.engineChannel = m_playPcmOnBus(
                                pcm->data, pcm->sizeBytes, 1 /* PCM16 */, pcm->freqHz, pcm->numChannels,
                                1 /* loop */, 0 /* exterior bus */, nullptr, nullptr);
                            if (sounds.engineChannel && m_setAudioFadeDistance)
                            {
                                m_setAudioFadeDistance(sounds.engineChannel, 50.0f, 10000.0f);
                            }
                        }
                    }
                    if (sounds.engineChannel && m_setAudioVolume)
                    {
                        m_setAudioVolume(sounds.engineChannel, std::min(thrust, 1.0f));
                    }
                }
                else if (sounds.engineChannel)
                {
                    if (m_stopAudio) { m_stopAudio(sounds.engineChannel); }
                    sounds.engineChannel = nullptr;
                }
            }

            // tire roll sound
            {
                const double dLat = plane->positions[2].lat - plane->positions[1].lat;
                const double dLon = plane->positions[2].lon - plane->positions[1].lon;
                const double groundSpeedMs =
                    std::sqrt(dLat * dLat + dLon * dLon) * 6371000.0 * 0.017453292519943295;

                if (plane->isOnGround && groundSpeedMs > 2.0)
                {
                    if (!sounds.tireChannel && m_tireRoll.data)
                    {
                        sounds.tireChannel = m_playPcmOnBus(
                            m_tireRoll.data, m_tireRoll.sizeBytes, 1, m_tireRoll.freqHz, m_tireRoll.numChannels,
                            1, 0, nullptr, nullptr);
                        if (sounds.tireChannel && m_setAudioFadeDistance)
                        {
                            m_setAudioFadeDistance(sounds.tireChannel, 20.0f, 5000.0f);
                        }
                    }
                    if (sounds.tireChannel && m_setAudioVolume)
                    {
                        m_setAudioVolume(sounds.tireChannel,
                                         std::min(static_cast<float>(groundSpeedMs / 80.0), 1.0f));
                    }
                }
                else if (sounds.tireChannel)
                {
                    if (m_stopAudio) { m_stopAudio(sounds.tireChannel); }
                    sounds.tireChannel = nullptr;
                }
            }

            // gear / flap one-shot sounds on ratio change
            {
                const float gearRatio = plane->surfaces.gearPosition;
                const float flapRatio = plane->surfaces.flapRatio;
                if (sounds.prevGearRatio >= 0 && std::fabs(gearRatio - sounds.prevGearRatio) > 0.01f && m_gearSound.data)
                {
                    m_playPcmOnBus(m_gearSound.data, m_gearSound.sizeBytes, 1, m_gearSound.freqHz,
                                   m_gearSound.numChannels, 0, 0, nullptr, nullptr);
                }
                if (sounds.prevFlapRatio >= 0 && std::fabs(flapRatio - sounds.prevFlapRatio) > 0.01f && m_flapSound.data)
                {
                    m_playPcmOnBus(m_flapSound.data, m_flapSound.sizeBytes, 1, m_flapSound.freqHz,
                                   m_flapSound.numChannels, 0, 0, nullptr, nullptr);
                }
                sounds.prevGearRatio = gearRatio;
                sounds.prevFlapRatio = flapRatio;
            }

            // 3D positioning every 10 frames
            if (doExpensive && m_setAudioPosition)
            {
                double glX = 0;
                double glY = 0;
                double glZ = 0;
                XPLMLocalToWorld(plane->positions[3].lat, plane->positions[3].lon,
                                 plane->positions[3].elevation * 0.3048f, &glX, &glY, &glZ);
                float pos[3] = { static_cast<float>(glX), static_cast<float>(glY), static_cast<float>(glZ) };
                float vel[3] = {};
                if (sounds.engineChannel) { m_setAudioPosition(sounds.engineChannel, pos, vel); }
                if (sounds.tireChannel) { m_setAudioPosition(sounds.tireChannel, pos, vel); }
            }
        }

        // remove sound state for removed planes
        for (auto it = m_planeSounds.begin(); it != m_planeSounds.end();)
        {
            bool found = false;
            for (const auto &pair : traffic->getPlanesById())
            {
                if (pair.second->callsign == it->first) { found = true; break; }
            }
            if (!found) { it = m_planeSounds.erase(it); }
            else { ++it; }
        }
    }
} // namespace XSwiftBus
