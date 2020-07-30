/* Copyright (C) 2020
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \cond PRIVATE_TESTS

/*!
 * \file
 * \ingroup testblackmisc
 */

#include "blackmisc/sharedstate/scalarmutator.h"
#include "blackmisc/sharedstate/scalarjournal.h"
#include "blackmisc/sharedstate/scalarobserver.h"
#include "blackmisc/sharedstate/listmutator.h"
#include "blackmisc/sharedstate/listjournal.h"
#include "blackmisc/sharedstate/listobserver.h"
#include "blackmisc/sharedstate/datalink.h"
#include <QMetaType>

namespace BlackMiscTest
{
    //! \private List filter for testing
    class CTestFilter : public BlackMisc::CValueObject<CTestFilter>
    {
    public:
        bool matches(const BlackMisc::CVariant &value) const
        {
            return value.canConvert<unsigned>() && (value.to<unsigned>() & m_mask);
        }
        QString convertToQString(bool = false) const { return QString::number(m_mask); }
        CTestFilter(unsigned mask = ~0u) : m_mask(mask) {}
    private:
        unsigned m_mask = ~0u;
        BLACK_METACLASS(CTestFilter, BLACK_METAMEMBER(mask));
    };
}

//! \cond
Q_DECLARE_METATYPE(BlackMiscTest::CTestFilter);
//! \endcond

namespace BlackMiscTest
{
    //! Scalar mutator subclass
    class CTestScalarMutator : public BlackMisc::SharedState::CScalarMutator<int>
    {
        Q_OBJECT
        BLACK_SHARED_STATE_CHANNEL("test_scalar_channel")
    public:
        //! Ctor
        CTestScalarMutator(QObject *parent) : CScalarMutator(parent) {}
    };

    //! Scalar journal subclass
    class CTestScalarJournal : public BlackMisc::SharedState::CScalarJournal<int>
    {
        Q_OBJECT
        BLACK_SHARED_STATE_CHANNEL("test_scalar_channel")
    public:
        //! Ctor
        CTestScalarJournal(QObject *parent) : CScalarJournal(parent) {}
    };

    //! Scalar observer subclass
    class CTestScalarObserver : public BlackMisc::SharedState::CScalarObserver<int>
    {
        Q_OBJECT
        BLACK_SHARED_STATE_CHANNEL("test_scalar_channel")
    public:
        //! Ctor
        CTestScalarObserver(QObject *parent) : CScalarObserver(parent) {}
        virtual void onValueChanged(const int &) override {}
    };

    //! List mutator subclass
    class CTestListMutator : public BlackMisc::SharedState::CListMutator<QList<int>>
    {
        Q_OBJECT
        BLACK_SHARED_STATE_CHANNEL("test_list_channel")
    public:
        //! Ctor
        CTestListMutator(QObject *parent) : CListMutator(parent) {}
    };

    //! List journal subclass
    class CTestListJournal : public BlackMisc::SharedState::CListJournal<QList<int>>
    {
        Q_OBJECT
        BLACK_SHARED_STATE_CHANNEL("test_list_channel")
    public:
        //! Ctor
        CTestListJournal(QObject *parent) : CListJournal(parent) {}
    };

    //! List observer subclass
    class CTestListObserver : public BlackMisc::SharedState::CListObserver<QList<int>, CTestFilter>
    {
        Q_OBJECT
        BLACK_SHARED_STATE_CHANNEL("test_list_channel")
    public:
        //! Ctor
        CTestListObserver(QObject *parent) : CListObserver(parent) {}

        //! \name Interface implementation
        //! @{
        virtual void onElementAdded(const int &) override {}
        virtual void onElementsReplaced(const QList<int> &) override {}
        //! @}
    };
}

//! \endcond
