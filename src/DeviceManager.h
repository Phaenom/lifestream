#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <Arduino.h>

class DeviceManager {
public:
    void begin();

    void assumeHostRole();
    void assumePlayerRole(uint8_t id);

    bool isHost() const;
    uint8_t getPlayerId() const;

    void setIsHost(bool value);

private:
    bool host = false;
    uint8_t playerId = 0;
    unsigned long lastUpdateTime = 0;
};

#endif // DEVICEMANAGER_H
