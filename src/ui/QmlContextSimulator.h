//
// Created by lars on 12/23/24.
//

#ifndef QMLCONTEXTSIMULATOR_H
#define QMLCONTEXTSIMULATOR_H

#include <iostream>
#include <ostream>

#include <QObject>

#include "core/context/contextsimulator.h"

class QmlContextSimulator : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList availableSimulators READ getAvailableSimulators NOTIFY availableSimulatorsChanged)
    Q_PROPERTY(bool simulatorPluginSelected READ isSimulatorPluginSelected NOTIFY simulatorPluginSelected)
    //    Q_PROPERTY(bool connectionStatus READ getConnectionStatus NOTIFY connectionStatusChanged)
public:
    QmlContextSimulator(swift::core::context::IContextSimulator *simulator) : simulator_(simulator)
    {
        connect(simulator_, &swift::core::context::IContextSimulator::simulatorPluginSelected, this,
                &QmlContextSimulator::simulatorPluginSelected);
    }

    QStringList getAvailableSimulators() const
    {
        auto availableSimulators = simulator_->getAvailableSimulatorPlugins();
        QStringList sims;
        std::transform(availableSimulators.begin(), availableSimulators.end(), std::back_inserter(sims),
                       [](const auto &val) { return val.getName(); });
        return sims;
    }

    bool isSimulatorPluginSelected() const { return simulator_->isSimulatorPluginSelected(); }

    Q_INVOKABLE void setPluginIdentifier(const QString &identifier, bool store)
    {
        auto available = simulator_->getAvailableSimulatorPlugins();
        auto it =
            std::find_if(available.begin(), available.end(), [&](const auto &v) { return v.getName() == identifier; });

        assert(it != available.end());
        simulator_->setPlugin(*it);

        if (store) { m_enabledSimulator.setAndSave(it->getIdentifier()); }
    }

    Q_INVOKABLE void clearSimulator() { m_enabledSimulator.setAndSave(""); }

    //    bool getConnectionStatus() { return simulator_->isConnected(); }

signals:
    void availableSimulatorsChanged();
    void connectionStatusChanged();
    void simulatorPluginSelected();

private:
    swift::core::context::IContextSimulator *simulator_;
    swift::misc::CSetting<swift::core::application::TEnabledSimulator> m_enabledSimulator { this };
};

#endif // QMLCONTEXTSIMULATOR_H
