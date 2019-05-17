#include "StudentWorld.h"
#include "GameConstants.h"
#include "Level.h"
#include "Actor.h"
#include <string>
#include <vector>
#include <sstream> 
#include <iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetPath){
	return new StudentWorld(assetPath);
}

StudentWorld::StudentWorld(string assetPath): GameWorld(assetPath){
    m_nCitizensStart = 0;
    m_nCitizensGone = 0;
    m_player = nullptr;
    currentGameStatus = GWSTATUS_CONTINUE_GAME;
}

StudentWorld::~StudentWorld(){

    for(vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); ++it) {
        if((*it) != nullptr){
            delete *it;
        } 
    }
}

int StudentWorld::init(){

    m_nCitizensStart = 0;
    m_nCitizensGone = 0;
    m_player = nullptr;
    currentGameStatus = GWSTATUS_CONTINUE_GAME;

    Level lev(assetPath());
    
    // Get level number
    int currentLevel = getLevel();
    if(currentLevel == 100){
        return GWSTATUS_PLAYER_WON;
    }

    // Get file  name
    ostringstream oss;
    oss.fill('0');
    oss << "level";
	oss << setw(2) << currentLevel;
    oss << ".txt";
	string levelName = oss.str();

    // Load level file
    cerr << "Attempting to load Level " << levelName << endl;
    Level::LoadResult result = lev.loadLevel(levelName);
    if(result == Level::load_fail_file_not_found){
        cerr << "ERROR: LOADING ERROR\n\tCould not find level file: " << levelName << "\n";
        return GWSTATUS_PLAYER_WON;
    }else if(result == Level::load_fail_bad_format){
        cerr << "ERROR: LEVEL FORMATTING ERROR\n\tLevel file " << levelName << " formatted incorrectly\n";
        return GWSTATUS_LEVEL_ERROR;
    }else if(result == Level::load_success){
        cerr << "SUCCESS: Level loaded" << endl;

        for(int i=0; i<LEVEL_WIDTH; i++){
            for(int j=0; j<LEVEL_HEIGHT; j++){
                Level::MazeEntry me = lev.getContentsOf(i, j);
                switch(me){
                    case Level::empty:
                        break;
                    case Level::exit:{
                        Actor* newStuff = new Exit(this, i*SPRITE_WIDTH, j*SPRITE_HEIGHT);
                        m_actors.push_back(newStuff);
                        break;
                    }
                    case Level::player:{
                        m_player = new Penelope(this, i*SPRITE_WIDTH, j*SPRITE_HEIGHT);
                        m_actors.push_back(m_player);
                        break;
                    }
                    case Level::dumb_zombie:{
                        Actor* newStuff = new DumbZombie(this, i*SPRITE_WIDTH, j*SPRITE_HEIGHT);
                        m_actors.push_back(newStuff);
                        break;
                    }
                    case Level::smart_zombie:{
                        Actor* newStuff = new SmartZombie(this, i*SPRITE_WIDTH, j*SPRITE_HEIGHT);
                        m_actors.push_back(newStuff);
                        break;
                    }
                    case Level::citizen:{
                        Actor* newStuff = new Citizen(this, i*SPRITE_WIDTH, j*SPRITE_HEIGHT);
                        m_actors.push_back(newStuff);
                        m_nCitizensStart++;
                        break;
                    }
                    case Level::wall:{
                        Actor* newStuff = new Wall(this, i*SPRITE_WIDTH, j*SPRITE_HEIGHT);
                        m_actors.push_back(newStuff);
                        break;
                    }
                    case Level::pit:{
                        Actor* newStuff = new Pit(this, i*SPRITE_WIDTH, j*SPRITE_HEIGHT);
                        m_actors.push_back(newStuff);
                        break;
                    }
                    case Level::vaccine_goodie:{
                        Actor* newStuff = new VaccineGoodie(this, i*SPRITE_WIDTH, j*SPRITE_HEIGHT);
                        m_actors.push_back(newStuff);
                        break;
                    }
                    case Level::gas_can_goodie:{
                        Actor* newStuff = new GasCanGoodie(this, i*SPRITE_WIDTH, j*SPRITE_HEIGHT);
                        m_actors.push_back(newStuff);
                        break;
                    }
                    case Level::landmine_goodie:{
                        Actor* newStuff = new LandmineGoodie(this, i*SPRITE_WIDTH, j*SPRITE_HEIGHT);
                        m_actors.push_back(newStuff);
                        break;
                    }
                }
            }
        }
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move(){


    for(vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); ++it) {
        if(!(*it)->isDead()){
            (*it)->doSomething();
        }

        // Check if something caused player to die
        if(m_player->isDead()){
            decLives();
            return GWSTATUS_PLAYER_DIED;
        }

        // Check if level has ended
        if(currentGameStatus == GWSTATUS_FINISHED_LEVEL){
            playSound(SOUND_LEVEL_FINISHED);
            return GWSTATUS_FINISHED_LEVEL;
        }
    }
    

    // Delete dead actors
    for(vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end();) {
        if((*it)->isDead()){
            delete *it;
            it = m_actors.erase(it);
        }else{
            it++;
        }
    }

    //Set text
    ostringstream oss;
    oss<<"Score: ";
    oss.fill('0');
    if(getScore() < 0){
        oss<<"-"<<setw(5)<<abs(getScore())<<"  ";
    }else{
        oss<<setw(6)<<getScore()<<"  ";
    }
    oss<<"Level: ";
    oss<<getLevel()<<"  ";
    oss<<"Lives: ";
    oss<<getLives()<<"  ";
    oss<<"Vaccines: ";
    oss<<m_player->getNumVaccines()<<"  ";
    oss<<"Flames: ";
    oss<<m_player->getNumFlameCharges()<<"  ";
    oss<<"Mines: ";
    oss<<m_player->getNumLandmines()<<"  ";
    oss<<"Infected: ";
    oss<<m_player->getInfectionDuration();

    setGameStatText(oss.str());


    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp(){
    for(vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end();) {
        if((*it) != nullptr){
            delete *it;
            it = m_actors.erase(it);
        } 
    }
    m_actors.clear();
}

void StudentWorld::addActor(Actor* a){
    m_actors.push_back(a);
}

void StudentWorld::recordCitizenGone(){
    m_nCitizensGone++;
}

void StudentWorld::recordLevelFinishedIfAllCitizensGone(){
    if(allCitizensGone()){
        currentGameStatus = GWSTATUS_FINISHED_LEVEL;
    }
}

bool StudentWorld::allCitizensGone(){
    return m_nCitizensStart == m_nCitizensGone;
}

void StudentWorld::activateOnAppropriateActors(Actor* a){
    for(vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); ++it){
        if(*it == a){ continue; }
        if((*it)->isDead()){ continue; }
        if(objectsOverlap(a,*it)){
            a->activate(*it);
        }
    }
}

bool StudentWorld::isAgentMovementBlockedAt(double ax, double ay, Actor* actorToMove) const{
    int a_boxStartX = ax;
    int a_boxEndX   = a_boxStartX + SPRITE_WIDTH - 1;
    int a_boxStartY = ay;
    int a_boxEndY   = a_boxStartY + SPRITE_HEIGHT - 1;
    for(vector<Actor*>::const_iterator it = m_actors.begin(); it != m_actors.end(); ++it) {
        
        if((*it) == actorToMove) { continue; }
        if(!(*it)->blocksMovement()) { continue; }
        if((*it)->isDead()) { continue; }

        int b_boxStartX = (*it)->getX();
        int b_boxEndX   = b_boxStartX + SPRITE_WIDTH - 1;
        int b_boxStartY = (*it)->getY();
        int b_boxEndY   = b_boxStartY + SPRITE_HEIGHT - 1;
        
        if((b_boxStartX >= a_boxStartX && b_boxStartX <= a_boxEndX) ||
           (b_boxEndX >= a_boxStartX && b_boxEndX <= a_boxEndX)){
            
            if((b_boxStartY >= a_boxStartY && b_boxStartY <= a_boxEndY) ||
               (b_boxEndY >= a_boxStartY && b_boxEndY <= a_boxEndY)){
                
                return true;
            }
            
        }

    }

    return false;
}

bool StudentWorld::isFlameBlockedAt(double ax, double ay) const{
    for(vector<Actor*>::const_iterator it = m_actors.begin(); it != m_actors.end(); ++it) {
        
        if(!(*it)->blocksFlame()){
            continue;
        }

        if((*it)->isDead()){
            continue;
        }

        if(objectsOverlap(ax, ay, (*it)->getX(), (*it)->getY())){
            return true;
        }

    }
    return false;
}

bool StudentWorld::isZombieVomitTriggerAt(double x, double y) const{
    for(vector<Actor*>::const_iterator it = m_actors.begin(); it != m_actors.end(); ++it) {
        if(!(*it)->triggersZombieVomit()){ continue; }
        if(getDistance(x, y, (*it)->getX(), (*it)->getY()) <= 10){
            return true;
        }
    }
    return false;
}

bool StudentWorld::locateNearestVomitTrigger(double x, double y, double& otherX, double& otherY, double& distance){
    double minX=999999, minY=999999, minD=999999;
    bool foundHuman = false;
    for(vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); ++it){
        if(!(*it)->triggersZombieVomit()) { continue; }
        foundHuman = true;
        double d = getDistance(x, y, (*it)->getX(), (*it)->getY());
        if(d<minD){
            minD = d;
            minX = (*it)->getX();
            minY = (*it)->getY();
        }
    }
    if(foundHuman){
        otherX = minX;
        otherY = minY;
        distance = minD;
        return true;
    }
    return false;
}

bool StudentWorld::locateNearestCitizenTrigger(double x, double y, double& otherX, double& otherY, double& distance, bool& isThreat) const{
    double minX=999999, minY=999999, minD=999999;
    bool found = false;
    isThreat = false;
    for(vector<Actor*>::const_iterator it = m_actors.begin(); it != m_actors.end(); ++it){
        if(!(*it)->triggersCitizens()) { continue; }
        found = true;
        double d = getDistance(x, y, (*it)->getX(), (*it)->getY());
        if(d<minD){
            isThreat = ((*it)->threatensCitizens()) ? true : false;
            minD = d;
            minX = (*it)->getX();
            minY = (*it)->getY();
        }
    }
    if(found){
        otherX = minX;
        otherY = minY;
        distance = minD;
        return true;
    }
    return false;
}

bool StudentWorld::locateNearestCitizenThreat(double x, double y, double& otherX, double& otherY, double& distance) const{
    double minX=999999, minY=999999, minD=999999;
    bool found = false;
    for(vector<Actor*>::const_iterator it = m_actors.begin(); it != m_actors.end(); ++it){
        if(!(*it)->threatensCitizens()) { continue; }
        found = true;
        double d = getDistance(x, y, (*it)->getX(), (*it)->getY());
        if(d<minD){
            minD = d;
            minX = (*it)->getX();
            minY = (*it)->getY();
        }
    }
    if(found){
        otherX = minX;
        otherY = minY;
        distance = minD;
        return true;
    }
    return false;
}

bool StudentWorld::objectsOverlap(double ax, double ay) const{
    for(vector<Actor*>::const_iterator it = m_actors.begin(); it != m_actors.end(); ++it){
        if(objectsOverlap(ax, ay, (*it)->getX(), (*it)->getY())){
            return true;
        }
    }
    return false;
}

bool StudentWorld::objectsOverlap(Actor* a, Actor* b) const{
    double dist = getDistance(a->getX()+(SPRITE_WIDTH/2), a->getY()+(SPRITE_HEIGHT/2), b->getX()+(SPRITE_WIDTH/2), b->getY()+(SPRITE_HEIGHT/2));
    return (dist<=10);
}

bool StudentWorld::objectsOverlap(double ax, double ay, double bx, double by) const{
    double dist = getDistance(ax+(SPRITE_WIDTH/2), ay+(SPRITE_HEIGHT/2), bx+(SPRITE_WIDTH/2), by+(SPRITE_HEIGHT/2));
    return (dist<=10);
}

double StudentWorld::getDistance(double ax, double ay, double bx, double by) const{
    double dx = abs(ax-bx);
    double dy = abs(ay-by);
    return sqrt((dx*dx) + (dy*dy));
}
