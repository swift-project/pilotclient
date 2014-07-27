/* Copyright (C) 2013
 * Swift Project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKMISC_ICONSNETWORK_H
#define BLACKMISC_ICONSNETWORK_H

#include <QPixmap>
#include "iconsstandard.h"

//! \file

namespace BlackMisc
{
    //! Standard icons
    class CIconsNetworkAndAviation
    {
    private:
        //! Constructor, use class static only
        CIconsNetworkAndAviation();

    public:

        //! C1
        static const QPixmap &roleC1()
        {
            static const QPixmap pm(":/vatsim/icons/vatsim/C1.png");
            return pm;
        }

        //! C2
        static const QPixmap &roleC2()
        {
            static const QPixmap pm(":/vatsim/icons/vatsim/C2.png");
            return pm;
        }

        //! C3
        static const QPixmap &roleC3()
        {
            static const QPixmap pm(":/vatsim/icons/vatsim/C2.png");
            return pm;
        }

        //! S1
        static const QPixmap &roleS1()
        {
            static const QPixmap pm(":/vatsim/icons/vatsim/S1.png");
            return pm;
        }

        //! S2
        static const QPixmap &roleS2()
        {
            static const QPixmap pm(":/vatsim/icons/vatsim/S2.png");
            return pm;
        }

        //! S3
        static const QPixmap &roleS3()
        {
            static const QPixmap pm(":/vatsim/icons/vatsim/S3.png");
            return pm;
        }

        //! I1
        static const QPixmap &roleI1()
        {
            static const QPixmap pm(":/vatsim/icons/vatsim/I1.png");
            return pm;
        }

        //! I3
        static const QPixmap &roleI3()
        {
            static const QPixmap pm(":/vatsim/icons/vatsim/I3.png");
            return pm;
        }

        //! MNT
        static const QPixmap &roleMnt()
        {
            static const QPixmap pm(":/vatsim/icons/vatsim/MNT.png");
            return pm;
        }

        //! OBS
        static const QPixmap &roleObs()
        {
            static const QPixmap pm(":/vatsim/icons/vatsim/OBS.png");
            return pm;
        }

        //! SUP
        static const QPixmap &roleSup()
        {
            static const QPixmap pm(":/vatsim/icons/vatsim/SUP.png");
            return pm;
        }

        //! Pilot
        static const QPixmap &rolePilot()
        {
            static const QPixmap pm(QPixmap(":/own/icons/own/pilot.png").scaledToWidth(16, Qt::SmoothTransformation));
            return pm;
        }

        //! Approach
        static const QPixmap &roleApproach()
        {
            static const QPixmap pm(QPixmap(":/own/icons/own/app.jpg").scaledToWidth(16, Qt::SmoothTransformation));
            return pm;
        }

        //! Ground
        static const QPixmap &roleGround()
        {
            static const QPixmap pm(QPixmap(":/own/icons/own/gnd.jpg").scaledToWidth(16, Qt::SmoothTransformation));
            return pm;
        }

        //! Delivery
        static const QPixmap &roleDelivery()
        {
            static const QPixmap pm(QPixmap(":/own/icons/own/del.jpg").scaledToWidth(16, Qt::SmoothTransformation));
            return pm;
        }

        //! Tower
        static const QPixmap &roleTower()
        {
            static const QPixmap pm(QPixmap(":/own/icons/own/twr.jpg").scaledToWidth(16, Qt::SmoothTransformation));
            return pm;
        }

        //! Center
        static const QPixmap &roleCenter()
        {
            static const QPixmap pm(QPixmap(":/own/icons/own/ctr.jpg").scaledToWidth(16, Qt::SmoothTransformation));
            return pm;
        }

        //! Unknown
        static const QPixmap &roleUnknown()
        {
            static const QPixmap pm(":/pastel/icons/pastel/16/help.png");
            return pm;
        }

        //! Connected
        static const QPixmap &statusConnected()
        {
            static const QPixmap pm(":/own/icons/own/logingreen.png");
            return pm;
        }

        //! Disconnected
        static const QPixmap &statusDisconnected()
        {
            static const QPixmap pm(":/own/icons/own/loginred.png");
            return pm;
        }

        //! Transition
        static const QPixmap &statusTransition()
        {
            static const QPixmap pm(":/own/icons/own/loginyellow.png");
            return pm;
        }

        //! Full voice capability
        static const QPixmap &capabilityVoice()
        {
            static const QPixmap pm(":/diagona/icons/diagona/icons/headphone.png");
            return pm;
        }

        //! Voice receive only
        static const QPixmap &capabilityVoiceReceiveOnly()
        {
            static const QPixmap pm(":/diagona/icons/diagona/icons/headphone.png");
            return pm;
        }

        //! Text only
        static const QPixmap &capabilityTextOnly()
        {
            return CIconsStandard::appTextMessages16();
        }

        //! Text only
        static const QPixmap &capabilityUnknown()
        {
            return CIconsStandard::unknown16();
        }
    };
}
#endif // guard
