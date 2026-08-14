// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/aviation/aircraftenginelist.h"

#include <QString>
#include <QtGlobal>

SWIFT_DEFINE_SEQUENCE_MIXINS(swift::misc::aviation, CAircraftEngine, CAircraftEngineList)

namespace swift::misc::aviation
{
    CAircraftEngineList::CAircraftEngineList(std::initializer_list<std::pair<bool, int>> enginesOnOff)
    {
        int no = 1; // engines 1 based
        for (const auto &[on, engineRpmPct] : enginesOnOff)
        {
            CAircraftEngine engine(no++, on, engineRpmPct);
            this->push_back(engine);
        }
    }

    CAircraftEngineList::CAircraftEngineList(const CSequence<CAircraftEngine> &other)
        : CSequence<CAircraftEngine>(other)
    {}

    CAircraftEngine CAircraftEngineList::getEngine(int engineNumber) const
    {
        Q_ASSERT(engineNumber > 0);
        return this->findBy(&CAircraftEngine::getNumber, engineNumber).frontOrDefault();
    }

    bool CAircraftEngineList::isEngineOn(int engineNumber) const
    {
        Q_ASSERT(engineNumber > 0);
        return this->getEngine(engineNumber).isOn();
    }

    void CAircraftEngineList::setEngineOn(int engineNumber, bool on)
    {
        Q_ASSERT(engineNumber > 0);
        for (CAircraftEngine &engine : *this)
        {
            if (engine.getNumber() == engineNumber)
            {
                engine.setOn(on);
                break;
            }
        }
    }

    void CAircraftEngineList::setEngines(const CAircraftEngine &engine, int engineNumber)
    {
        this->clear();
        for (int e = 0; e < engineNumber; e++)
        {
            CAircraftEngine copy(engine);
            copy.setNumber(e + 1); // 1 based
            this->push_back(engine);
        }
    }

    void CAircraftEngineList::initEngines(int engineNumber, bool on, int engineRpmPercentage)
    {
        this->clear();
        for (int e = 0; e < engineNumber; e++)
        {
            const CAircraftEngine engine(e + 1, on, engineRpmPercentage);
            this->push_back(engine);
        }
    }

    void CAircraftEngineList::setEngineRpmPct(int engineNumber, int percentage)
    {
        Q_ASSERT(engineNumber > 0);
        for (CAircraftEngine &engine : *this)
        {
            if (engine.getNumber() == engineNumber)
            {
                engine.setEngineRpmPct(percentage);
                break;
            }
        }
    }

    int CAircraftEngineList::getEngineRpmPct(int engineNumber) const
    {
        Q_ASSERT(engineNumber > 0);
        return this->getEngine(engineNumber).getEngineRpmPct();
    }

    bool CAircraftEngineList::isAnyEngineOn() const { return this->contains(&CAircraftEngine::isOn, true); }

    QJsonObject CAircraftEngineList::toJson() const
    {
        QJsonObject map;

        for (const auto &e : *this)
        {
            const QJsonObject value = e.toJson();
            map.insert(QString::number(e.getNumber()), value);
        }
        return map;
    }

    void CAircraftEngineList::convertFromJson(const QJsonObject &json)
    {
        this->clear();
        for (const auto &e : json.keys())
        {
            bool ok{false};
            const int number = e.toInt(&ok);
            if (!ok || number <= 0)
            {
                // skip invalid JSON entries 
                return;
            }
            CJsonScope scope(e);
            Q_UNUSED(scope);
            CAircraftEngine engine;
            engine.convertFromJson(json.value(e).toObject());
            engine.setNumber(number);
            push_back(engine);
        }
    }
} // namespace swift::misc::aviation
