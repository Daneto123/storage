#pragma once
#include "exeption.cpp"
#include "storage.h"
#include "function_for_currect_data.cpp"

Storage::Storage(){
    products = new Product*[10];
    currentNumber = 0;
}

Storage::Storage(int currentNumber){
    products = new Product*[currentNumber];
    currentNumber = currentNumber;
}

Storage::Storage(const Storage& other){
    products = new Product*[10];
    for (int i = 0; i < other.currentNumber; i++){
        products[i] = new Product(*other.products[i]);
    }
    currentNumber = other.currentNumber;
}

Storage& Storage::operator=(const Storage& other){
    if(this == &other){
        return *this;
    }
    remove_el();

    products = new Product*[10];
    for (int i = 0; i < other.currentNumber; i++){
        products[i] = new Product(*other.products[i]);
    }
    currentNumber = other.currentNumber;

    return *this;
}

void Storage::add_product(Product &product){

    //cout<<product.name<<endl;
    //cout<<product.comment<<endl;
    char* new_unit = new char[8];
    int quantity = 0, new_date = 0, loct = 0;
    bool flag = true, flag_ed = false, flag_ds = false, flag_u = false, flag_l = false;

    for(int i = 0 ;i < currentNumber;i++){
        if(strcmp((*products[i]).getName(), product.getName()) == 0){
            if((*products[i]).getExpiration_date() == product.getExpiration_date()){ 
                if(is_valid_date((*products[i]).getExpiration_date()) == true){
                    (*products[i]).available_quantity += product.getAvailable_quantity();
                    flag = false;
                }else{
                    cout<<not_valid_expiration_date;
                }
            }
        }
    }

    if(flag == true){

        if(is_valid_date(product.getExpiration_date())){
            flag_ed = true;
        }else{
            cout<<"please enter expiration date for " << product.getName() << " ";
            cin>>new_date;
            product.setExpiration_date(new_date);
        }

        if(is_valid_date(product.getDate_in_storage())){
            flag_ds = true;
        }else{
            cout<<"please enter the date in storage for " << product.getName() << " ";
            cin>>new_date;
            product.setDate_in_storage(new_date);
            new_date = 0;
        }

        if(currect_unit(product.getUnit())){
            flag_u = true;
        }else{
            cin>>new_unit;
            product.setUnit(new_unit);
            flag_u = true;
        }

        // Location location;
        // cout<<"enter location " << product.getName()<<" ";
        // cin>>loct;
        // int section = (loct/10000)%10;
        // int shelf = (loct/100)%100;
        // int num = (loct%100);

        // Position position(section, shelf, num);
        // location.is_free_location(position);
        
    }

    if(flag_ds == true && flag_ed == true && flag_u == true){
        products[currentNumber] = new Product(product.getName(),product.getExpiration_date(),product.getDate_in_storage(),product.getName_of_manufactor(),product.getUnit(),product.getAvailable_quantity(),product.getComment());

        cout<<"you successfuly add "<<(*products[currentNumber]).getName() << endl;
        currentNumber++;
    }

}

/**
 * изчиства продуктите с изтекъл срок на годност
 */

void Storage::clean_odd_products() {

    //int day = 27;
    //int month = 3;
    int date = 0;
    cout<<clean_odd_products_date;
    cin>>date;
    for(int i = 0; i < currentNumber;i++){
        if((*products[i]).getExpiration_date() <= date){
           move(i);
        }
    }
    cout<<clean_odd_successfuly;
}

/**
 * показва промените по продуктите в склада по зададена начална и крайна дата
 * @param start_date - начална дата
 * @param end_date - крайна дата
 */

void Storage::log_products(int start_date, int end_date){

    int k;

    cout<<"products log from "<<start_date<<" to "<<end_date<<endl<<endl;

    for(size_t i = 0;i < currentNumber;i++){
        if(start_date < (*products[i]).getDate_in_storage()){
            k = i;
            cout<<"you entered products: "<<endl;
            while(start_date<end_date){
                if(k<currentNumber){
                    if((*products[k]).getDate_in_storage() < end_date){
                        cout<<name_of_product<<(*products[k]).getName()<<endl;
                        cout<<expiration_date_of_product<<(*products[k]).getExpiration_date()<<endl;
                        cout<<date_in_storage_of_product<<(*products[k]).getDate_in_storage()<<endl;
                        cout<<quantity_of_product<<(*products[k]).getAvailable_quantity()<< " " <<endl;//products[k].getUnit()<<endl;
                        cout<<endl;
                    }
                    k++;
                    start_date +=  100000;
                }
            }
        }
    }
}

/**
 * взимаме продукт от склада
 * @param name_to_remove - продуктът които искаме да вземем от склада
 * @param available_quantity - количеството което искаме да вземем от продукта
 * @param unit_to_remove - мерната еденица на продукта
 */

void Storage::remove_product(const char* name_to_remove, int quantity, const char* unit_to_remove){

    Location location;
    for(int i = 0; i < currentNumber;i++){
        if(strcmp((*products[i]).getName(), name_to_remove) == 0){
            if(((*products[i]).getAvailable_quantity() >= quantity) && ((strcmp((*products[i]).getUnit(), unit_to_remove) == 0))){
                (*products[i]).available_quantity -= quantity;
                if((*products[i]).getAvailable_quantity() == 0){
                    cout<<cleaned_location_successfuly;
                    move(i);
                    location.remove_location(i);
                }else{
                    cout<<"you successfuly removed "<<quantity<<" left: "<<(*products[i]).getAvailable_quantity() << " " << (*products[i]).getUnit() <<" from " <<(*products[i]).name<<endl<<endl;
                }
            }
        } 
    }
}

/**
 * показва елементите от клас арейа
 */

void Storage::show_products() {
    //cout<<"pone tova dano raboti";
        
    for(int i = 0 ;i < currentNumber;i++){
        //remove_product(products[h].name, products[h].available_quantity, products[h].unit);  
        cout<<name_of_product << " is: " << (*products[i]).getName() << endl;
        cout<<quantity_of_product << "is: " << (*products[i]).getAvailable_quantity() << " " << (*products[i]).getUnit() << endl;
        cout<<endl;
    }
}

void Storage::remove_el(){

    for(int i = 0;i<currentNumber;i++){
        delete products[i];
    }

    delete[] products;
}

Storage::~Storage(){
    remove_el();
}