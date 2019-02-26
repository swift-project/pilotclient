/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SHAREDSTATE_SCALARMUTATOR_H
#define BLACKMISC_SHAREDSTATE_SCALARMUTATOR_H

#include "blackmisc/sharedstate/passivemutator.h"
#include "blackmisc/variant.h"
#include "blackmisc/blackmiscexport.h"
#include <QObject>
#include <QMutex>

namespace BlackMisc
{
    namespace SharedState
    {
        class IDataLink;

        /*!
         * Non-template base class for CScalarMutator.
         */
        class BLACKMISC_EXPORT CGenericScalarMutator : public QObject
        {
            Q_OBJECT

        public:
            //! Publish using the given transport mechanism.
            void initialize(IDataLink *);

        protected:
            //! Constructor.
            CGenericScalarMutator(QObject *parent) : QObject(parent) {}

            //! Set scalar value as variant.
            void setValue(const CVariant &value);

        private:
            QSharedPointer<CPassiveMutator> m_mutator = CPassiveMutator::create(this);
        };

        /*!
         * Base class for an object that shares state with a corresponding CScalarObserver subclass object.
         * \tparam T Datatype encapsulating the state to be shared.
         */
        template <typename T>
        class CScalarMutator : public CGenericScalarMutator
        {
        protected:
            //! Constructor.
            CScalarMutator(QObject *parent) : CGenericScalarMutator(parent) {}

        public:
            //! Set scalar value.
            void setValue(const T &value) { CGenericScalarMutator::setValue(CVariant::from(value)); }
        };
    }
}

#endif
