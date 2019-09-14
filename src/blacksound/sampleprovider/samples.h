#ifndef SAMPLES_H
#define SAMPLES_H

#include "blacksound/blacksoundexport.h"
#include "resourcesound.h"

class BLACKSOUND_EXPORT Samples
{
public:
    static Samples &instance();

    ResourceSound crackle() const;
    ResourceSound click() const;
    ResourceSound whiteNoise() const;

private:
    Samples();

    ResourceSound m_crackle;
    ResourceSound m_click;
    ResourceSound m_whiteNoise;
};

#endif // SAMPLES_H
