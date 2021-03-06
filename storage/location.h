#pragma once
#include<iostream>
#include<vector>
using namespace std;

class Position {
private:
    int _section;
    int _shelf;
    int _num;

public:

    Position() : _section(0), _shelf(0), _num(0) {}
    
    Position(int _section, int _shelf, int _num) : _section(_section), _shelf(_shelf), _num(_num) {}
    
    Position(const Position &other) : _section(other._section), _shelf(other._shelf), _num(other._num) {}
    
    Position& operator=(const Position &other){
        if(this != &other){
            _section = other._section;
            _shelf = other._shelf;
            _num = other._num;
        }

        return *this;
    }

    void set_section(int _section){ this->_section = _section; }
    void set_shelf(int _shelf){ this->_shelf = _shelf; }
    void set_num(int _num){ this->_num = _num; }

    int get_section() { return _section; }
    int get_shelf() { return _shelf; }
    int get_num() { return _num; }

};

class Location : public Position{
    friend class Position;
protected:
    vector<Position*> locations;
public:

    Location() : locations() {}

    Location(const vector<Position*> locations) : locations(locations) {}

    Location(const Location& other) : locations(other.locations) {}

    Location& operator=(const Location& other){
       if(this == &other){
           locations = other.locations;
       }

      return *this;
    }

    ~Location(){
        
        for(auto *l : locations){
            delete l;
        }
    }

    void is_free_location(Position *position);
    void remove_location(int index);

};