#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"
#include <algorithm>
#include <cmath>

using namespace std;

// Start of Actor implementation

Actor::Actor(int imageID, double startX, double startY, int dir, double size, int depth, StudentWorld* world)
    : GraphObject(imageID, startX, startY, dir, size, depth),
    m_world(world), m_active(true), m_verticalSpeed(0), m_horizontalSpeed(0), m_health(-1) {}

Actor::~Actor() {}

// Many Actors follow the same movement algorithm as defined in the spec,
// so here is a common method that dictates if and how many Actors move
bool Actor::move() {
    double vert_speed = verticalSpeed() - world()->ghostRacer()->verticalSpeed();
    double horiz_speed = horizontalSpeed();
    double new_y = getY() + vert_speed;
    double new_x = getX() + horiz_speed;
    moveTo(new_x, new_y);

    if (new_x < 0 || new_x > VIEW_WIDTH || new_y < 0 || new_y > VIEW_HEIGHT) {
        setActive(false);
        return false;
    }

    return true;
}

// Most Actors aren't "collision avoidance-worthy," so this returns false unless overridden
bool Actor::isCollisionAvoidanceWorthy() const {
    return false;
}

// Most Actors aren't damageable, so this returns false unless overridden
bool Actor::canBeDamaged() const {
    return false;
}

// Most Actors aren't affected by holy water, so this returns false unless overridden
bool Actor::affectedByHolyWater() const {
    return false;
}

// Unless overridden, Actors that are affected by holy water take 1 hit point of damage
// when they overlap with holy water projectiles
void Actor::onHitByHolyWater() {
    decHealth(1);
}

// Returns the pointer to the StudentWorld that controls all the Actors
StudentWorld* Actor::world() const {
    return m_world;
}

// Returns whether an Actor is considered active
bool Actor::isActive() const {
    return m_active;
}

// Change whether an Actor is considered active
void Actor::setActive(bool active) {
    m_active = active;
}

// Return the vertical speed of an Actor
double Actor::verticalSpeed() const {
    return m_verticalSpeed;
}

// Change the vertical speed of an Actor
void Actor::setVerticalSpeed(double verticalSpeed) {
    m_verticalSpeed = verticalSpeed;
}

// Return the horizontal speed of an Actor
double Actor::horizontalSpeed() const {
    return m_horizontalSpeed;
}

// Change the horizontal speed of an Actor
void Actor::setHorizontalSpeed(double horizontalSpeed) {
    m_horizontalSpeed = horizontalSpeed;
}

// Return an Actor's health
int Actor::health() const {
    return m_health;
}

// Set the health of an Actor if it can be damaged
void Actor::setHealth(int health) {
    if (canBeDamaged()) {
        m_health = health;
    }
}

// Decrease the health of an Actor if it can be damaged
void Actor::decHealth(int amt) {
    if (canBeDamaged()) {
        m_health = max(0, m_health - amt);
    }
}

// Increase the health of an Actor if it can be damaged
void Actor::incHealth(int amt) {
    if (canBeDamaged()) {
        m_health = min(100, m_health + amt);
    }
}

// End of Actor implementation
// Start of Damageable implementation

// Damageable constructor that invokes Actor's constructor and sets the Actor's health
Damageable::Damageable(int imageID, double startX, double startY, int dir, double size, int depth, StudentWorld* world, int health) : Actor(imageID, startX, startY, dir, size, depth, world) {
    setHealth(health);
}

// All Actors that inherit Damageable can be damaged (i.e. their health can be modified)
bool Damageable::canBeDamaged() const {
    return true;
}

// End of Damageable implementation
// Start of Pedestrian implementation

// Pedestrian constructor that invokes Damageable's constructor and initializes the movement plan distance to 0
Pedestrian::Pedestrian(int imageID, double startX, double startY, int dir, double size, int depth, StudentWorld* world, int health)
    : Damageable(imageID, startX, startY, dir, size, depth, world, health), m_movementPlanDistance(0) {}

// All Actors that inherit Pedestrian are considered "collision avoidance-worthy"
bool Pedestrian::isCollisionAvoidanceWorthy() const {
    return true;
}

// All Actors that inherit Pedestrian are affected by holy water
bool Pedestrian::affectedByHolyWater() const {
    return true;
}

// Return a Pedestrian's movement plan distance
int Pedestrian::movementPlanDistance() const {
    return m_movementPlanDistance;
}

// Set the movement plan distance of a Pedestrian
void Pedestrian::setMovementPlanDistance(int movementPlanDistance) {
    m_movementPlanDistance = movementPlanDistance;
}

// Decrease the movement plan distance of a Pedestrian by 1
void Pedestrian::decMovementPlanDistance() {
    m_movementPlanDistance--;
}

// End of Pedestrian implementation
// Start of BorderLine implementation

// BorderLine constructor that invokes Actor's constructor and sets the vertical and horizontal speeds to appropriate values as defined in the spec
BorderLine::BorderLine(int imageID, double startX, double startY, StudentWorld* world) : Actor(imageID, startX, startY, BORDER_LINE_DIR, BORDER_LINE_SIZE, BORDER_LINE_DEPTH, world) {
    setVerticalSpeed(BORDER_LINE_VS);
    setHorizontalSpeed(BORDER_LINE_HS);
}

// BorderLines do nothing except move every tick
void BorderLine::doSomething() {
    move();
}

// End of BorderLine implementation
// Start of Goodie implementation

// Goodie constructor that invokes Actor's constructor and sets the vertical and horizontal speeds to appropriate values as defined in the spec
Goodie::Goodie(int imageID, double startX, double startY, int dir, double size, int depth, StudentWorld* world) : Actor(imageID, startX, startY, dir, size, depth, world) {
    setVerticalSpeed(GOODIE_VS);
    setHorizontalSpeed(GOODIE_HS);
}

void Goodie::doSomething() {
    // If the Goodie can't move (i.e. it isn't on the screen anymore), return
    if (!move()) {
        return;
    }

    // If the Goodie overlaps with GhostRacer, call onPickup(), increase the score, and set the Goodie to be inactive
    if (overlaps(this, world()->ghostRacer())) {
        onPickup();
        world()->increaseScore(getScoreIncrease());
        setActive(false);
    }

    // If a Goodie has other defined behavior, make sure that behavior is executed
    doOtherBehavior();
}

// This method is overridden to define additional behavior for Goodies
void Goodie::doOtherBehavior() {}

// End of Goodie implementation
// Start of GhostRacer implementation

// GhostRacer constructor that invokes Damageable's constructor and sets the vertical and horizontal speeds to appropriate values as defined in the spec
GhostRacer::GhostRacer(StudentWorld* world) : Damageable(IID_GHOST_RACER, GHOST_RACER_START_X, GHOST_RACER_START_Y, GHOST_RACER_DIR, GHOST_RACER_SIZE, GHOST_RACER_DEPTH,
    world, GHOST_RACER_HEALTH), m_numSprays(GHOST_RACER_NUM_SPRAYS), m_soulsSaved(0) {
    setVerticalSpeed(GHOST_RACER_VS);
    setHorizontalSpeed(GHOST_RACER_HS);
}

void GhostRacer::doSomething() {
    // If GhostRacer doesn't have any health remaining, set it to be inactive and play its death noise
    if (health() <= 0) {
        world()->playSound(SOUND_PLAYER_DIE);
        setActive(false);
        return;
    }

    if (getX() <= world()->leftEdge()) {
        // If GhostRacer goes too far left, damage it and adjust its direction
        if (getDirection() > 90) {
            decHealth(10);
        }
        setDirection(82);
        world()->playSound(SOUND_VEHICLE_CRASH);
    } else if (getX() >= world()->rightEdge()) {
        // If GhostRacer goes too far right, damage it and adjust its direction
        if (getDirection() < 90) {
            decHealth(10);
        }
        setDirection(98);
        world()->playSound(SOUND_VEHICLE_CRASH);
    } else {
        int ch;
        if (world()->getKey(ch)) {
            switch (ch) {
            case KEY_PRESS_SPACE:
                // Shoot a spray in the correct direction if the user presses space
                if (m_numSprays > 0) {
                    // We need to define PI to convert our direction to radians
                    const double PI = acos(-1);

                    double direction = getDirection() * (PI / 180);

                    // To account for shooting a spray at an angle other than 90 degrees
                    double delta_x = SPRITE_HEIGHT * cos(direction);
                    double delta_y = SPRITE_HEIGHT * sin(direction);

                    double startX = getX() + delta_x;
                    double startY = getY() + delta_y;

                    world()->addHolyWaterProjectile(startX, startY, getDirection());

                    world()->playSound(SOUND_PLAYER_SPRAY);

                    m_numSprays--;
                }
                break;
            case KEY_PRESS_LEFT:
                // Increase GhostRacer's direction to move left if the user presses the left button
                if (getDirection() < 114) {
                    setDirection(getDirection() + 8);
                }
                break;
            case KEY_PRESS_RIGHT:
                // Increase GhostRacer's direction to move right if the user presses the right button
                if (getDirection() > 66) {
                    setDirection(getDirection() - 8);
                }
                break;
            case KEY_PRESS_UP:
                // Increase GhostRacer's vertical speed if the user presses the up button
                if (verticalSpeed() < 5) {
                    setVerticalSpeed(verticalSpeed() + 1);
                }
                break;
            case KEY_PRESS_DOWN:
                // Decrease GhostRacer's vertical speed if the user presses the down button
                if (verticalSpeed() > -1) {
                    setVerticalSpeed(verticalSpeed() - 1);
                }
                break;
            }
        }
    }

    // Give GhostRacer the chance to move
    move();
}

bool GhostRacer::move() {
    // We need to define PI to convert our direction to radians
    const double PI = acos(-1);

    // GhostRacer movement algorithm as defined in the spec
    double max_shift_per_tick = 4;
    double direction = getDirection() * (PI / 180);
    double delta_x = cos(direction) * max_shift_per_tick;
    double cur_x = getX();
    double cur_y = getY();
    moveTo(cur_x + delta_x, cur_y);

    return true;
}

// GhostRacer is considered "collision avoidance-worthy"
bool GhostRacer::isCollisionAvoidanceWorthy() const {
    return true;
}

// Return the number of sprays GhostRacer has
int GhostRacer::numSprays() const {
    return m_numSprays;
}

// Increase the number of sprays GhostRacer has by a given amount
void GhostRacer::incNumSprays(int amt) {
    m_numSprays += amt;
}

// Return the number of souls GhostRacer has saved
int GhostRacer::soulsSaved() const {
    return m_soulsSaved;
}

// Increase the number of souls GhostRacer has saved by a given amount
void GhostRacer::incSoulsSaved() {
    m_soulsSaved++;
}

// End of GhostRacer implementation
// Start of YellowBorderLine implementation

// Trivial constructor for YellowBorderLine that inherits BorderLine
YellowBorderLine::YellowBorderLine(double startX, double startY, StudentWorld* world) : BorderLine(IID_YELLOW_BORDER_LINE, startX, startY, world) {}

// End of YellowBorderLine implementation
// Start of WhiteBorderLine implementation

// Trivial constructor for WhiteBorderLine that inherits BorderLine
WhiteBorderLine::WhiteBorderLine(double startX, double startY, StudentWorld* world) : BorderLine(IID_WHITE_BORDER_LINE, startX, startY, world) {}

// End of WhiteBorderLine implementation
// Start of HumanPedestrian implementation

// HumanPedestrian constructor that invokes Pedestrian's constructor and sets the vertical and horizontal speeds to appropriate values as defined in the spec
HumanPedestrian::HumanPedestrian(double startX, double startY, StudentWorld* world) : Pedestrian(IID_HUMAN_PED, startX, startY, HUMAN_PED_DIR, HUMAN_PED_SIZE, HUMAN_PED_DEPTH, world, HUMAN_PED_HEALTH) {
    setVerticalSpeed(HUMAN_PED_VS);
    setHorizontalSpeed(HUMAN_PED_HS);
}

void HumanPedestrian::doSomething() {
    // If a HumanPedestrian overlaps with GhostRacer, end the level
    if (overlaps(this, world()->ghostRacer())) {
        world()->decLives();
        world()->ghostRacer()->setActive(false);
        return;
    }

    // Give a HumanPedestrian a chance to move within bounds
    if (!move()) {
        return;
    }

    // Behavior to determine how a HumanPedestrian move as defined in the spec
    decMovementPlanDistance();
    if (movementPlanDistance() > 0) {
        return;
    }

    int rand_horiz_speed = 0;
    while (rand_horiz_speed == 0) {
        rand_horiz_speed = randInt(-3, 3);
    }

    setHorizontalSpeed(rand_horiz_speed);
    setMovementPlanDistance(randInt(4, 32));

    if (horizontalSpeed() < 0) {
        setDirection(180);
    } else {
        setDirection(0);
    }
}

// When a HumanPedestrian is hit by holy water, its horizontal speed and direction must flip, and it must play its hurt sound
void HumanPedestrian::onHitByHolyWater() {
    setHorizontalSpeed(horizontalSpeed() * -1);
    setDirection(getDirection() == 180 ? 0 : 180);
    world()->playSound(SOUND_PED_HURT);
}

// End of HumanPedestrian implementation
// Start of ZombiePedestrian implementation

// ZombiePedestrian constructor that invokes Pedestrian's constructor and sets the vertical and horizontal speeds to appropriate values as defined in the spec
ZombiePedestrian::ZombiePedestrian(double startX, double startY, StudentWorld* world) : Pedestrian(IID_ZOMBIE_PED, startX, startY, ZOMBIE_PED_DIR, ZOMBIE_PED_SIZE, ZOMBIE_PED_DEPTH,
    world, ZOMBIE_PED_HEALTH), m_ticksUntilGrunt(0) {
    setVerticalSpeed(ZOMBIE_PED_VS);
    setHorizontalSpeed(ZOMBIE_PED_HS);
}

void ZombiePedestrian::doSomething() {
    // If a ZombiePedestrian doesn't have any health remaining, set it to be inactive, play its death noise, and increase the score
    if (health() <= 0) {
        world()->increaseScore(ZOMBIE_PED_KILL_POINTS);
        world()->playSound(SOUND_PED_DIE);
        setActive(false);
        return;
    }

    // If a ZombiePedestrian overlaps with GhostRacer, kill the ZombiePedestrian and decrease GhostRacer's health
    if (overlaps(this, world()->ghostRacer())) {
        world()->ghostRacer()->decHealth(5);
        decHealth(2);
        return;
    }

    if (fabs(getX() - world()->ghostRacer()->getX()) < 30 && getY() > world()->ghostRacer()->getY()) {
        setDirection(270);

        if (getX() < world()->ghostRacer()->getX()) { // zombie ped is to the left of Ghost Racer
            setHorizontalSpeed(1);
        } else if (getX() > world()->ghostRacer()->getX()) { // zombie ped is to the right of Ghost Racer
            setHorizontalSpeed(-1);
        } else {
            setHorizontalSpeed(0);
        }

        m_ticksUntilGrunt--;

        if (m_ticksUntilGrunt <= 0) {
            world()->playSound(SOUND_ZOMBIE_ATTACK);
            m_ticksUntilGrunt = 20;
        }
    }

    // Give a ZombiePedestrian a chance to move within bounds
    if (!move()) {
        return;
    }

    if (movementPlanDistance() > 0) {
        decMovementPlanDistance();
        return;
    }

    int rand_horiz_speed = 0;
    while (rand_horiz_speed == 0) {
        rand_horiz_speed = randInt(-3, 3);
    }

    setHorizontalSpeed(rand_horiz_speed);
    setMovementPlanDistance(randInt(4, 32));

    if (horizontalSpeed() < 0) {
        setDirection(180);
    } else {
        setDirection(0);
    }
}

// When a ZombiePedestrian is hit by holy water, it must take 1 hit point of damage. If it dies by holy water, then give it a chance
// to create a HealingGoodie at its position. Otherwise, play its hurt noise.
void ZombiePedestrian::onHitByHolyWater() {
    Actor::onHitByHolyWater();

    if (health() <= 0) {
        int rand = randInt(1, 5);
        if (rand == 1) {
            world()->addHealingGoodie(getX(), getY());
        }
    } else {
        world()->playSound(SOUND_PED_HURT);
    }
}

// End of ZombiePedestrian implementation
// Start of ZombieCab implementation

// ZombieCab constructor that invokes Damageable's constructor and sets the vertical and horizontal speeds to appropriate values as defined in the spec
ZombieCab::ZombieCab(double startX, double startY, StudentWorld* world, double verticalSpeed, int lane) : Damageable(IID_ZOMBIE_CAB, startX, startY,
    ZOMBIE_CAB_DIR, ZOMBIE_CAB_SIZE, ZOMBIE_CAB_DEPTH, world, ZOMBIE_CAB_HEALTH), m_lane(lane), m_planLength(0), m_hasDamagedGhostRacer(false) {
    setVerticalSpeed(verticalSpeed);
    setHorizontalSpeed(ZOMBIE_CAB_HS);
}

void ZombieCab::doSomething() {
    // If a ZombieCab doesn't have any health remaining, set it to be inactive, play its death noise, and increase the score
    if (health() <= 0) {
        world()->increaseScore(ZOMBIE_CAB_KILL_POINTS);
        world()->playSound(SOUND_VEHICLE_DIE);
        setActive(false);
        return;
    }

    // If a ZombieCab overlaps with GhostRacer and it hasn't already damaged GhostRacer, do the correct behavior as defined in the spec
    if (overlaps(this, world()->ghostRacer())) {
        if (!m_hasDamagedGhostRacer) {
            world()->playSound(SOUND_VEHICLE_CRASH);
            world()->ghostRacer()->decHealth(20);

            if (getX() <= world()->ghostRacer()->getX()) {
                setHorizontalSpeed(-5);
                setDirection(120 + randInt(0, 19));
            } else if (getX() > world()->ghostRacer()->getX()) {
                setHorizontalSpeed(5);
                setDirection(60 - randInt(0, 19));
            }

            m_hasDamagedGhostRacer = true;
        }
    }

    // Give a ZombiePedestrian a chance to move within bounds
    if (!move()) {
        return;
    }

    if (verticalSpeed() > world()->ghostRacer()->verticalSpeed()) {
        Actor* closestInFront = nullptr;

        for (unsigned int i = 0; i < world()->actors().size(); i++) {
            Actor* actor = world()->actors().at(i);

            // We don't want to compare the zombie cab to itself
            if (actor == this) {
                continue;
            }

            // If the Actor in question is "collision avoidance-worthy" and it's in the same lane as the ZombieCab and it's in front of the ZombieCab,
            // then this might be the closest Actor in front of the ZombieCab
            if (actor->isCollisionAvoidanceWorthy() && world()->isInLane(actor, m_lane) && actor->getY() > getY()) {
                if (closestInFront == nullptr || actor->getY() < closestInFront->getY()) {
                    closestInFront = actor;
                }
            }
        }

        // If we found an Actor closest in front of the ZombieCab within bounds, then decrease the ZombieCab's vertical speed
        if (closestInFront != nullptr && fabs(getX() - closestInFront->getX()) < 96) {
            setVerticalSpeed(verticalSpeed() - 0.5);
            return;
        }
    }

    if (verticalSpeed() <= world()->ghostRacer()->verticalSpeed()) {
        Actor* closestBehind = nullptr;

        for (unsigned int i = 0; i < world()->actors().size(); i++) {
            Actor* actor = world()->actors().at(i);

            // We don't want to compare the zombie cab to itself
            if (actor == this) {
                continue;
            }

            // If the Actor in question is "collision avoidance-worthy" and it's in the same lane as the ZombieCab and it's behind the ZombieCab,
            // then this might be the closest Actor behind the ZombieCab
            if (actor->isCollisionAvoidanceWorthy() && world()->isInLane(actor, m_lane) && actor->getY() < getY()) {
                if (closestBehind == nullptr || actor->getY() > closestBehind->getY()) {
                    closestBehind = actor;
                }
            }
        }

        // If we found an Actor closest behind the ZombieCab within bounds, then increase the ZombieCab's vertical speed
        if (closestBehind != nullptr && fabs(getX() - closestBehind->getX()) < 96) {
            setVerticalSpeed(verticalSpeed() + 0.5);
            return;
        }
    }

    m_planLength--;
    if (m_planLength > 0) {
        return;
    }

    m_planLength = randInt(4, 32);
    setVerticalSpeed(verticalSpeed() + randInt(-2, 2));
}

// ZombieCabs are considered "collision avoidance-worthy"
bool ZombieCab::isCollisionAvoidanceWorthy() const {
    return true;
}

// ZombieCabs are affected by holy water
bool ZombieCab::affectedByHolyWater() const {
    return true;
}

// When a ZombieCab is hit by holy water, it must take 1 hit point of damage. If it dies by holy water, then give it a chance
// to create an OilSlick at its position. Otherwise, play its hurt noise.
void ZombieCab::onHitByHolyWater() {
    Actor::onHitByHolyWater();

    if (health() <= 0) {
        int rand = randInt(1, 5);
        if (rand == 1) {
            world()->addOilSlick(getX(), getY());
        }
    } else {
        world()->playSound(SOUND_VEHICLE_HURT);
    }
}

// End of ZombieCab implementation
// Start of OilSlick implementation

// OilSlick constructor that invokes Actor's constructor and sets the vertical and horizontal speeds to appropriate values as defined in the spec
OilSlick::OilSlick(double startX, double startY, StudentWorld* world) : Actor(IID_OIL_SLICK, startX, startY, OIL_SLICK_DIR, randInt(2, 5), OIL_SLICK_DEPTH, world) {
    setVerticalSpeed(OIL_SLICK_VS);
    setHorizontalSpeed(OIL_SLICK_HS);
}

void OilSlick::doSomething() {
    // Give an OilSlick a chance to move within bounds
    if (!move()) {
        return;
    }

    // If an OilSlick overlaps with GhostRacer, do the correct behavior as defined in the spec
    if (overlaps(this, world()->ghostRacer())) {
        world()->playSound(SOUND_OIL_SLICK);
        int dir = randInt(0, 1);    // 0 = clockwise, 1 = counterclockwise

        int oldDir = world()->ghostRacer()->getDirection();
        if (dir == 0) {
            world()->ghostRacer()->setDirection(min(120, oldDir + randInt(5, 20)));
        } else if (dir == 1) {
            world()->ghostRacer()->setDirection(max(60, oldDir - randInt(5, 20)));
        }
    }
}

// End of OilSlick implementation
// Start of HolyWaterProjectile implementation

// HolyWaterProjectile constructor that invokes Actor's constructor and initializes its distance traveled to 0
HolyWaterProjectile::HolyWaterProjectile(double startX, double startY, int dir, StudentWorld* world)
    : Actor(IID_HOLY_WATER_PROJECTILE, startX, startY, dir, HOLY_WATER_PROJ_SIZE, HOLY_WATER_PROJ_DEPTH, world), m_distanceTraveled(0) {}

void HolyWaterProjectile::doSomething() {
    for (unsigned int i = 0; i < world()->actors().size(); i++) {
        Actor* actor = world()->actors().at(i);

        // We don't want holy water to affect actors that aren't affected by holy water
        if (!actor->affectedByHolyWater()) {
            continue;
        }

        // If the Actor being examined overlaps with the holy water, then it must do what it does when it's hit by holy water
        if (overlaps(this, actor)) {
            actor->onHitByHolyWater();
            setActive(false);
            return;
        }
    }

    // Give a HolyWaterProjectile a chance to move within bounds
    if (!move()) {
        return;
    }

    // If the holy water has traveled up to or past its max distance, set it to be inactive
    if (m_distanceTraveled >= HOLY_WATER_PROJ_MAX_TRAVEL_DIST) {
        setActive(false);
    }
}

// Holy water follows a unique movement algorithm, so it must
bool HolyWaterProjectile::move() {
    // Move forward by a distance of SPRITE_HEIGHT and increase the distance traveled
    moveForward(SPRITE_HEIGHT);
    m_distanceTraveled += SPRITE_HEIGHT;

    if (getX() < 0 || getX() > VIEW_WIDTH || getY() < 0 || getY() > VIEW_HEIGHT) {
        setActive(false);
        return false;
    }

    return true;
}

// End of HolyWaterProjectile implementation
// Start of HealingGoodie implementation

// HealingGoodie constructor that invokes Goodie's constructor
HealingGoodie::HealingGoodie(double startX, double startY, StudentWorld* world)
    : Goodie(IID_HEAL_GOODIE, startX, startY, HEALING_GOODIE_DIR, HEALING_GOODIE_SIZE, HEALING_GOODIE_DEPTH, world) {}

// Picking up a HealingGoodie increase the score by 250 points
int HealingGoodie::getScoreIncrease() const {
    return 250;
}

// Picking up a HealingGoodie increases GhostRacer's health
void HealingGoodie::onPickup() {
    world()->ghostRacer()->incHealth(10);
    world()->playSound(SOUND_GOT_GOODIE);
}

// HealingGoodies are affected by holy water
bool HealingGoodie::affectedByHolyWater() const {
    return true;
}

void HealingGoodie::onHitByHolyWater() {
    // Destroy a HealingGoodie that has been hit by a holy water projectile
    setActive(false);
}

// End of HealingGoodie implementation
// Start of HolyWaterGoodie implementation

// HolyWaterGoodie constructor that invokes Goodie's constructor
HolyWaterGoodie::HolyWaterGoodie(double startX, double startY, StudentWorld* world)
    : Goodie(IID_HOLY_WATER_GOODIE, startX, startY, HOLY_WATER_GOODIE_DIR, HOLY_WATER_GOODIE_SIZE, HOLY_WATER_GOODIE_DEPTH, world) {}

// Picking up a HealingGoodie increase the score by 50 points
int HolyWaterGoodie::getScoreIncrease() const {
    return 50;
}

// Picking up a HealingGoodie increases the number of sprays GhostRacer has
void HolyWaterGoodie::onPickup() {
    world()->ghostRacer()->incNumSprays(10);
    world()->playSound(SOUND_GOT_GOODIE);
}

// HolyWaterGoodies are affected by holy water
bool HolyWaterGoodie::affectedByHolyWater() const {
    return true;
}

void HolyWaterGoodie::onHitByHolyWater() {
    // Destroy a HolyWaterGoodie that has been hit by a holy water projectile
    setActive(false);
}

// End of HolyWaterGoodie implementation
// Start of LostSoulGoodie implementation

// LostSoulGoodie constructor that invokes Goodie's constructor
LostSoulGoodie::LostSoulGoodie(double startX, double startY, StudentWorld* world)
    : Goodie(IID_SOUL_GOODIE, startX, startY, LOST_SOUL_GOODIE_DIR, LOST_SOUL_GOODIE_SIZE, LOST_SOUL_GOODIE_DEPTH, world) {}

// Picking up a HealingGoodie increase the score by 100 points
int LostSoulGoodie::getScoreIncrease() const {
    return 100;
}

// Picking up a HealingGoodie increases the number of souls GhostRacer has saved by 1
void LostSoulGoodie::onPickup() {
    world()->ghostRacer()->incSoulsSaved();
    world()->playSound(SOUND_GOT_SOUL);
}

void LostSoulGoodie::doOtherBehavior() {
    // Lost Soul Goodies rotate themselves by 10 degrees clockwise every tick
    setDirection(getDirection() + 10);
}

// End of LostSoulGoodie implementation
// Start of non-member function implementation

// Determines if two Actors overlap as defined in the spec
bool overlaps(Actor* a1, Actor* a2) {
    double delta_x = fabs(a1->getX() - a2->getX());
    double delta_y = fabs(a1->getY() - a2->getY());
    double radius_sum = a1->getRadius() + a2->getRadius();

    return (delta_x < (radius_sum * 0.25)) && (delta_y < (radius_sum * 0.6));
}

// End of non-member function implementation