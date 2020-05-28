#pragma once
#include "product.h"

/**
 * дефолтен конструктор
 */

Product::Product() : name(nullptr), name_of_manufactor(nullptr), unit(nullptr), comment(nullptr), date_in_storage(0), expiration_date(0), available_quantity(0) {}

/**
 * конструктор за копиране
 * @param name, @param name_of_manufactor, @param unit, @param comment, @param date_in_storage, @param expiration_date, @param available_quantity
 */

Product::Product(const char* name, int expiration_date, int date_in_storage ,const char* name_of_manufactor, const char* unit, int available_quantity, const char* comment){
    this->name = new char[strlen(name) + 1];
    strcpy(this->name, name);
    this->expiration_date = expiration_date;
    this->date_in_storage = date_in_storage;
    this->name_of_manufactor = new char[strlen(name_of_manufactor) + 1];
    strcpy(this->name_of_manufactor, name_of_manufactor);
    this->unit = new char[strlen(unit) + 1];
    strcpy(this->unit, unit);
    this->available_quantity = available_quantity;
    this->comment = new char[strlen(comment) + 1];
    strcpy(this->comment, comment);
}

/**
 * коструктор за присвояване
 */

Product::Product(const Product &other){
    strcpy(name,other.name);
    expiration_date = other.expiration_date;
    date_in_storage = other.date_in_storage;
    strcpy(name_of_manufactor,other.name_of_manufactor);
    unit = other.unit;
    available_quantity = other.available_quantity;
    strcpy(comment,other.comment);
}

/**
 * коструктор за присвояване
 */

Product& Product::operator= (const Product &other){
    if(this != &other){
        strcpy(name,other.name);
        expiration_date = other.expiration_date;
        date_in_storage = other.date_in_storage;
        strcpy(name_of_manufactor,other.name_of_manufactor);
        unit = other.unit;
        available_quantity = other.available_quantity;
        strcpy(comment,other.comment);
    }

    return *this;
}

/**
 * деструктор
 */

Product::~Product(){
    delete[] this->name;
    delete[] this->name_of_manufactor;
    delete[] this->unit;
    delete[] this->comment;
}