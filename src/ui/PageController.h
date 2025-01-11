//
// Created by lars on 12/23/24.
//

#ifndef PAGECONTROLLER_H
#define PAGECONTROLLER_H

#include <iostream>
#include <ostream>

#include <QObject>

#include "core/context/contextsimulator.h"

class PageController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString component READ getComponent NOTIFY componentChanged)
public:

    PageController()
    {
        swift::misc::CSetting<swift::core::application::TEnabledSimulator> enabledSimulator { this };
    };
    virtual ~PageController() {};
    QString getComponent() const
    {
        return m_component;
    }

    Q_INVOKABLE void navigateToHome()
    {
        m_component = "../pages/MainPage.qml";
        emit componentChanged();
    }
    Q_INVOKABLE void navigateToConnection()
    {
        m_component = "../pages/ConnectionPage.qml";
        emit componentChanged();
    }

signals:
    void componentChanged();

private:

    QString m_component = "../pages/MainPage.qml";

private:
};

#endif // QMLCONTEXTSIMULATOR_H
