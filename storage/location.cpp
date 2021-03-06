#include "location.h"

/**
* този метод проверява дали локацията в склада е свободна
* @param location - локацията въведена от потребителя
*/

void Location::is_free_location(Position *position){

    bool flag = true;
    int new_loct, count;

    for (int i = 0; i < locations.size(); i++){
        if((*locations[i]).get_shelf() == (*position).get_shelf()){
            if((*locations[i]).get_num() == (*position).get_num()){
                cout<<"please enter new location ";
                cin>>new_loct;
                int section = (new_loct/10000)%10;
                int shelf = (new_loct/100)%100;
                int num = (new_loct%100);

                locations.push_back(new Position(section, shelf, num));
                //cout<<(*locations[i]).get_section()<<(*locations[i]).get_shelf()<<(*locations[i]).get_num()<<endl;
                flag = false;
            }
        }
    }

    if(flag == true){
        locations.push_back(position);
        //cout<<"you successfuly add product "<<(*locations[count]).get_section()<<(*locations[count]).get_shelf()<<(*locations[count]).get_num()<<endl;
        count++;
    }

}

/**
* този метод изчиства локация
* @param index - индех на елемента който искаме да махнем
*/

void Location::remove_location(int index){

    locations.erase(locations.begin() + index);
    
    cout<<"location is eraced successfuly"<<endl;

}