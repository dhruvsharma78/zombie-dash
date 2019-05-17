#include "Actor.h"
#include "StudentWorld.h"

////////////////////////////////////////////////////////
// Actor
////////////////////////////////////////////////////////

Actor::Actor(StudentWorld *w, int imageID, double x, double y, int dir, int depth) :
    GraphObject(imageID, x, y, dir, depth)
{
    m_alive          = true;
    m_sw             = w;
}

bool Actor::isDead() const{
    return !m_alive;
}

void Actor::setDead(){
    m_alive = false;
}

StudentWorld* Actor::getWorld() const{
    return m_sw;
}

void Actor::activate(Actor* a){}
void Actor::useExit(){}
void Actor::dieByFallOrBurn(){}
void Actor::beVomitedOn(){}
void Actor::pickUpGoodie(Goodie* g){}
bool Actor::blocksMovement() const { return false; }
bool Actor::blocksFlame() const{return false;}
bool Actor::triggersOnlyActiveLandmines() const{return false;}
bool Actor::triggersZombieVomit() const { return false; }
bool Actor::threatensCitizens() const{return false;}
bool Actor::triggersCitizens() const{return false;}


////////////////////////////////////////////////////////
// Wall
////////////////////////////////////////////////////////

Wall::Wall(StudentWorld* w, double x, double y) : 
    Actor(w, IID_WALL, x, y, right, 0)
{
    return;
}

void Wall::doSomething(){
    return;
}        

bool Wall::blocksMovement() const{
    return true;
}

bool Wall::blocksFlame() const{
    return true;
}

////////////////////////////////////////////////////////
// ActivatingObject
////////////////////////////////////////////////////////

ActivatingObject::ActivatingObject(StudentWorld* w, int imageID, double x, double y, int depth, int dir) :
    Actor(w, imageID, x, y, dir, depth)
{

}

////////////////////////////////////////////////////////
// Goodie
////////////////////////////////////////////////////////

Goodie::Goodie(StudentWorld *w, int imageID, double x, double y) : 
    ActivatingObject(w, imageID, x, y, 1, right)
{

}

void Goodie::activate(Actor *a){
    a->pickUpGoodie(this);
}

void Goodie::dieByFallOrBurn(){
    setDead();
}


////////////////////////////////////////////////////////
// VaccineGoodie
////////////////////////////////////////////////////////

VaccineGoodie::VaccineGoodie(StudentWorld* w, double x, double y) : 
    Goodie(w, IID_VACCINE_GOODIE, x, y)
{

}

void VaccineGoodie::doSomething(){
    if(isDead()){
        return;
    }
    getWorld()->activateOnAppropriateActors(this);
} 

void VaccineGoodie::pickUp(Penelope* p){
    getWorld()->increaseScore(50);
    p->increaseVaccines(1);
}

////////////////////////////////////////////////////////
// GasCanGoodie
////////////////////////////////////////////////////////

GasCanGoodie::GasCanGoodie(StudentWorld* w, double x, double y) : 
    Goodie(w, IID_GAS_CAN_GOODIE, x, y)
{

}

void GasCanGoodie::doSomething(){
    if(isDead()){
        return;
    }
    getWorld()->activateOnAppropriateActors(this);
}

void GasCanGoodie::pickUp(Penelope* p){
    getWorld()->increaseScore(50);
    p->increaseFlameCharges(5);
}

////////////////////////////////////////////////////////
// LandmineGoodie
////////////////////////////////////////////////////////

LandmineGoodie::LandmineGoodie(StudentWorld* w, double x, double y) : 
    Goodie(w, IID_LANDMINE_GOODIE, x, y)
{

}

void LandmineGoodie::doSomething(){
    if(isDead()){
        return;
    }
    getWorld()->activateOnAppropriateActors(this);
}

void LandmineGoodie::pickUp(Penelope* p){
    getWorld()->increaseScore(50);
    p->increaseLandmines(2);
}
////////////////////////////////////////////////////////
// Agent
////////////////////////////////////////////////////////

Agent::Agent(StudentWorld *w, int imageID, double x, double y, int dir) : 
    Actor(w, imageID, x, y, dir, 0)
{
}

bool Agent::blocksMovement() const{ return true; }

bool Agent::triggersOnlyActiveLandmines() const{ return true;}

////////////////////////////////////////////////////////
// Human
////////////////////////////////////////////////////////

Human::Human(StudentWorld *w, int imageID, double x, double y) :
    Agent(w, imageID, x, y, right)
{
    m_infectionLevel = 0;
    m_isInfected =false;
}

void Human::beVomitedOn(){
    m_isInfected = true;
    infect();
}

bool Human::triggersZombieVomit() const{ return true; }

void Human::clearInfection() { m_infectionLevel = 0; m_isInfected = false; }

int Human::getInfectionDuration() const { return m_infectionLevel; }

bool Human::isInfected() const{ return m_isInfected; }

void Human::infect(){ m_infectionLevel++; }

////////////////////////////////////////////////////////
// Penelope
////////////////////////////////////////////////////////

Penelope::Penelope(StudentWorld* w, double x, double y) :
    Human(w, IID_PLAYER, x, y)
{
    m_nVaccines  = 0;
    m_nFlames    = 0;
    m_nLandmines = 0;
}

bool Penelope::triggersCitizens() const{return true;}

void Penelope::doSomething(){
    if(isDead()){
        return;
    }
    if(isInfected()){
        infect();
        if(getInfectionDuration() == 500){
            setDead();
            getWorld()->playSound(SOUND_PLAYER_DIE);
        }
    }

    // Check for key press and carry out action
    int key;
    if(getWorld()->getKey(key)){
        switch(key){
            case KEY_PRESS_UP:
            case KEY_PRESS_DOWN:
            case KEY_PRESS_LEFT:
            case KEY_PRESS_RIGHT:
                movePenelope(key);
                break;
            case KEY_PRESS_SPACE:
                fireFlame();
                break;
            case KEY_PRESS_ENTER:
                if(getNumVaccines() > 0){
                    m_nVaccines--;
                    clearInfection();
                }
                break;
            case KEY_PRESS_TAB:
                if(getNumLandmines() > 0){
                    m_nLandmines--;
                    getWorld()->addActor(new Landmine(getWorld(), getX(), getY()));
                }
                break;
        }
    }   
}

void Penelope::useExit(){
    if(getWorld()->allCitizensGone()){
        getWorld()->recordLevelFinishedIfAllCitizensGone();
    }
}

void Penelope::dieByFallOrBurn(){
    setDead();
    getWorld()->playSound(SOUND_PLAYER_DIE);
}

void Penelope::pickUpGoodie(Goodie* g){
    g->setDead();
    getWorld()->playSound(SOUND_GOT_GOODIE); 
    g->pickUp(this);
}


void Penelope::increaseVaccines(int x){
    m_nVaccines+=x;
}

void Penelope::increaseFlameCharges(int x){
    m_nFlames+=x;
}

void Penelope::increaseLandmines(int x){
    m_nLandmines+=x;
}

int Penelope::getNumVaccines() const{
    return m_nVaccines;
}

int Penelope::getNumFlameCharges() const{
    return m_nFlames;
}

int Penelope::getNumLandmines() const{
    return m_nLandmines;
}

void Penelope::movePenelope(int key){
    int newX = 0, newY = 0;
    switch (key){
        case KEY_PRESS_DOWN:
            setDirection(down);
            newX = getX();
            newY = getY() - 4;
            break;
        case KEY_PRESS_LEFT:
            setDirection(left);
            newX = getX() - 4;
            newY = getY();
            break;
        case KEY_PRESS_RIGHT:
            setDirection(right);
            newX = getX() + 4;
            newY = getY();
            break;
        case KEY_PRESS_UP:
            setDirection(up);
            newX = getX();
            newY = getY() + 4;
            break;
    }
    if (!getWorld()->isAgentMovementBlockedAt(newX, newY, this)){
        moveTo(newX, newY);
    }
}

void Penelope::fireFlame(){
    if(getNumFlameCharges() <= 0){
        return;
    }
    m_nFlames--;
    getWorld()->playSound(SOUND_PLAYER_FIRE);
    int currentDir = getDirection();
    double fx = 0, fy = 0;
    for (int i = 1; i <= 3; i++){
        switch (currentDir){
            case up:
                fx = getX();
                fy = getY() + (i * SPRITE_HEIGHT);
                break;
            case down:
                fx = getX();
                fy = getY() - (i * SPRITE_HEIGHT);
                break;
            case left:
                fx = getX() - (i * SPRITE_WIDTH);
                fy = getY();
                break;
            case right:
                fx = getX() + (i * SPRITE_WIDTH);
                fy = getY();
                break;
        }
        if (!getWorld()->isFlameBlockedAt(fx, fy)){
            getWorld()->addActor(new Flame(getWorld(), fx, fy, currentDir));
        }else{
            return;
        }
    }
}

////////////////////////////////////////////////////////
// Citizen
////////////////////////////////////////////////////////

Citizen::Citizen(StudentWorld* w,  double x, double y) :
    Human(w, IID_CITIZEN, x, y)
{
}

void Citizen::doSomething(){
    if(isDead()){
        return;
    }
    if(isInfected()){
        infect();
        if(getInfectionDuration() == 500){
            setDead();
            getWorld()->playSound(SOUND_ZOMBIE_BORN);
            getWorld()->increaseScore(-1000);
            getWorld()->recordCitizenGone();
            int x = randInt(1,100);
            if(x<=70){
                getWorld()->addActor(new DumbZombie(getWorld(), getX(), getY()));
            }else{
                getWorld()->addActor(new SmartZombie(getWorld(), getX(), getY()));
            }
            return;
        }
    }

    if(isParalyzed()){
        toggleParalysis();
        return;
    }
    toggleParalysis();

    double nearestX=0, nearestY=0, nearestD=0;
    bool nearestIsThreat;

    getWorld()->locateNearestCitizenTrigger(getX(), getY(), nearestX, nearestY, nearestD, nearestIsThreat);

    if(!nearestIsThreat && nearestD <= 80){
        
        double newX = getX() , newY = getY();
        if(getX() == nearestX || getY() == nearestY){
            int newDir = up;
            if(getY() < nearestY){
                newDir = up;
                newY+=2;
            }else if(getY() > nearestY){
                newDir = down;
                newY-=2;
            }else if(getX() < nearestX){
                newDir = right;
                newX+=2;
            }else if(getX() > nearestX){
                newDir = left;
                newX-=2;
            }
            if(!getWorld()->isAgentMovementBlockedAt(newX, newY, this)){
                setDirection(newDir);
                moveTo(newX, newY);
                return;
            }
        }

        double distances[4];
        //UP
        distances[0] = getWorld()->getDistance(getX(), getY() + 1, nearestX, nearestY);
        //DOWN
        distances[1] = getWorld()->getDistance(getX(), getY() - 1, nearestX, nearestY);
        //LEFT
        distances[2] = getWorld()->getDistance(getX() - 1, getY(), nearestX, nearestY);
        //RIGHT
        distances[3] = getWorld()->getDistance(getX() + 1, getY(), nearestX, nearestY);

        double minHorizontalD = fmin(distances[2], distances[3]);
        double minVerticalD = fmin(distances[0], distances[1]);
        int newDir[2];
        newDir[0] = (minHorizontalD == distances[2]) ? left : right;
        newDir[1] = (minVerticalD == distances[0]) ? up : down;
        int randIndex = randInt(0,1);
        int randomDir = newDir[randIndex];

        for(int i=0; i<2; i++){
            switch(randomDir){
                case up:
                    newY+=2;
                    break;
                case down:
                    newY-=2;
                    break;
                case left:
                    newX-=2;
                    break;
                case right:
                    newX+=2;
                    break;
            }
            if(!getWorld()->isAgentMovementBlockedAt(newX, newY, this)){
                setDirection(randomDir);
                moveTo(newX, newY);
                return;
            }
            randomDir = newDir[!randIndex];
        }
        return;
    }

    if(nearestIsThreat && nearestD <= 80){
        double distances[4];
        bool movementAllowed[4];
        double coords[4][2];
        double tempx,tempy;

        coords[0][0] = getX();
        coords[0][1] = getY()+2;

        coords[1][0] = getX();
        coords[1][1] = getY()-2;

        coords[2][0] = getX()-2;
        coords[2][1] = getY();

        coords[3][0] = getX()+2;
        coords[3][1] = getY();

        for(int i=0; i<4; i++){
            movementAllowed[i] = !getWorld()->isAgentMovementBlockedAt(coords[i][0], coords[i][1], this);
            getWorld()->locateNearestCitizenThreat(coords[i][0], coords[i][1], tempx, tempy, distances[i]);
        }

        double newDir= -999;
        double minD = nearestD;
        int directionX = -999;
        for(int i=0; i<4; i++){
            if(movementAllowed[i]){
                if(distances[i] > minD){
                    minD = distances[i];
                    switch(i){
                        case 0: newDir = up;    break;
                        case 1: newDir = down;  break;
                        case 2: newDir = left;  break;
                        case 3: newDir = right; break;
                    }
                    directionX = i;
                }
            }
        }

        if(newDir != -999){
            setDirection(newDir);
            moveTo(coords[directionX][0], coords[directionX][1]);
        }

    }

}

void Citizen::useExit(){
    getWorld()->increaseScore(500);
    setDead();
    getWorld()->playSound(SOUND_CITIZEN_SAVED);
    getWorld()->recordCitizenGone();
}

void Citizen::dieByFallOrBurn(){
    setDead();
    getWorld()->recordCitizenGone();
    getWorld()->playSound(SOUND_CITIZEN_DIE);
    getWorld()->increaseScore(-1000);
}

bool Citizen::isParalyzed() const { return m_isParalyzed; }
void Citizen::toggleParalysis() { m_isParalyzed = !m_isParalyzed; }

////////////////////////////////////////////////////////
// Zombie
////////////////////////////////////////////////////////

Zombie::Zombie(StudentWorld* w,  double x, double y) :
    Agent(w, IID_ZOMBIE, x, y, right)
{
    setMovementPlan(0);
}

int Zombie::movementPlan() const { return m_movementPlan; }

bool Zombie::triggersCitizens() const { return true; }
bool Zombie::threatensCitizens() const { return true; }

void Zombie::setMovementPlan(int x) { m_movementPlan = x; }

void Zombie::doSomething(){
    if(isDead()){
        return;
    }
    if(isParalyzed()){
        toggleParalysis();
        return;
    }
    toggleParalysis();

    double vomitCoordX=0, vomitCoordY=0;

    switch(getDirection()){
        case up:
            vomitCoordX = getX();
            vomitCoordY = getY() + SPRITE_HEIGHT;
            break;
        case down:
            vomitCoordX = getX();
            vomitCoordY = getY() - SPRITE_HEIGHT;
            break;
        case left:
            vomitCoordX = getX() - SPRITE_WIDTH;
            vomitCoordY = getY();
            break;
        case right:
            vomitCoordX = getX() + SPRITE_WIDTH;
            vomitCoordY = getY();
            break;
    }
    
    if(getWorld()->isZombieVomitTriggerAt(vomitCoordX, vomitCoordY)){
        int x = randInt(1, 3);
        if(x == 2){
            getWorld()->addActor(new Vomit(getWorld(), vomitCoordX, vomitCoordY, getDirection()));
            getWorld()->playSound(SOUND_ZOMBIE_VOMIT);
            return;
        }
    }

    zombieMovementMechanism();


    double newX = getX();
    double newY = getY();

    switch(getDirection()){
        case up:
            newY++;
            break;
        case down:
            newY--;
            break;
        case left:
            newX--;
            break;
        case right:
            newX++;
            break;
    }

    if(!getWorld()->isAgentMovementBlockedAt(newX, newY, this)){
        moveTo(newX, newY);
        setMovementPlan(movementPlan() - 1);
    }else{
        setMovementPlan(0);
    }

}

bool Zombie::isParalyzed() const { return m_isParalyzed; }
void Zombie::toggleParalysis() { m_isParalyzed = !m_isParalyzed; }

////////////////////////////////////////////////////////
// DumbZombie
////////////////////////////////////////////////////////

DumbZombie::DumbZombie(StudentWorld* w,  double x, double y) :
    Zombie(w, x, y)
{
}

void DumbZombie::zombieMovementMechanism(){
    if(movementPlan() == 0){
        setMovementPlan(randInt(3,10));
        switch(randInt(1,4)){
            case 1:
                setDirection(up);
                break;
            case 2:
                setDirection(down);
                break;
            case 3:
                setDirection(left);
                break;
            case 4:
                setDirection(right);
                break;
        }
    }
}

void DumbZombie::dieByFallOrBurn(){
    setDead();
    getWorld()->playSound(SOUND_ZOMBIE_DIE);
    getWorld()->increaseScore(1000);
    int x = randInt(1,10); 
    if(x == 5){
        double newX=0, newY=0;
        switch(randInt(1,4)){
            case 1:
                newX = getX();
                newY = getY() + SPRITE_HEIGHT;
                break;
            case 2:
                newX = getX();
                newY = getY() - SPRITE_HEIGHT;
                break;
            case 3:
                newX = getX() - SPRITE_WIDTH;
                newY = getY();
                break;
            case 4:
                newX = getX() + SPRITE_WIDTH;
                newY = getY();
                break;
        }
        if(!getWorld()->objectsOverlap(newX, newY)){
            getWorld()->addActor(new VaccineGoodie(getWorld(), newX, newY));
        }
    }
}


////////////////////////////////////////////////////////
// SmartZombie
////////////////////////////////////////////////////////

SmartZombie::SmartZombie(StudentWorld* w,  double x, double y) :
    Zombie(w, x, y)
{
}

void SmartZombie::zombieMovementMechanism(){

    double newX, newY, minD;

    if(movementPlan() == 0){
        setMovementPlan(randInt(3,10));

        getWorld()->locateNearestVomitTrigger(getX(), getY(), newX, newY, minD);

        if(minD > 80){
            switch(randInt(1,4)){
                case 1:
                    setDirection(up);
                    break;
                case 2:
                    setDirection(down);
                    break;
                case 3:
                    setDirection(left);
                    break;
                case 4:
                    setDirection(right);
                    break;
            }
        }else{
            double distances[4];
            //UP
            distances[0] = getWorld()->getDistance(getX(), getY()+1, newX, newY);
            //DOWN
            distances[1] = getWorld()->getDistance(getX(), getY()-1, newX, newY);
            //LEFT
            distances[2] = getWorld()->getDistance(getX()-1, getY(), newX, newY);
            //RIGHT
            distances[3] = getWorld()->getDistance(getX()+1, getY(), newX, newY);

            if(getX() == newX || getY() == newY){
                double minD = 99999;
                for(int i=0; i<4; i++){
                    if(distances[i] < minD){
                        minD = distances[i];
                    }
                }
                if(minD == distances[0]){
                    setDirection(up);
                }else if(minD == distances[1]){
                    setDirection(down);
                }else if(minD == distances[2]){
                    setDirection(left);
                }else if(minD == distances[3]){
                    setDirection(right);
                }

            }else{
                double minHorizontalD = fmin(distances[2], distances[3]);
                double minVerticalD   = fmin(distances[0], distances[1]);
                double newDir[2];
                newDir[0] = (minHorizontalD == distances[2]) ? left : right;
                newDir[1] = (minVerticalD == distances[0]) ? up : down;
                setDirection(newDir[randInt(0,1)]);
            }
        }
    }
}

void SmartZombie::dieByFallOrBurn(){
    setDead();
    getWorld()->playSound(SOUND_ZOMBIE_DIE);
    getWorld()->increaseScore(2000);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////
// Exit
////////////////////////////////////////////////////////

Exit::Exit(StudentWorld *w, double x, double y) :
    ActivatingObject(w, IID_EXIT, x, y, 1, right)
{
}

void Exit::doSomething(){
    getWorld()->activateOnAppropriateActors(this);
}

void Exit::activate(Actor *a){
    a->useExit(); 
}

bool Exit::blocksFlame() const{
    return true;
}

////////////////////////////////////////////////////////
// Pit
////////////////////////////////////////////////////////

Pit::Pit(StudentWorld *w, double x, double y) : 
    ActivatingObject(w, IID_PIT, x, y, 0, right)
{
}

void Pit::doSomething(){
    getWorld()->activateOnAppropriateActors(this);
}

void Pit::activate(Actor *a){
    a->dieByFallOrBurn();
}

////////////////////////////////////////////////////////
// Flame
////////////////////////////////////////////////////////

Flame::Flame(StudentWorld* w, double x, double y, int dir) :
    ActivatingObject(w, IID_FLAME, x, y, 0, dir)
{
    m_numberOfTicks = 0;
}

void Flame::doSomething(){
    if(isDead()){
        return;
    }

    m_numberOfTicks++;
    getWorld()->activateOnAppropriateActors(this);

    if(m_numberOfTicks >= 2){
        setDead();
        return;
    }
    
}

void Flame::activate(Actor* a){
    a->dieByFallOrBurn();
}

////////////////////////////////////////////////////////
// Vomit
////////////////////////////////////////////////////////

Vomit::Vomit(StudentWorld* w, double x, double y, int dir) :
    ActivatingObject(w, IID_VOMIT, x, y, 0, dir)
{
}

void Vomit::doSomething(){
    if(isDead()){
        return;
    }

    m_numberOfTicks++;
    getWorld()->activateOnAppropriateActors(this);

    if(m_numberOfTicks >= 2){
        setDead();
        return;
    }
}

void Vomit::activate(Actor* a){
    a->beVomitedOn();
}


////////////////////////////////////////////////////////
// Landmine
////////////////////////////////////////////////////////

Landmine::Landmine(StudentWorld* w, double x, double y) :
    ActivatingObject(w, IID_LANDMINE, x, y, 1, right)
{
    m_safetyOn = true;
    m_safetyticks = 30;
}

void Landmine::doSomething(){
    if(isDead()){
        return;
    }

    if(safetyOn()){
        decrementSafetyTicks();
        return;
    }

    getWorld()->activateOnAppropriateActors(this);
}

void Landmine::activate(Actor* a){

    if(!a->triggersOnlyActiveLandmines()){
        return;
    }

    explosionMechanism();

}

void Landmine::dieByFallOrBurn(){
    explosionMechanism();
}

void Landmine::explosionMechanism(){
    setDead();
    getWorld()->playSound(SOUND_LANDMINE_EXPLODE);

    double newFlameLocations[8][2];
    double currentX = getX();
    double currentY = getY();
    getWorld()->addActor(new Flame(getWorld(), currentX, currentY, up));

    //NORTH
    newFlameLocations[0][0] = getX();
    newFlameLocations[0][1] = getY()+SPRITE_HEIGHT;
    //NORTH EAST
    newFlameLocations[1][0] = getX()+SPRITE_WIDTH;
    newFlameLocations[1][1] = getY()+SPRITE_HEIGHT;
    //EAST
    newFlameLocations[2][0] = getX()+SPRITE_WIDTH;
    newFlameLocations[2][1] = getY();
    //SOUTH EAST
    newFlameLocations[3][0] = getX()+SPRITE_WIDTH;
    newFlameLocations[3][1] = getY()-SPRITE_HEIGHT;
    //SOUTH
    newFlameLocations[4][0] = getX();
    newFlameLocations[4][1] = getY()-SPRITE_HEIGHT;
    //SOUTH WEST
    newFlameLocations[5][0] = getX()-SPRITE_WIDTH;
    newFlameLocations[5][1] = getY()-SPRITE_HEIGHT;
    //WEST
    newFlameLocations[6][0] = getX()-SPRITE_WIDTH;
    newFlameLocations[6][1] = getY();
    //NORTH WEST
    newFlameLocations[7][0] = getX()-SPRITE_WIDTH;
    newFlameLocations[7][1] = getY()+SPRITE_HEIGHT;

    for(int i=0; i<8; i++){
        if (!getWorld()->isFlameBlockedAt(newFlameLocations[i][0], newFlameLocations[i][1])){
                getWorld()->addActor(new Flame(getWorld(), newFlameLocations[i][0], newFlameLocations[i][1], up));
        }
    }

    getWorld()->addActor(new Pit(getWorld(), currentX, currentY));
}


bool Landmine::safetyOn() const{
    return m_safetyOn;
}

void Landmine::turnOffSafety(){
    m_safetyOn = false;
}

void Landmine::decrementSafetyTicks(){
    m_safetyticks--;
    if(m_safetyticks == 0){
        turnOffSafety();
    }
}



