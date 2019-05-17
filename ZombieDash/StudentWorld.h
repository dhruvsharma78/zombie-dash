#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <vector>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class Actor;
class Penelope;

class StudentWorld : public GameWorld{
    public:
        StudentWorld(std::string assetDir);
        virtual ~StudentWorld();
        
        virtual int init();
        virtual int move();
        virtual void cleanUp();

        // Add an actor to the world.
        void addActor(Actor* a);

        // Record that one more citizen on the current level is gone (exited,
        // died, or turned into a zombie).
        void recordCitizenGone();

        // Indicate that the player has finished the level if all citizens
        // are gone.
        void recordLevelFinishedIfAllCitizensGone();

        bool allCitizensGone();

        // For each actor overlapping a, activate a if appropriate.
        void activateOnAppropriateActors(Actor* a);

        // Is an agent blocked from moving to the indicated location?
        bool isAgentMovementBlockedAt(double ax, double ay, Actor* actorToMove) const;

        // Is creation of a flame blocked at the indicated location?
        bool isFlameBlockedAt(double ax, double ay) const;

        // Is there something at the indicated location that might cause a
        // zombie to vomit (i.e., a human)?
        bool isZombieVomitTriggerAt(double x, double y) const;

        // Return true if there is a living human, otherwise false.  If true,
        // otherX, otherY, and distance will be set to the location and distance
        // of the human nearest to (x,y).
        bool locateNearestVomitTrigger(double x, double y, double& otherX, double& otherY, double& distance);

        // Return true if there is a living zombie or Penelope, otherwise false.
        // If true, otherX, otherY, and distance will be set to the location and
        // distance of the one nearest to (x,y), and isThreat will be set to true
        // if it's a zombie, false if a Penelope.
        bool locateNearestCitizenTrigger(double x, double y, double& otherX, double& otherY, double& distance, bool& isThreat) const;

        // Return true if there is a living zombie, false otherwise.  If true,
        // otherX, otherY and distance will be set to the location and distance
        // of the one nearest to (x,y).
        bool locateNearestCitizenThreat(double x, double y, double& otherX, double& otherY, double& distance) const;

        bool objectsOverlap(double ax, double ay) const;
        bool objectsOverlap(Actor* a, Actor* b) const;
        bool objectsOverlap(double ax, double ay, double bx, double by) const;
        double getDistance(double ax, double ay, double bx, double by) const;
    private:
        std::vector<Actor*> m_actors;
        Penelope* m_player;
        int m_nCitizensStart;
        int m_nCitizensGone;
        int currentGameStatus;
};

#endif // STUDENTWORLD_INCLUDED
