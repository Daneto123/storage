#include "storage.h"
#include "function_for_currect_data.cpp"

//ideqta e vzaimstvana ot internet
Storage::Storage(){
    products = new Product*[current_number];
    current_number = 0;
}

Storage::Storage(int current_number){
    products = new Product*[current_number];
    current_number = current_number;
}

Storage::Storage(const Storage& other){
    products = new Product*[other.current_number];
    for (int i = 0; i < other.current_number; i++){
        products[i] = new Product(*other.products[i]);
    }
    current_number = other.current_number;
}

Storage& Storage::operator=(const Storage& other){

    if(this != &other){
        remove_el();

        products = new Product*[other.current_number];
        for (int i = 0; i < other.current_number; i++){
            products[i] = new Product(*other.products[i]);
        }
        current_number = other.current_number;
    }

    return *this;
}

// do tuk 

/**
 * метод за добавяне на продукт
 * @param Product - обект  
 * @param location - локация на продукта
 */

void Storage::add_product(Product &product, int location){

    //cout<<product.name<<endl;
    //cout<<product.comment<<endl;
    char* new_unit = new char[8];
    int quantity = 0, new_date = 0;
    bool flag = true, flag_ed = false, flag_ds = false, flag_u = false;

    for(int i = 0 ;i < current_number;i++){
        if(strcmp((*products[i]).get_name(), product.get_name()) == 0){
            if((*products[i]).get_expiration_date() == product.get_expiration_date()){ 
                if(is_valid_date((*products[i]).get_expiration_date()) == true){
                    (*products[i]).available_quantity += product.get_available_quantity();
                    flag = false;
                }else{
                    cout<<"please enter valid expiration date of product\n";
                }
            }
        }
    }

    if(flag == true){

        if(is_valid_date(product.get_expiration_date())){
            flag_ed = true;
        }else{
            cout<<"please enter expiration date for " << product.get_name() << " ";
            cin>>new_date;
            product.set_expiration_date(new_date);
            flag_ed = true;
        }

        if(is_valid_date(product.get_date_in_storage())){
            flag_ds = true;
        }else{
            cout<<"please enter the date in storage for " << product.get_name() << " ";
            cin>>new_date;
            product.set_date_in_storage(new_date);
            new_date = 0;
            flag_ds = true;
        }

        if(currect_unit(product.get_unit())){
            flag_u = true;
        }else{
            cin>>new_unit;
            product.set_unit(new_unit);
            flag_u = true;
        }

        int section = (location/10000)%10;
        int shelf = (location/100)%100;
        int num = (location%100);

        is_free_location(new Position(section, shelf, num));
        
    }

    if(flag_ds == true && flag_ed == true && flag_u == true){
        products[current_number] = new Product(product.get_name(),product.get_expiration_date(),product.get_date_in_storage(),product.get_name_of_manufactor(),product.get_unit(),product.get_available_quantity(),product.get_comment());

        cout<<"you successfuly add "<<(*products[current_number]).get_name() << endl;
        current_number++;
    }

}

/**
 * изчиства продуктите с изтекъл срок на годност
 * @param date - дата под която да изчисти 
 */

void Storage::clean_odd_products(int _date) {

    //int day = 27;
    //int month = 3;

    for(int i = 0; i < current_number;i++){
        if((*products[i]).get_expiration_date() <= _date){
           move(i);
           remove_location(i);
        }
    }
    cout<<"the storage is successfuly cleaned from odd products\n";
}

/**
 * показва промените по продуктите в склада по зададена начална и крайна дата
 * @param start_date - начална дата
 * @param end_date - крайна дата
 */

void Storage::log_products(int start_date, int end_date){

    int k;

    cout<<"products log from "<<start_date<<" to "<<end_date<<endl<<endl;

    for(size_t i = 0;i < current_number;i++){
        if(start_date < (*products[i]).get_date_in_storage()){
            cout<<"you entered products: "<<endl;
            for(int m = start_date, k = i; start_date <= end_date && k<current_number;m += 10000, k++){
                if((*products[k]).get_date_in_storage() <= end_date){
                    cout<<"name of the product "<<(*products[k]).get_name()<<endl;
                    cout<<"the expiration date of the product "<<(*products[k]).get_expiration_date()<<endl;
                    cout<<"date in storage of the product "<<(*products[k]).get_date_in_storage()<<endl;
                    cout<<"available quantity of the product "<<(*products[k]).get_available_quantity()<< " " <<(*products[k]).get_unit()<<endl;
                    cout<<endl;
                }
            }
        }
        break;
    }
}

/**
 * взимаме продукт от склада
 * @param name_to_remove - продуктът които искаме да вземем от склада
 * @param available_quantity - количеството което искаме да вземем от продукта
 * @param unit_to_remove - мерната еденица на продукта
 */

void Storage::remove_product(const char* name_to_remove, int quantity, const char* unit_to_remove){

    Location locat;
    for(int i = 0; i < current_number;i++){
        if(strcmp((*products[i]).get_name(), name_to_remove) == 0){
            if(((*products[i]).get_available_quantity() >= quantity) && ((strcmp((*products[i]).get_unit(), unit_to_remove) == 0))){
                (*products[i]).available_quantity -= quantity;
                if((*products[i]).get_available_quantity() == 0){
                    cout<<"you successfuly removed location\n";
                    move(i);
                    locat.remove_location(i);
                }else{
                    cout<<"you successfuly removed "<<quantity<<" left: "<<(*products[i]).get_available_quantity() << " " << (*products[i]).get_unit() <<" from " <<(*products[i]).get_name()<<endl<<endl;
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
        
    for(int i = 0 ;i < current_number;i++){
        //remove_product(products[h].name, products[h].available_quantity, products[h].unit);  
        cout<<"name of the product " << " is: " << (*products[i]).get_name() << endl;
        cout<<"available quantity of the product " << "is: " << (*products[i]).get_available_quantity() << " " << (*products[i]).get_unit() << endl;
        cout<<endl;
    }
}

void Storage::remove_el(){

    for(int i = 0;i<current_number;i++){
        delete products[i];
    }

    delete[] products;
}

Storage::~Storage(){
    remove_el();
}