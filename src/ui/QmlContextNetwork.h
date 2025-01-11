//
// Created by lars on 12/23/24.
//

#ifndef QMLCONTEXTNETWORK_H
#define QMLCONTEXTNETWORK_H

#include <iostream>
#include <ostream>

#include <QObject>

#include "core/context/contextnetwork.h"

namespace swift::core::context
{
    class IContextNetwork;
}
class QmlContextNetwork : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connectionStatus READ getConnectionStatus NOTIFY connectionStatusChanged)
public:
    QmlContextNetwork(swift::core::context::IContextNetwork *network) : network_(network)
    {
        connect(network_, &swift::core::context::IContextNetwork::connectionStatusChanged, this,
                &QmlContextNetwork::connectionStatusChanged);
    }

    bool getConnectionStatus() { return network_->isConnected(); }

signals:
    void connectionStatusChanged();

public slots:

private:
    swift::core::context::IContextNetwork *network_;
};

#endif // QMLCONTEXTNETWORK_H
