#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"
#include <algorithm>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

GameWorld* createStudentWorld(string assetPath) {
    return new StudentWorld(assetPath);
}

StudentWorld::StudentWorld(string assetPath) : GameWorld(assetPath), m_ghostRacer(nullptr), m_bonusScore(0), m_lastWhiteY(-1) {}

// Call the cleanUp() method to destruct StudentWorld
StudentWorld::~StudentWorld() {
    cleanUp();
}

int StudentWorld::init() {
    // The bonus score starts out at 5000 each level
    m_bonusScore = 5000;

    // Initialize GhostRacer
    m_ghostRacer = new GhostRacer(this);

    // Add Ghost Racer to our actors vector
    m_actors.push_back(m_ghostRacer);

    // Logic to add the initial YellowBorderLines to the screen
    const int N = VIEW_HEIGHT / SPRITE_HEIGHT;
    for (int i = 0; i < N; i++) {
        int y = i * SPRITE_HEIGHT;
        YellowBorderLine* left = new YellowBorderLine(leftEdge(), y, this);
        YellowBorderLine* right = new YellowBorderLine(rightEdge(), y, this);

        m_actors.push_back(left);
        m_actors.push_back(right);
    }

    // Logic to add the initial WhiteBorderLines to the screen
    const int M = VIEW_HEIGHT / (4 * SPRITE_HEIGHT);
    for (int i = 0; i < M; i++) {
        int y = i * (4 * SPRITE_HEIGHT);
        m_lastWhiteY = y;

        WhiteBorderLine* left = new WhiteBorderLine(leftEdge() + (ROAD_WIDTH / 3), y, this);
        WhiteBorderLine* right = new WhiteBorderLine(rightEdge() - (ROAD_WIDTH / 3), y, this);

        m_actors.push_back(left);
        m_actors.push_back(right);
    }

    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move() {
    // Loop through each Actor and call their doSomething() method if they are active
    for (unsigned int i = 0; i < m_actors.size(); i++) {
        Actor* actor = m_actors.at(i);

        if (actor->isActive()) {
            actor->doSomething();

            // After an Actor does something, GhostRacer might not be active anymore
            if (!m_ghostRacer->isActive()) {
                return GWSTATUS_PLAYER_DIED;
            }

            // If GhostRacer has saved the required number of souls for this level, add the bonus score
            // to its total score and play the sound to finish the level
            if (m_ghostRacer->soulsSaved() == 2 * getLevel() + 5) {
                increaseScore(m_bonusScore);
                playSound(SOUND_FINISHED_LEVEL);
                return GWSTATUS_FINISHED_LEVEL;
            }
        }
    }

    for (int i = m_actors.size() - 1; i >= 0; i--) {
        Actor* actor = m_actors.at(i);

        // We want to remove any inactive Actors from our vector and free up the memory they're occupying
        if (!actor->isActive()) {
            m_actors.erase(m_actors.begin() + i);

            delete actor;
        }
    }

    // Add new Actors if they should be added during each tick
    addBorderLines();
    addZombieCabs();
    addOilSlicks();
    addZombiePeds();
    addHumanPeds();
    addHolyWaterRefillGoodies();
    addLostSoulGoodies();

    // Write the game stat text to a string stream
    ostringstream oss;
    oss << "Score: " << getScore() << "  ";
    oss << "Lvl: " << getLevel() << "  ";
    oss << "Souls2Save: " << (2 * getLevel() + 5 - m_ghostRacer->soulsSaved()) << "  ";
    oss << "Lives: " << getLives() << "  ";
    oss << "Health: " << m_ghostRacer->health() << "  ";
    oss << "Sprays: " << m_ghostRacer->numSprays() << "  ";
    oss << "Bonus: " << m_bonusScore;

    setGameStatText(oss.str());

    // Decrement the bonus score every tick
    if (m_bonusScore > 0) {
        m_bonusScore--;
    }

    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp() {
    // Delete all pointers in our actors vector, which includes the pointer to Ghost Racer
    for (int i = m_actors.size() - 1; i >= 0; i--) {
        Actor* actor = m_actors.at(i);

        m_actors.erase(m_actors.begin() + i);

        delete actor;
    }
}

// Return a vector containing pointers to all active Actors in the game
vector<Actor*> StudentWorld::actors() const {
    return m_actors;
}

// Return a pointer to GhostRacer
GhostRacer* StudentWorld::ghostRacer() const {
    return m_ghostRacer;
}

// Return the left edge of the road
int StudentWorld::leftEdge() const {
    return ROAD_CENTER - (ROAD_WIDTH / 2);
}

// Return the right edge of the road
int StudentWorld::rightEdge() const {
    return ROAD_CENTER + (ROAD_WIDTH / 2);
}

void StudentWorld::addBorderLines() {
    // There needs to have been at least one white border line added for this value not to be -1
    // Should never happen but checking for it nevertheless
    if (m_lastWhiteY != -1) {
        int new_border_y = VIEW_HEIGHT - SPRITE_HEIGHT;
        int delta_y = new_border_y - m_lastWhiteY;

        // Logic to add new YellowBorderLines to the screen
        if (delta_y >= SPRITE_HEIGHT) {
            YellowBorderLine* left = new YellowBorderLine(leftEdge(), new_border_y, this);
            YellowBorderLine* right = new YellowBorderLine(rightEdge(), new_border_y, this);

            m_actors.push_back(left);
            m_actors.push_back(right);
        }

        // Logic to add new WhiteBorderLines to the screen
        if (delta_y >= 4 * SPRITE_HEIGHT) {
            m_lastWhiteY = new_border_y;

            WhiteBorderLine* left = new WhiteBorderLine(leftEdge() + (ROAD_WIDTH / 3), new_border_y, this);
            WhiteBorderLine* right = new WhiteBorderLine(rightEdge() - (ROAD_WIDTH / 3), new_border_y, this);

            m_actors.push_back(left);
            m_actors.push_back(right);
        }

        // The value for the Y of the last WhiteBorderLine added needs to update every tick otherwise new WhiteBorderLines won't be added
        m_lastWhiteY += (int)(BORDER_LINE_VS - m_ghostRacer->verticalSpeed());
    }
}

void StudentWorld::addZombieCabs() {
    int chanceZombieCab = max(100 - getLevel() * 10, 20);
    int rand = randInt(0, chanceZombieCab - 1);
    if (rand == 0) {
        // We want to visit each lane, but we want the first one we visit to be random.
        int cur_lane = randInt(0, 2);   // 0 = left, 1 = middle, 2 = right,

        int numLanesVisited = 0;
        double startY = -1, zombieCabVertSpeed;

        while (numLanesVisited < 3) {
            Actor* closestToBottom = nullptr;

            for (unsigned int i = 0; i < m_actors.size(); i++) {
                Actor* actor = m_actors.at(i);
                // If the Actor being examined is considered "collision avoidance-worthy" and it's in cur_lane and
                // it's near the bottom of the screen, then it might be the Actor closest to the bottom of the screen
                if (actor->isCollisionAvoidanceWorthy() && isInLane(actor, cur_lane) && actor->getY() <= (closestToBottom != nullptr ? closestToBottom->getY() : VIEW_HEIGHT)) {
                    closestToBottom = actor;
                }
            }

            // If the Actor closest to the bottom of the screen exists and it's in the bottom third of the screen, then
            // set its starting Y position and its vertical speed and break out of the while loop
            if (closestToBottom == nullptr || closestToBottom->getY() > (VIEW_HEIGHT / 3)) {
                startY = SPRITE_HEIGHT / 2;
                zombieCabVertSpeed = m_ghostRacer->verticalSpeed() + randInt(2, 4);
                break;
            }

            Actor* closestToTop = nullptr;

            for (unsigned int i = 0; i < m_actors.size(); i++) {
                Actor* actor = m_actors.at(i);
                // If the Actor being examined is considered "collision avoidance-worthy" and it's in cur_lane and
                // it's near the top of the screen, then it might be the Actor closest to the top of the screen
                if (actor->isCollisionAvoidanceWorthy() && isInLane(actor, cur_lane) && actor->getY() >= (closestToTop != nullptr ? closestToTop->getY() : 0)) {
                    closestToTop = actor;
                }
            }

            // If the Actor closest to the bottom of the screen exists and it's in the upper third of the screen, then
            // set its starting Y position and its vertical speed and break out of the while loop
            if (closestToTop == nullptr || closestToTop->getY() < (VIEW_HEIGHT * 2 / 3)) {
                startY = VIEW_HEIGHT - SPRITE_HEIGHT / 2;
                zombieCabVertSpeed = m_ghostRacer->verticalSpeed() - randInt(2, 4);
                break;
            }

            // Change the value of cur_lane so we visit a different lane next time
            if (cur_lane == 0) {
                cur_lane = 1;
            } else if (cur_lane == 1) {
                cur_lane = 2;
            } else if (cur_lane == 2) {
                cur_lane = 0;
            }

            numLanesVisited++;
        }

        // If startY is still -1, then there is no viable lane to introduce a new zombie cab
        if (startY == -1) {
            return;
        }

        double startX = leftEdge() + (ROAD_WIDTH / 6) * (2 * cur_lane + 1); // kind of scuffed formula to find the center of a lane
        ZombieCab* zombieCab = new ZombieCab(startX, startY, this, zombieCabVertSpeed, cur_lane);
        m_actors.push_back(zombieCab);
    }
}

// Add a new OilSlick given a starting X and a starting Y
void StudentWorld::addOilSlick(double startX, double startY) {
    OilSlick* oilSlick = new OilSlick(startX, startY, this);
    m_actors.push_back(oilSlick);
}

// Add a new OilSlick if the criteria defined in the spec is met
void StudentWorld::addOilSlicks() {
    int chanceOilSlick = max(150 - getLevel() * 10, 40);
    int rand = randInt(0, chanceOilSlick - 1);
    if (rand == 0) {
        addOilSlick(randInt(leftEdge(), rightEdge()), VIEW_HEIGHT);
    }
}

// Add a new ZombiePed if the criteria defined in the spec is met
void StudentWorld::addZombiePeds() {
    int chanceZombiePed = max(100 - getLevel() * 10, 20);
    int rand = randInt(0, chanceZombiePed - 1);
    if (rand == 0) {
        ZombiePedestrian* zombiePed = new ZombiePedestrian(randInt(0, VIEW_WIDTH), VIEW_HEIGHT, this);
        m_actors.push_back(zombiePed);
    }
}

// Add a new HumanPed if the criteria defined in the spec is met
void StudentWorld::addHumanPeds() {
    int chanceHumanPed = max(200 - getLevel() * 10, 30);
    int rand = randInt(0, chanceHumanPed - 1);
    if (rand == 0) {
        HumanPedestrian* humanPed = new HumanPedestrian(randInt(0, VIEW_WIDTH), VIEW_HEIGHT, this);
        m_actors.push_back(humanPed);
    }
}

// Add a new HolyWaterRefillGoodie if the criteria defined in the spec is met
void StudentWorld::addHolyWaterRefillGoodies() {
    int chanceOfHolyWater = 100 + 10 * getLevel();
    int rand = randInt(0, chanceOfHolyWater - 1);
    if (rand == 0) {
        HolyWaterGoodie* holyWaterGoodie = new HolyWaterGoodie(randInt(leftEdge(), rightEdge()), VIEW_HEIGHT, this);
        m_actors.push_back(holyWaterGoodie);
    }
}

// Add a new LostSoulGoodie if the criteria defined in the spec is met
void StudentWorld::addLostSoulGoodies() {
    int chanceOfLostSoul = 100;
    int rand = randInt(0, chanceOfLostSoul - 1);
    if (rand == 0) {
        LostSoulGoodie* lostSoulGoodie = new LostSoulGoodie(randInt(leftEdge(), rightEdge()), VIEW_HEIGHT, this);
        m_actors.push_back(lostSoulGoodie);
    }
}

// Add a new HolyWaterProjectile given a starting X, a starting Y, and a direction
void StudentWorld::addHolyWaterProjectile(double startX, double startY, int dir) {
    HolyWaterProjectile* proj = new HolyWaterProjectile(startX, startY, dir, this);
    m_actors.push_back(proj);
}

// Add a new HealingGoodie if the criteria defined in the spec is met
void StudentWorld::addHealingGoodie(double startX, double startY) {
    HealingGoodie* healingGoodie = new HealingGoodie(startX, startY, this);
    m_actors.push_back(healingGoodie);
}

// Returns whether an Actor is in a specific lane
bool StudentWorld::isInLane(Actor* actor, int lane) {
    // If the lane isn't between 0 and 2 inclusive, return false
    if (lane < 0 || lane > 2) {
        return false;
    }

    double x = actor->getX();
    double leftX = leftEdge() + (ROAD_WIDTH / 3);   // x of the white border line to the left
    double rightX = rightEdge() - (ROAD_WIDTH / 3);     // x of the white border line to the right

    switch (lane) {
    case 0:
        if (x >= leftEdge() && x < leftX) {
            return true;
        }
        break;
    case 1:
        if (x >= leftX && x < rightX) {
            return true;
        }
        break;
    case 2:
        if (x >= rightX && x < rightEdge()) {
            return true;
        }
        break;
    }

    return false;
}