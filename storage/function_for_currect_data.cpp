#pragma once
#include "storage.h"

/**
 * този метод проверява дали въведената дата е във валиден формат
 * @param date - дата въведена от потребителя
 */

bool is_valid_date(int _date) {

    int _month = (_date / 100)%100;
    int _day =  (_date)%100;
    int _year = (_date / 10000)%1000;
    bool flag = true;

    if(_date < 1012019){
        flag = false;
    }else{
        if(_month > 12 && _month < 1){
            flag = false;
        }else{
            if((_day > 31 && _day < 1) || (_day > 29 && _day < 1 && _year%4!=0)){
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

bool currect_unit(char* _unit){

    bool flag = true;

    if((_unit[0] == 'g' && _unit[1] == 'r') || (_unit[0] == 'k' && _unit[1] == 'g') || (_unit[0] == 'm' && _unit[1] == 'l') || (_unit[0] == 'l') || (_unit[0] == 'l' && _unit[1] == 'b' && _unit[2] == 's')){
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

    current_number--;

    for (int k = i; k < current_number; k++){
        (*products[k])._name = (*products[k+1])._name;
        (*products[k]).expiration_date = (*products[k+1]).expiration_date;
        (*products[k]).date_in_storage = (*products[k+1]).date_in_storage;
        (*products[k]).name_of_manufactor = (*products[k+1]).name_of_manufactor;
        (*products[k])._unit = (*products[k+1])._unit;
        (*products[k]).available_quantity = (*products[k+1]).available_quantity;
        (*products[k])._comment = (*products[k+1])._comment;
    }
    
}

