#ifndef TRACK_H
#define TRACK_H

#include "ArdourControllable.h"

class Track : public ArdourControllable
{
    bool soloed;
    bool muted;
    char panDir;
    char panWidth;
public:
    void setSoled(bool mod=true);
    bool getSoloed();
    void setMuted(bool mod=true);
    bool getMuted();
    char getPanDir(void);
    void setPanDir(char);
    char getPanWidth(void);
    void setPanWidth(char);
    void sendToArdour(Jack *pJack);
    Track(char id) : ArdourControllable(id) {}
    Track(char id,char gain) : ArdourControllable(id,gain) {}
};

#endif // TRACK_H
