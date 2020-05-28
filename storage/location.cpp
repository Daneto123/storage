#pragma once
#include "location.h"

/**
* този метод проверява дали локацията в склада е свободна
* @param location - локацията въведена от потребителя
*/

void Location::is_free_location(Position &position){

    //bool flag = false;

    // for (size_t i = 0; i < currentNumberLocation; i++){
    //     if(((*locations[i]).get_section() == position.get_section()) && ((*locations[i]).get_shelf() == position.get_shelf()) && ((*locations[i]).get_num() == position.get_num())){
    //     if((*locations[i]).get_section() != position.get_section()){
    //         flag = false;
    //         std::cout<<"false";
    //     }
    // }

    //if(flag == false){
        locations[currentNumberLocation] = new Position(position.get_section(), position.get_shelf(), position.get_num());
        cout<<currentNumberLocation<<"you successfuly add product "<<(*locations[currentNumberLocation]).get_section()<<(*locations[currentNumberLocation]).get_shelf()<<(*locations[currentNumberLocation]).get_num()<<endl;
        currentNumberLocation++;

        //flag = true;
    //}


    //return flag;

}

void Location::remove_location(int index){

    for (size_t i = index; i < currentNumberLocation; i++){
        (*locations[i]).set_section((*locations[i+1]).get_section());
        (*locations[i]).set_shelf((*locations[i+1]).get_shelf());
        (*locations[i]).set_num((*locations[i+1]).get_num());

    }
    
    cout<<"location is eraced successfuly"<<endl;

}