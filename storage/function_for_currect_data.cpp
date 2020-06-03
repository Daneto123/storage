#pragma once
#include "storage.h"

/**
 * този метод проверява дали въведената дата е във валиден формат
 * @param date - дата въведена от потребителя
 */

bool is_valid_date(int date) {

    int month = (date / 100)%100;
    int day =  (date)%100;
    int year = (date / 10000)%1000;
    bool flag = true;

    if(date < 1012019){
        flag = false;
    }else{
        if(month > 12 && month < 1){
            flag = false;
        }else{
            if((day > 31 && day < 1) || (day > 29 && day < 1 && year%4!=0)){
                flag = false;
            }
        }
        if(flag == false){
            cout<<"please enter the valid date year/month/date/ - ****/**/**\n";
        }
    }

    return flag;
} 

/**
 * този метод проверява дали въведената мерната еденица съсществува
 * @param unit - мерна еденица въведена от потребителя
 */

bool currect_unit(char* unit){

    bool flag = true;

    if((unit[0] == 'g' && unit[1] == 'r') || (unit[0] == 'k' && unit[1] == 'g') || (unit[0] == 'm' && unit[1] == 'l') || (unit[0] == 'l') || (unit[0] == 'l' && unit[1] == 'b' && unit[2] == 's')){
        flag = true;
    }else{
        flag = false;
    }

    if(flag == false){
        cout<<"it allow only : gr,kg,ml,l,lbs | please enter the valid unit ";
    }

    return flag;

}

void Storage::move(int i){
    for (int k = i; k < current_number; k++){ 
        (*products[k])._name = (*products[k+1])._name;
        (*products[k]).expiration_date = (*products[k+1]).expiration_date;
        (*products[k]).date_in_storage = (*products[k+1]).date_in_storage;
        (*products[k]).name_of_manufactor = (*products[k+1]).name_of_manufactor;
        (*products[k])._unit = (*products[k+1])._unit;
        (*products[k]).available_quantity = (*products[k+1]).available_quantity;
        (*products[k])._comment = (*products[k+1])._comment;
    }
    current_number--;
}

// void Storage::move(int i){
//     for (int k = i; k < current_number; k++){ 
//         delete products[i];
//     }
//     current_number--;
// }
