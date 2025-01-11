//
// Created by lars on 12/31/24.
//

#ifndef SIMSELECTION_H
#define SIMSELECTION_H

struct SimSelection : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString simName MEMBER m_simName)
    Q_PROPERTY(bool remember MEMBER m_remember)
public:
    QString m_simName;
    bool m_remember = false;
};

#endif //SIMSELECTION_H
