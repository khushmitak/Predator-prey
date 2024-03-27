#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>
using namespace std;

const int SIZE = 20; // size * size grid of cells in the world
const int ANT = 1;
const int DOODLEBUG = 2;
const int ANTS_NUM = 100; // the field is initialized with 100 ants
const int DOODLEBUGS_NUM = 5; // the field is initialized with 5 doodlebugs

class Field { 
public:
    void setOrganisms(); 
    void startSimulation(); 
    void printOutput(); 
    int getTimeSteps() const {return timeSteps;}
    int getRandomNumber(int highestValue, int supplementary = 0) {return ((rand() % highestValue) + supplementary);}
    Field(int doodleBugsCount, int antsCount) : doodlebugsNum(doodleBugsCount), timeSteps(0),
    antsNum(antsCount) {this->setOrganisms();}
    Field() : doodlebugsNum(DOODLEBUGS_NUM), antsNum(ANTS_NUM), timeSteps(0) {this->setOrganisms();}
    ~Field();
    Organism* grid[SIZE][SIZE];
private:
    int antsNum;
    int doodlebugsNum;
    int timeSteps;
};

class Organism { 
public:
    virtual void moveAhead(); 
    virtual bool starve() {return false;} 
    virtual void breeding() = 0; 
    virtual int getOrganism() = 0; 
    void getNextCell(int direction, int& theRow, int& theColumn);
    bool isWithinGrid(int theRow, int theColumn);
    vector<int> getEmptyCells(int theRow, int theColumn);
    Organism(Field* theField, int theRow, int theColumn) : field(theField), rowPosition(theRow), columnPosition(theColumn), breedeDuration(0), 
    totalLife(theField->getTimeSteps()) {}
    Organism() : field(nullptr), rowPosition(0), columnPosition(0), breedeDuration(0), totalLife(0) {}

protected:
    Field* field;
    int rowPosition;
    int columnPosition;
    int breedeDuration;
    int totalLife;
};

class DoodleBug : public Organism { 
public:
    virtual int getOrganism() {return DOODLEBUG;} 
    virtual void moveAhead(); 
    virtual void breeding(); 
    virtual bool starve() {return starveDuration == 3;} 
    DoodleBug(Field* theField, int theRow, int theColumn) : Organism(theField, theRow, theColumn), starveDuration(0) {}
    DoodleBug() : Organism(), starveDuration(0) {}
private:
    int starveDuration;
};

class Ant : public Organism { 
public:
    virtual int getOrganism() {return ANT;} 
    virtual void breeding(); 
    virtual void moveAhead(); 
    Ant(Field* theField, int theRow, int theColumn) : Organism(theField, theRow, theColumn) {}
    Ant() : Organism(){};
};