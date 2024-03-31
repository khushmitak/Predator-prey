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

// Field class begins
void Field::setOrganisms() {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++)
            grid[i][j] = nullptr;
    }
    // assigning random positions to ants and doodlebugs
    srand(time(0));
    int antsCount = 0;
    while (antsCount < antsNum){
        int i = getRandomNumber(SIZE);
        int j = getRandomNumber(SIZE);
        if (grid[i][j] != nullptr) 
        continue;
        grid[i][j] = new Ant(this, i, j);
        antsCount++;
    }

    int doodleBugsCount = 0;
    while (doodleBugsCount < doodlebugsNum) {
        int i = getRandomNumber(SIZE);
        int j = getRandomNumber(SIZE);
        if (grid[i][j] != nullptr) 
        continue;
        grid[i][j] = new DoodleBug(this, i, j);
        doodleBugsCount++;
    }
    printOutput(); 
}

void Field::startSimulation() {
    cout << "Press the enter key for next time step after every step \n";
    char enter;
    cin.get(enter);

    if (enter != '\n') {
        cout << "Invalid key entered. \n";
        cout << "The simulation is now ended. \n";
        exit(1);
    }

    while (enter == '\n') {
        timeSteps++;
        //Moving the doodlebugs and ants
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (grid[i][j] == nullptr)
                    continue;
                if (grid[i][j]->getOrganism() == DOODLEBUG)
                    grid[i][j]->moveAhead();
            }
        }
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (grid[i][j] == nullptr)
                    continue;
                if (grid[i][j]->getOrganism() == ANT)
                    grid[i][j]->moveAhead();
            }
        }
        //Breeding 
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (grid[i][j] == nullptr)
                    continue;
                grid[i][j]->breeding();
            }
        }
        //remove the starving doodlebugs
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (grid[i][j] == nullptr)
                    continue;
                if (grid[i][j]->starve()) {
                    delete grid[i][j];
                    grid[i][j] = nullptr;
                }
            }
        }
        printOutput(); 

        cin.get(enter);
        if (enter != '\n') {
            cout << "Invalid key entered. \n";
            cout << "The simulation is now ended. \n";
            exit(1);
        }
    }
}

void Field::printOutput() {
    for (auto & row : grid) {
        for (auto organism : row) {
            if (organism == nullptr)
                cout << "- ";
            else if (organism->getOrganism() == ANT)
                cout << "o ";
            else
                cout << "X ";
        }
        cout << endl;
    }
}

Field::~Field() {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (grid[i][j] != nullptr)
                delete grid[i][j];
        }
    }
}
// Field class ends

//Organism class begins
void Organism::moveAhead() {
    totalLife++;
    breedeDuration++;
    int theRow = rowPosition;
    int theColumn = columnPosition;
    int direction = field->getRandomNumber(4,1); // 4 is the total possible directions
    getNextCell(direction, theRow, theColumn);

    if (isWithinGrid(theRow, theColumn)) {
        if (field->grid[theRow][theColumn] != nullptr) {
            return;
        }
        else {
            field->grid[theRow][theColumn] = this; // moves when next cell is empty
            field->grid[rowPosition][columnPosition] = nullptr;
            rowPosition = theRow;
            columnPosition = theColumn;
        }
    }
}

void Organism::getNextCell(int direction, int& theRow, int& theColumn) {
    if (direction == 1) {
        theRow--; // upwards direction
    }
    else if (direction == 2) {
        theRow++; // downwards direction
    }
    else if (direction == 3) {
        theColumn--; // left direction
    }
    else if (direction == 4) {
        theColumn++; // right direction
    }
    else {
        cout << "Wrong direction \n";
        exit(1);
    }
}

bool Organism::isWithinGrid(int theRow, int theColumn) {
    return ((theRow >= 0) && (theRow < SIZE) && (theColumn >= 0) && (theColumn < SIZE));
}

vector<int> Organism::getEmptyCells(int row, int column) {

    vector<int> nextEmpty;
    for (int direction = 1; direction <= 4; direction++) {
        int tempR = row;
        int tempC = column;
        getNextCell(direction, tempR, tempC);

        if ((isWithinGrid(tempR, tempC)) && (field->grid[tempR][tempC] == nullptr))
            nextEmpty.push_back(direction);
    }
    return nextEmpty;
}
// Organism class ends

//Doodlebug class begins
void DoodleBug::moveAhead() {
    if (this->totalLife == field->getTimeSteps())
        return;
    //check for adjacent ants
    vector<int> adjacentAnts;

    for (int direction = 1; direction <= 4; direction++) {
        int row = rowPosition;
        int column = columnPosition;
        getNextCell(direction, row, column);
        if ((isWithinGrid(row, column)) && (field->grid[row][column] != nullptr)
        && (field->grid[row][column]->getOrganism() == ANT))
            adjacentAnts.push_back(direction);
    }

    int row = rowPosition;
    int column = columnPosition;
    //eat the adjacent ants, if any
    if (!(adjacentAnts.empty())){
        int randomPosition = field->getRandomNumber(adjacentAnts.size());
        int direction = adjacentAnts[randomPosition];
        getNextCell(direction, row, column);

        delete field->grid[row][column];
        field->grid[row][column] = this;
        field->grid[rowPosition][columnPosition] = nullptr;
        rowPosition = row;
        columnPosition = column;
        totalLife++;
        breedeDuration++;
        starveDuration = 0;
    }
    else {
        Organism::moveAhead(); // if no ants, move to next cell
        starveDuration++;
    }
}

void DoodleBug::breeding() {
    if (breedeDuration >= 8) { // 8 is the doodlebug breed period
        int row = rowPosition;
        int column = columnPosition;
        vector<int> nextEmpty = getEmptyCells(row, column);

        if (!(nextEmpty.empty())) {
            int randomPosition = field->getRandomNumber(nextEmpty.size());
            int direction = nextEmpty[randomPosition];
            getNextCell(direction, row, column);
            field->grid[row][column] = new DoodleBug(field, row, column);
            breedeDuration = 0;
        }
    }
}

//Doodlebug class ends

// Ant class begins
void Ant::moveAhead() {
    if (this->totalLife == field->getTimeSteps()) {
        return;
    }
    else
        Organism::moveAhead();
}

void Ant::breeding() {
    if (this->breedeDuration >= 3) { // 3 is the breeding period for ants
        int row= rowPosition;
        int column = columnPosition;
        vector<int> nextEmpty = getEmptyCells(row, column);
        if (!(nextEmpty.empty())) {
            int randomIndex = field->getRandomNumber(nextEmpty.size());
            int direction = nextEmpty[randomIndex];
            getNextCell(direction, row, column);
            field->grid[row][column] = new Ant(field, row, column);
            breedeDuration = 0;
        }
    }
}
// Ant class ends

int main() {
    Field simulation(5, 100);
    simulation.startSimulation(); // plays the simulation as long as enter is pressed every step
    return 0;
}