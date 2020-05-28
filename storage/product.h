#pragma once
#include<iostream>
#include<cstring>
#include<fstream>
#include "location.h"
using namespace std;

const int char_buffer = 64;
const int date = 10;

class Product : public Location{
public:
    char* name ;
    int expiration_date ;
    int date_in_storage ;
    char* name_of_manufactor ;
    char* unit ;
    int available_quantity;
    char* comment ;

    Product();
    Product(const char*, int, int, const char*, const char*, int, const char*);
    Product(const Product &other);
    Product& operator= (const Product &other);
    ~Product();

    void setName(const char* name){
        this->name = new char[strlen(name) + 1];
        strcpy(this->name,name);
        delete[] name;
    }

    void setExpiration_date(int expiration_date){this->expiration_date = expiration_date;}

    void setDate_in_storage(int date_in_storage){this->date_in_storage = date_in_storage;}

    void setName_of_manufactor(const char* name_of_manufactor){
        this->name_of_manufactor = new char[strlen(name_of_manufactor) + 1];
        strcpy(this->name_of_manufactor,name_of_manufactor);
        delete[] name_of_manufactor;
    }

    void setUnit(const char* unit){
        this->unit = new char[strlen(unit) + 1];
        strcpy(this->unit,unit);
        delete[] unit;
    }

    void setAvailable_quantity(int available_quantity){this->available_quantity = available_quantity;}

    void setComment(const char* comment){
        this->comment = new char[strlen(comment) + 1];
        strcpy(this->comment,comment);
        delete[] comment;
    }

    const char* getName()const{return this->name;}
    int getExpiration_date()const {return this->expiration_date;}
    int getDate_in_storage()const {return this->date_in_storage;}
    const char* getName_of_manufactor()const{return this->name_of_manufactor;}
    char* getUnit()const{return this->unit;}
    int getAvailable_quantity()const{return this->available_quantity;}
    const char* getComment()const{return this->comment;}

    void printinfo() const{
        cout<<this->getName()<<endl;
        cout<<this->getExpiration_date()<<endl;   
        cout<<this->getName_of_manufactor()<<endl;   
        cout<<this->getUnit()<<endl;        
    }

};