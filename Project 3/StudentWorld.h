#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <vector>

// Need to tell the compiler that there exist classes called Actor and GhostRacer
class Actor;
class GhostRacer;

class StudentWorld : public GameWorld {
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();

    virtual int init();
    virtual int move();
    virtual void cleanUp();

    // Return a container holding pointers to all the Actors currently active in the game
    std::vector<Actor*> actors() const;
    // Return a pointer to GhostRacer
    GhostRacer* ghostRacer() const;

    // Return the left and right edges of the road respectively
    int leftEdge() const;
    int rightEdge() const;

    // Method to determine whether new BorderLines should be added during a tick
    void addBorderLines();
    // Method to determine whether new ZombieCabs should be added during a tick
    void addZombieCabs();
    // Method to add a new OilSlick, given a starting X and a starting Y
    void addOilSlick(double startX, double startY);
    // Method to determine whether new OilSlicks should be added during a tick
    void addOilSlicks();
    // Method to determine whether new ZombiePedestrians should be added during a tick
    void addZombiePeds();
    // Method to determine whether new HumanPedestrians should be added during a tick
    void addHumanPeds();
    // Method to determine whether new HolyWaterGoodies should be added during a tick
    void addHolyWaterRefillGoodies();
    // Method to determine whether new LostSoulGoodies should be added during a tick
    void addLostSoulGoodies();
    // Method to add a new HolyWaterProjectile, given a starting X and a starting Y
    void addHolyWaterProjectile(double startX, double startY, int dir);
    // Method to add a new HealingGoodie, given a starting X and a starting Y
    void addHealingGoodie(double startX, double startY);

    // Return whether an Actor is in a given lane
    bool isInLane(Actor* actor, int lane);
private:
    std::vector<Actor*> m_actors;
    GhostRacer* m_ghostRacer;
    int m_bonusScore;
    int m_lastWhiteY;
};

#endif // STUDENTWORLD_H_