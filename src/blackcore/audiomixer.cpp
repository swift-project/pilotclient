/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/audiomixer.h"

namespace BlackCore
{
    IAudioMixer::IAudioMixer(QObject *parent) : QObject(parent)
    { }

    bool IAudioMixer::makeOrRemoveConnection(IAudioMixer::InputPort inputPort, IAudioMixer::OutputPort outputPort, bool make)
    {
        return make ?
               this->makeMixerConnectionIfNotExisting(inputPort, outputPort) :
               this->removeMixerConnectionIfExisting(inputPort, outputPort);
    }

    bool IAudioMixer::makeMixerConnectionIfNotExisting(IAudioMixer::InputPort inputPort, IAudioMixer::OutputPort outputPort)
    {
        if (this->hasMixerConnection(inputPort, outputPort)) { return false; }
        this->makeMixerConnection(inputPort, outputPort);
        return true;
    }

    bool IAudioMixer::removeMixerConnectionIfExisting(IAudioMixer::InputPort inputPort, IAudioMixer::OutputPort outputPort)
    {
        if (!this->hasMixerConnection(inputPort, outputPort)) { return false; }
        this->removeMixerConnection(inputPort, outputPort);
        return true;
    }
}
