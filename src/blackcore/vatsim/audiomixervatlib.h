/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_VATSIM_AUDIOMIXERVATLIB_H
#define BLACKCORE_VATSIM_AUDIOMIXERVATLIB_H

#include "blackcore/audiomixer.h"
#include "blackcore/blackcoreexport.h"
#include "vatlib/vatlib.h"

#include <stdbool.h>
#include <QObject>
#include <QScopedPointer>

namespace BlackCore
{
    namespace Vatsim
    {
        //! Interface to an audio mixer
        class BLACKCORE_EXPORT CAudioMixerVatlib : public IAudioMixer
        {
            Q_OBJECT

        public:

            /*!
             * \brief Default constructor with parent
             * \param parent
             */
            CAudioMixerVatlib(QObject *parent = nullptr);

            //! Virtual destructor.
            virtual ~CAudioMixerVatlib() {}

            //! \copydoc IAudioMixer::makeMixerConnection
            virtual void makeMixerConnection(InputPort inputPort, OutputPort outputPort) override;

            //! \copydoc IAudioMixer::removeMixerConnection
            virtual void removeMixerConnection(InputPort inputPort, OutputPort outputPort) override;

            //! \copydoc IAudioMixer::hasMixerConnection
            virtual bool hasMixerConnection(InputPort inputPort, OutputPort outputPort) override;

            //! Return the pointer to vatlib audio mixer
            VatAudioMixer *getVatAudioMixer() { return m_audioMixer.data(); }

        private:

            struct VatAudioMixerDeleter
            {
                static inline void cleanup(VatAudioMixer *obj)
                {
                    if (obj) Vat_DestroyAudioMixer(obj);
                }
            };

            QScopedPointer<VatAudioMixer, VatAudioMixerDeleter> m_audioMixer;
        };
    } // ns
} // ns

#endif // guard
