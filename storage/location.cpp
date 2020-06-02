#include "location.h"

/**
* този метод проверява дали локацията в склада е свободна
* @param location - локацията въведена от потребителя
*/

void Location::is_free_location(Position &position){

    bool flag = true;
    int new_loct, count;

    for (int i = 0; i < locations.size(); i++){
        if(locations[i].get_shelf() == position.get_shelf()){
            cout<<"shelf-a e zaet"<<endl;
            if(locations[i].get_num() == position.get_num()){
                cout<<"please enter new num ";
                cin>>new_loct;
                locations.push_back(Position(position.get_section(), position.get_shelf(), new_loct));
                cout<<locations[i].get_section()<<locations[i].get_shelf()<<locations[i].get_num()<<endl;
                flag = false;
            }
        }
    }

    if(flag == true){
        locations.push_back(Position(position.get_section(), position.get_shelf(), position.get_num()));
        cout<<"you successfuly add product "<<locations[count].get_section()<<locations[count].get_shelf()<<locations[count].get_num()<<endl;
        count++;
    }

}

/**
* този метод изчиства локация
* @param index - индех на елемента който искаме да махнем
*/

void Location::remove_location(int index){

    locations.erase(locations.begin(), locations.begin() + index);
    
    cout<<"location is eraced successfuly"<<endl;

}