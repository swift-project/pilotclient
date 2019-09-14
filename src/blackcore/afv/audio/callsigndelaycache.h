#ifndef CALLSIGNDELAYCACHE_H
#define CALLSIGNDELAYCACHE_H

#include <QHash>
#include <QString>


class CallsignDelayCache
{
public:
    void initialise(const QString &callsign);
    int get(const QString &callsign);
    void underflow(const QString &callsign);
    void success(const QString &callsign);
    void increaseDelayMs(const QString &callsign);
    void decreaseDelayMs(const QString &callsign);

    static CallsignDelayCache &instance();

private:
    CallsignDelayCache() = default;

    static constexpr int delayDefault = 60;
    static constexpr int delayMin = 40;
    static constexpr int delayIncrement = 20;
    static constexpr int delayMax = 300;

    QHash<QString, int> m_delayCache;
    QHash<QString, int> successfulTransmissionsCache;
};

#endif // CALLSIGNDELAYCACHE_H
