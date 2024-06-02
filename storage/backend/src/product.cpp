#pragma once
#include "product.h"

/**
 * дефолтен конструктор
 */

Product::Product() : _name(nullptr), name_of_manufactor(nullptr), _unit(nullptr), _comment(nullptr), date_in_storage(0), expiration_date(0), available_quantity(0) {}

/**
 * конструктор за копиране
 * @param _name, @param name_of_manufactor, @param _unit, @param _comment, @param date_in_storage, @param expiration_date, @param available_quantity
 */

Product::Product(const char* _name, int expiration_date, int date_in_storage ,const char* name_of_manufactor, const char* _unit, int available_quantity, const char* _comment){
    this->_name = new char[strlen(_name) + 1];
    strcpy(this->_name, _name);
    this->expiration_date = expiration_date;
    this->date_in_storage = date_in_storage;
    this->name_of_manufactor = new char[strlen(name_of_manufactor) + 1];
    strcpy(this->name_of_manufactor, name_of_manufactor);
    this->_unit = new char[strlen(_unit) + 1];
    strcpy(this->_unit, _unit);
    this->available_quantity = available_quantity;
    this->_comment = new char[strlen(_comment) + 1];
    strcpy(this->_comment, _comment);
}

/**
 * коструктор за присвояване
 */

Product::Product(const Product &other){
    strcpy(_name,other._name);
    expiration_date = other.expiration_date;
    date_in_storage = other.date_in_storage;
    strcpy(name_of_manufactor,other.name_of_manufactor);
    _unit = other._unit;
    available_quantity = other.available_quantity;
    strcpy(_comment,other._comment);
}

/**
 * коструктор за присвояване
 */

Product& Product::operator= (const Product &other){
    if(this != &other){
        strcpy(_name,other._name);
        expiration_date = other.expiration_date;
        date_in_storage = other.date_in_storage;
        strcpy(name_of_manufactor,other.name_of_manufactor);
        _unit = other._unit;
        available_quantity = other.available_quantity;
        strcpy(_comment,other._comment);
    }

    return *this;
}

/**
 * деструктор
 */

Product::~Product(){
    delete[] this->_name;
    delete[] this->name_of_manufactor;
    delete[] this->_unit;
    delete[] this->_comment;
}