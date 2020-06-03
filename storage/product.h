#pragma once
#include<iostream>
#include<cstring>
#include<fstream>
#include "location.h"
using namespace std;

const int char_buffer = 64;
const int _date = 10;

class Product : public Location{
public:
    char* _name ;
    int expiration_date ;
    int date_in_storage ;
    char* name_of_manufactor ;
    char* _unit ;
    int available_quantity;
    char* _comment ;

    Product();
    Product(const char*, int, int, const char*, const char*, int, const char*);
    Product(const Product &other);
    Product& operator= (const Product &other);
    ~Product();

    void set_name(const char* _name){
        this->_name = new char[strlen(_name) + 1];
        strcpy(this->_name,_name);
        delete[] _name;
    }

    void set_expiration_date(int expiration_date){this->expiration_date = expiration_date;}

    void set_date_in_storage(int date_in_storage){this->date_in_storage = date_in_storage;}

    void set_name_of_manufactor(const char* name_of_manufactor){
        this->name_of_manufactor = new char[strlen(name_of_manufactor) + 1];
        strcpy(this->name_of_manufactor, name_of_manufactor);
        delete[] name_of_manufactor;
    }

    void set_unit(const char* _unit){
        this->_unit = new char[strlen(_unit) + 1];
        strcpy(this->_unit,_unit);
        delete[] _unit;
    }

    void set_available_quantity(int available_quantity){this->available_quantity = available_quantity;}

    void set_comment(const char* _comment){
        this->_comment = new char[strlen(_comment) + 1];
        strcpy(this->_comment,_comment);
        delete[] _comment;
    }

    const char* get_name()const{return this->_name;}
    int get_expiration_date()const {return this->expiration_date;}
    int get_date_in_storage()const {return this->date_in_storage;}
    const char* get_name_of_manufactor()const{return this->name_of_manufactor;}
    char* get_unit()const{return this->_unit;}
    int get_available_quantity()const{return this->available_quantity;}
    const char* get_comment()const{return this->_comment;}

    void printinfo() const{
        cout<<this->get_name()<<endl;
        cout<<this->get_expiration_date()<<endl;   
        cout<<this->get_name_of_manufactor()<<endl;   
        cout<<this->get_unit()<<endl;        
    }

};