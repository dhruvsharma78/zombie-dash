#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class StudentWorld;
class Goodie;
class Penelope;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////BASE CLASSES///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////
// Actor
////////////////////////////////////////////////////////

class Actor : public GraphObject{
    public:
        Actor(StudentWorld* w, int imageID, double x, double y, int dir, int depth);
        // Action to perform for each tick.
        virtual void doSomething() = 0;
        // Is this actor dead?
        bool isDead() const;
        // Mark this actor as dead.
        void setDead();
        // Get this actor's world
        StudentWorld* getWorld() const;

        // If this is an activated object, perform its effect on a (e.g., for an
        // Exit have a use the exit).
        virtual void activate(Actor* a); 


        // If this object uses exits, use the exit.
        virtual void useExit();
        // If this object can die by falling into a pit or burning, die.
        virtual void dieByFallOrBurn();
        // If this object can be infected by vomit, get infected.
        virtual void beVomitedOn();
        // If this object can pick up goodies, pick up g
        virtual void pickUpGoodie(Goodie* g);


        // Does this object block agent movement?
        virtual bool blocksMovement() const;
        // Does this object block flames?
        virtual bool blocksFlame() const;
        // Does this object trigger landmines only when they're active?
        virtual bool triggersOnlyActiveLandmines() const;
        // Can this object cause a zombie to vomit?
        virtual bool triggersZombieVomit() const;
        // Is this object a threat to citizens?
        virtual bool threatensCitizens() const;
        // Does this object trigger citizens to follow it or flee it?
        virtual bool triggersCitizens() const;
    private:
        bool m_alive; // True if alive, false if dead
        StudentWorld* m_sw;
};

////////////////////////////////////////////////////////
// Wall
////////////////////////////////////////////////////////

class Wall : public Actor{
    public:
        Wall(StudentWorld* w, double x, double y);
        virtual void doSomething();
        virtual bool blocksMovement() const;
        virtual bool blocksFlame() const;
};

////////////////////////////////////////////////////////
// ActivatingObject
////////////////////////////////////////////////////////

class ActivatingObject : public Actor{
    public:
        ActivatingObject(StudentWorld* w, int imageID, double x, double y, int depth, int dir);
        virtual void activate(Actor* a) = 0;
};

////////////////////////////////////////////////////////
// Goodie
////////////////////////////////////////////////////////

class Goodie : public ActivatingObject{
    public:
        Goodie(StudentWorld* w, int imageID, double x, double y);
        virtual void activate(Actor* a);
        virtual void dieByFallOrBurn();

        // Have p pick up this goodie.
        virtual void pickUp(Penelope* p) = 0;
};

class VaccineGoodie : public Goodie{
    public:
        VaccineGoodie(StudentWorld* w, double x, double y);
        virtual void doSomething();
        virtual void pickUp(Penelope* p);
};

class GasCanGoodie : public Goodie{
    public:
        GasCanGoodie(StudentWorld* w, double x, double y);
        virtual void doSomething();
        virtual void pickUp(Penelope* p);
};

class LandmineGoodie : public Goodie{
    public:
        LandmineGoodie(StudentWorld* w, double x, double y);
        virtual void doSomething();
        virtual void pickUp(Penelope* p);
};

////////////////////////////////////////////////////////
// Agent
////////////////////////////////////////////////////////

class Agent : public Actor{
    public: 
        Agent(StudentWorld* w, int imageID, double x, double y, int dir);
        virtual bool blocksMovement() const;
        virtual bool triggersOnlyActiveLandmines() const;
};

////////////////////////////////////////////////////////
// Human
////////////////////////////////////////////////////////

class Human : public Agent{
    public:
        Human(StudentWorld* w, int imageID, double x, double y);
        virtual void beVomitedOn();
        virtual bool triggersZombieVomit() const;
 
        // Make this human uninfected by vomit.
        void clearInfection();

        // How many ticks since this human was infected by vomit?
        int getInfectionDuration() const;
        bool isInfected() const;
        void infect();
    private:
        int m_infectionLevel;
        bool m_isInfected;
};

class Penelope : public Human{
    public:
        Penelope(StudentWorld* w, double x, double y);
        virtual void doSomething();
        virtual void useExit();
        virtual void dieByFallOrBurn();
        virtual void pickUpGoodie(Goodie* g);

        // Increase the number of vaccines the object has.
        void increaseVaccines(int x);

        // Increase the number of flame charges the object has.
        void increaseFlameCharges(int x);

        // Increase the number of landmines the object has.
        void increaseLandmines(int x);

        // How many vaccines does the object have?
        int getNumVaccines() const;


        // How many flame charges does the object have?
        int getNumFlameCharges() const;

        // How many landmines does the object have?
        int getNumLandmines() const;

        virtual bool triggersCitizens() const;

    private:
        void movePenelope(int key);
        void fireFlame();

        int m_nVaccines;
        int m_nFlames;
        int m_nLandmines;
};

class Citizen : public Human{
    public: 
        Citizen(StudentWorld* w,  double x, double y);
        virtual void doSomething();
        virtual void useExit();
        virtual void dieByFallOrBurn();

    private:
        bool isParalyzed() const;
        void toggleParalysis();

        bool m_isParalyzed;
};

////////////////////////////////////////////////////////
// Zombie
////////////////////////////////////////////////////////

class Zombie : public Agent{
    public: 
        Zombie(StudentWorld* w,  double x, double y);
        virtual void doSomething();
        virtual bool triggersCitizens() const;
        virtual bool threatensCitizens() const;
        int movementPlan() const;
        void setMovementPlan(int x);
    private:
        virtual void zombieMovementMechanism() = 0;
        bool isParalyzed() const;
        void toggleParalysis();
        int m_movementPlan;
        bool m_isParalyzed;
};

class DumbZombie : public Zombie{
    public:
        DumbZombie(StudentWorld* w,  double x, double y);
        virtual void dieByFallOrBurn();
    private:    
        virtual void zombieMovementMechanism();
};

class SmartZombie : public Zombie{
    public:
        SmartZombie(StudentWorld* w,  double x, double y);
        virtual void dieByFallOrBurn();
    private:
        virtual void zombieMovementMechanism();
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Exit : public ActivatingObject{
    public:
        Exit(StudentWorld* w, double x, double y);
        virtual void doSomething();
        virtual void activate(Actor* a);
        virtual bool blocksFlame() const;
};

class Pit : public ActivatingObject{
    public:
        Pit(StudentWorld* w, double x, double y);
        virtual void doSomething();
        virtual void activate(Actor* a);
};

class Flame : public ActivatingObject{
    public:
        Flame(StudentWorld* w, double x, double y, int dir);
        virtual void doSomething();
        virtual void activate(Actor* a);
    private:
        int m_numberOfTicks;
};

class Vomit : public ActivatingObject
{
    public:
        Vomit(StudentWorld* w, double x, double y, int dir);
        virtual void doSomething();
        virtual void activate(Actor* a);
    private:
        int m_numberOfTicks;
};

class Landmine : public ActivatingObject
{
    public:
        Landmine(StudentWorld* w, double x, double y);
        virtual void doSomething();
        virtual void activate(Actor* a);
        virtual void dieByFallOrBurn();

        void explosionMechanism();
        bool safetyOn() const;
        void turnOffSafety();
        void decrementSafetyTicks();
        
    private:
        bool m_safetyOn;
        int m_safetyticks;
};










#endif // ACTOR_INCLUDED
