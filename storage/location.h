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
protected:
    vector<Position> locations;
public:

    Location() : locations() {}

    Location(vector<Position> locations) : locations(locations) {}

    Location(const Location& other) : locations(other.locations) {}

    Location& operator=(const Location& other){
        if(this == &other){
            locations = other.locations;
        }

        return *this;
    }

    void is_free_location(Position &position);
    void remove_location(int index);

};