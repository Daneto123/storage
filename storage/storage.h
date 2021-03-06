#pragma once
#include "product.h"
#include "location.cpp"
#include <fstream>

class Storage : public Product {
    friend class Location;
protected:
    int current_number;
    Product** products;

    void remove_el();

public:

    Storage();
    Storage(int current_number);
    Storage(const Storage& other);
    Storage& operator= (const Storage& other);
    ~Storage();
    void add_product(Product &product, int location);
    void clean_odd_products(int date);
    void log_products(int start_date, const int end_date);
    void remove_product(const char* name_to_remove, int quantity, const char* unit_to_remove);
    void show_products();
    void move(int i);
    void in_to_file(const string &file_path);
    void print_file(const string &file_path);
    
};