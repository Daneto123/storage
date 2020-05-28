#pragma once
#include<vector>
using namespace std;

class Position {
private:
    int section;
    int shelf;
    int num;

public:

    Position() : section(0), shelf(0), num(0) {}
    Position(int section, int shelf, int num) : section(section), shelf(shelf), num(num) {}
    Position(const Position &other){
        section = other.section;
        shelf = other.shelf;
        num = other.num;
    }
    Position& operator=(const Position &other){
        if(this != &other){
            section = other.section;
            shelf = other.shelf;
            num = other.num;
        }

        return *this;
    }
    ~Position(){

    }

    void set_section(int section){section = section; }
    void set_shelf(int shelf){shelf = shelf; }
    void set_num(int num){num = num; }

    int get_section()const { return section; }
    int get_shelf()const { return shelf; }
    int get_num()const { return num; }

};

class Location : public Position{
    friend class Position;
private:
    int currentNumberLocation;
    Position** locations;

public:
    void remove_el(){

        for(int i = 0;i<currentNumberLocation;i++){
            delete locations[i];
        }

        delete[] locations;
    }

    Location(){
        locations = new Position*[10];
        currentNumberLocation = 0;
    }

    Location(int currentNumberLocation){
        locations = new Position*[currentNumberLocation];
        currentNumberLocation = currentNumberLocation;
    }

    Location(const Location& other){
        locations = new Position*[10];
        for (int i = 0; i < other.currentNumberLocation; i++){
            locations[i] = new Position(*other.locations[i]);
        }
        currentNumberLocation = other.currentNumberLocation;
    }

    Location& operator=(const Location& other){
        if(this == &other){
            return *this;
        }
        remove_el();

        locations = new Position*[10];
        for (int i = 0; i < other.currentNumberLocation; i++){
            locations[i] = new Position(*other.locations[i]);
        }
        currentNumberLocation = other.currentNumberLocation;

        return *this;
    }

    ~Location(){
        remove_el();
    }

    void is_free_location(Position &position);
    void remove_location(int index);

};