#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

// Start of base classes

// Base class that all Actors in our game inherit
class Actor : public GraphObject {
public:
    Actor(int imageID, double startX, double startY, int dir, double size, int depth, StudentWorld* world);
    virtual ~Actor();

    // Each Actor does something each tick, but does different things. So this is a pure virtual function.
    virtual void doSomething() = 0;
    // Returns true if the Actor moves within its specified bounds, false otherwise
    virtual bool move();
    // Returns whether an Actor is considered "collision avoidance-worthy"
    virtual bool isCollisionAvoidanceWorthy() const;
    // Returns whether an Actor is allowed to be damaged (this includes modifying its health in any way)
    virtual bool canBeDamaged() const;
    // Returns whether an Actor is affected when its sprite overlaps with holy water
    virtual bool affectedByHolyWater() const;
    // What happens when an Actor is hit by holy water, granted it is affected by holy water
    virtual void onHitByHolyWater();

    // Returns a pointer to the StudentWorld that controls all behavior of our game
    StudentWorld* world() const;

    // Methods concerning the state of an Actor (i.e. whether it is active at any given tick)
    bool isActive() const;
    void setActive(bool active);

    // Methods concerning the vertical speed of an Actor
    double verticalSpeed() const;
    void setVerticalSpeed(double verticalSpeed);

    // Methods concerning the horizontal speed of an Actor
    double horizontalSpeed() const;
    void setHorizontalSpeed(double horizontalSpeed);

    // Methods concerning the health of an Actor, granted it is allowed to be damaged
    int health() const;
    void setHealth(int health);
    void decHealth(int amt);
    void incHealth(int amt);
private:
    StudentWorld* m_world;
    bool m_active;
    double m_verticalSpeed;
    double m_horizontalSpeed;
    int m_health;
};

// Base class for all Actors that can be damaged or whose health is modified in the game
class Damageable : public Actor {
public:
    Damageable(int imageID, double startX, double startY, int dir, double size, int depth, StudentWorld* world, int health);

    // As its name suggests, all Damageables can be damaged, so this method needs to be overridden
    bool canBeDamaged() const;
};

// Base class for Pedestrians (i.e. HumanPedestrian, ZombiePedestrian)
// All Pedestrians can be damaged, so Pedestrian inherits Damageable
class Pedestrian : public Damageable {
public:
    Pedestrian(int imageID, double startX, double startY, int dir, double size, int depth, StudentWorld* world, int health);

    // All Pedestrians are collision avoidance worthy actors
    bool isCollisionAvoidanceWorthy() const;

    // All Pedestrians are affected by holy water
    bool affectedByHolyWater() const;

    // Methods concerning a Pedestrian's movement plan as dictated in its doSomething()
    int movementPlanDistance() const;
    void setMovementPlanDistance(int movementPlanDistance);
    void decMovementPlanDistance();
private:
    int m_movementPlanDistance;
};

const int BORDER_LINE_DIR = 0;
const double BORDER_LINE_SIZE = 2;
const int BORDER_LINE_DEPTH = 2;
const double BORDER_LINE_VS = -4;
const double BORDER_LINE_HS = 0;

// Base class for BorderLines (i.e. YellowBorderLine, WhiteBorderLine)
class BorderLine : public Actor {
public:
    BorderLine(int imageID, double startX, double startY, StudentWorld* world);

    void doSomething();
};

const double GOODIE_VS = -4;
const double GOODIE_HS = 0;

// Base class for Goodies (i.e. HealingGoodie, HolyWaterGoodie, etc.)
class Goodie : public Actor {
public:
    Goodie(int imageID, double startX, double startY, int dir, double size, int depth, StudentWorld* world);

    void doSomething();

    // The amount of points Ghost Racer will attain when a Goodie is picked up
    virtual int getScoreIncrease() const = 0;
    // What happens when Ghost Racer picks up a Goodie
    virtual void onPickup() = 0;
    // Some Goodies have behavior outside of simply being picked up by Ghost Racer
    // This method is what happens after Ghost Racer picks up a Goodie
    virtual void doOtherBehavior();
};

// End of base classes
// Start of derived classes

const int GHOST_RACER_START_X = 128;
const int GHOST_RACER_START_Y = 32;
const int GHOST_RACER_DIR = 90;
const double GHOST_RACER_SIZE = 4;
const int GHOST_RACER_DEPTH = 0;
const double GHOST_RACER_VS = 0;
const double GHOST_RACER_HS = 0;
const int GHOST_RACER_HEALTH = 100;
const int GHOST_RACER_NUM_SPRAYS = 10;

class GhostRacer : public Damageable {
public:
    GhostRacer(StudentWorld* world);

    void doSomething();
    // GhostRacer follows a unique movement algorithm, so this needs to be overridden
    bool move();
    // GhostRacer is considered "collision avoidance-worthy"
    bool isCollisionAvoidanceWorthy() const;

    // Methods concerning the number of holy water sprays GhostRacer has
    int numSprays() const;
    void incNumSprays(int amt);

    // Methods concerning the number of souls GhostRacer has saved
    int soulsSaved() const;
    void incSoulsSaved();
private:
    int m_numSprays;
    int m_soulsSaved;
};

class YellowBorderLine : public BorderLine {
public:
    YellowBorderLine(double startX, double startY, StudentWorld* world);
};

class WhiteBorderLine : public BorderLine {
public:
    WhiteBorderLine(double startX, double startY, StudentWorld* world);
};

const int HUMAN_PED_DIR = 0;
const double HUMAN_PED_SIZE = 2;
const int HUMAN_PED_DEPTH = 0;
const double HUMAN_PED_VS = -4;
const double HUMAN_PED_HS = 0;
const int HUMAN_PED_HEALTH = 2;

class HumanPedestrian : public Pedestrian {
public:
    HumanPedestrian(double startX, double startY, StudentWorld* world);

    void doSomething();
    // Pedestrians are affected by holy water, so HumanPedestrian has to respond to being hit by holy water
    void onHitByHolyWater();
};

const int ZOMBIE_PED_DIR = 0;
const double ZOMBIE_PED_SIZE = 3;
const int ZOMBIE_PED_DEPTH = 0;
const double ZOMBIE_PED_VS = -4;
const double ZOMBIE_PED_HS = 0;
const int ZOMBIE_PED_HEALTH = 2;
const int ZOMBIE_PED_KILL_POINTS = 150;

class ZombiePedestrian : public Pedestrian {
public:
    ZombiePedestrian(double startX, double startY, StudentWorld* world);

    void doSomething();
    // Pedestrians are affected by holy water, so ZombiePedestrian has to respond to being hit by holy water
    void onHitByHolyWater();
private:
    int m_ticksUntilGrunt;
};

const int ZOMBIE_CAB_DIR = 90;
const double ZOMBIE_CAB_SIZE = 4;
const int ZOMBIE_CAB_DEPTH = 0;
const double ZOMBIE_CAB_HS = 0;
const int ZOMBIE_CAB_HEALTH = 3;
const int ZOMBIE_CAB_KILL_POINTS = 200;

class ZombieCab : public Damageable {
public:
    ZombieCab(double startX, double startY, StudentWorld* world, double verticalSpeed, int lane);

    void doSomething();
    // ZombieCabs are considered "collision avoidance-worthy"
    bool isCollisionAvoidanceWorthy() const;
    // ZombieCabs are affected by holy water
    bool affectedByHolyWater() const;
    // What happens when a ZombieCab is hit by holy water
    void onHitByHolyWater();
private:
    int m_lane;
    int m_planLength;
    bool m_hasDamagedGhostRacer;
};

const int OIL_SLICK_DIR = 0;
const int OIL_SLICK_DEPTH = 2;
const double OIL_SLICK_VS = -4;
const double OIL_SLICK_HS = 0;

class OilSlick : public Actor {
public:
    OilSlick(double startX, double startY, StudentWorld* world);

    void doSomething();
};

const double HOLY_WATER_PROJ_SIZE = 1;
const int HOLY_WATER_PROJ_DEPTH = 1;
const int HOLY_WATER_PROJ_MAX_TRAVEL_DIST = 160;

class HolyWaterProjectile : public Actor {
public:
    HolyWaterProjectile(double startX, double startY, int dir, StudentWorld* world);

    void doSomething();
    // HolyWaterProjectiles follow a unique movement algorithm, so this needs to be overridden
    bool move();
private:
    int m_distanceTraveled;
};

const int HEALING_GOODIE_DIR = 0;
const double HEALING_GOODIE_SIZE = 2;
const int HEALING_GOODIE_DEPTH = 2;

class HealingGoodie : public Goodie {
public:
    HealingGoodie(double startX, double startY, StudentWorld* world);

    // The points attained by picking up a HealingGoodie must be defined
    int getScoreIncrease() const;
    // What happens when a HealingGoodie is picked up
    void onPickup();

    // HealingGoodies are affected by holy water
    bool affectedByHolyWater() const;
    // HealingGoodies are destroyed by holy water
    void onHitByHolyWater();
};

const int HOLY_WATER_GOODIE_DIR = 90;
const double HOLY_WATER_GOODIE_SIZE = 2;
const int HOLY_WATER_GOODIE_DEPTH = 2;

class HolyWaterGoodie : public Goodie {
public:
    HolyWaterGoodie(double startX, double startY, StudentWorld* world);

    // The points attained by picking up a HolyWaterGoodie must be defined
    int getScoreIncrease() const;
    // What happens when a HolyWaterGoodie is picked up
    void onPickup();

    // HolyWaterGoodies are affected by holy water
    bool affectedByHolyWater() const;
    // HolyWaterGoodies are destroyed by holy water
    void onHitByHolyWater();
};

const int LOST_SOUL_GOODIE_DIR = 0;
const double LOST_SOUL_GOODIE_SIZE = 4;
const int LOST_SOUL_GOODIE_DEPTH = 2;

class LostSoulGoodie : public Goodie {
public:
    LostSoulGoodie(double startX, double startY, StudentWorld* world);

    // The points attained by picking up a LostSoulGoodie must be defined
    int getScoreIncrease() const;
    // What happens when a LostSoulGoodie is picked up
    void onPickup();
    // LostSoulGoodies rotate themselves every tick, so they have other behavior that needs to be defined
    void doOtherBehavior();
};

// End of derived classes
// Start of non-member functions

// Returns whether two Actors overlap with each other
bool overlaps(Actor* a1, Actor* a2);

// End of non-member functions

#endif // ACTOR_H_