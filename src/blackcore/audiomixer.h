/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_AUDIOMIXER_H
#define BLACKCORE_AUDIOMIXER_H

#include <QObject>
#include "blackcore/blackcoreexport.h"

namespace BlackCore
{
    //! Interface to an audio mixer
    class BLACKCORE_EXPORT IAudioMixer : public QObject
    {
        Q_OBJECT

    public:

        //! Audio mixer input ports
        enum InputPort
        {
            InputMicrophone,
            InputVoiceChannel1,
            InputVoiceChannel2,
        };

        //! Audio mixer output ports
        enum OutputPort
        {
            OutputOutputDevice1,
            OutputVoiceChannel1,
            OutputVoiceChannel2,
        };

        //! Default constructor
        IAudioMixer(QObject *parent = nullptr);

        //! Virtual destructor.
        virtual ~IAudioMixer() {}

        //! Connect mixer input to a mixer output. This causes audio to be routed from the input to output
        virtual void makeMixerConnection(InputPort inputPort, OutputPort outputPort) = 0;

        //! Remove the mixer connection from input to output
        virtual void removeMixerConnection(InputPort inputPort, OutputPort outputPort) = 0;

        //! Returns true if input port and output port are connected
        virtual bool hasMixerConnection(InputPort inputPort, OutputPort outputPort) = 0;
    };

} // ns

#endif // guard
