#pragma once
#include "product.h"
#include "location.h"
#include "location.cpp"
#include <fstream>

class Storage : public Product{
    //friend class Product;
    friend class Location;
protected:
    int currentNumber;
    Product** products;

    void remove_el();

public:

    Storage();
    Storage(int currentNumber);
    Storage(const Storage& other);
    Storage& operator= (const Storage& other);
    ~Storage();
    void add_product(Product &product);
    void clean_odd_products();
    void log_products(int start_date, const int end_date);
    void remove_product(const char* name_to_remove, int quantity, const char* unit_to_remove);
    void show_products();
    void move(int i);
    void in_to_file(const char* file_path);
    void print_file(const char* file_path);

};