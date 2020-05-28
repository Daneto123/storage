// Проект 12: Склад
// Да се напише програма, реализираща информационна система, обслужваща склад. Програмата съхранява и обработва данните за наличността в 
// склада във файл.
// За всеки продукт се съхранява следната информация:

// -име (описание на продукта, символен низ с произволна дължина)
// -срок на годност
// -дата на постъпване в склада
// -има не производител
// -мерна единица (килограми, литри)
// -налично количество
// -местоположение (секция/рафт/номер)
// -номерирайте склада си както прецените, че ще ви е удобно, имайте предвид, че в началото той е празен и различно количество стока е 
//  нормално да заема различно по обем място
// -коментар (свободен текст)

// След като приложението отвори даден файл, то трябва да може да извършва посочените по-долу операции, в допълнение на общите операции (open, close, save, save as, help и exit):

// print
// Извежда информация за наличните продукти в склада
// add
// Добавя нов продукт в склада в диалогов режим, като въвежда всички данни за продукта и ги проверява за коректност. При добавянето на продукта се спазват следните правила:
// ако нов продукт е с различен срок на годност от вече съществуващ едноименен продукт, той да бъде поставен на различно място
// ако имате достатъчно място, еднакви продукти с един и същи срок на годност да бъдат поставени на едно и също място
// при извеждане на списъка с налични продукти да се изведе общото количество на едноименните продукти независимо от срока им на годност

// remove 
// Изважда продукт от склада в диалогов режим:
// по дадено име и количество изважда съответните продукти от склада и извежда информацията за продукта и къде се е намирал
// при наличие на повече от една партида, първо намалява тази със най-скоро изтичащ срок на годност, тогава във информацията за извършеното действие се отбелязва количеството и мястото на всяка от партидите, които сме намалили
// в случай на опит за изваждане на повече от наличното да се дава информация на потребителя за наличността на продукта и срока на годност на партидите и възможност да реши дали все пак не иска да извади това което е останало

// log <from> <to>
// Извежда справка за всички промени в наличността в периода от дата <from> до дата <to>, включително зареждания и извеждания на стоки.
// clean
// Разчиства склада от всички стоки, на които е изтекъл или предстои скоро да изтече срока на годност, като извежда информация за разчистените стоки
#include <iostream>
#include "storage.cpp"
#include "product.cpp"
#include "command.cpp"
#include "file.cpp"
#include "exeption.cpp"
#include <stdio.h>

int main(){

//примерен тест
    //Storage storage;

    // Product product1("banan", 3052020, 3012020, "ivan", "kg", 20, "tova_e_testov_product");
    // Product product2("banan", 3052020, 3062020, "ivan", "kg", 20, "tova_e_testov_product");//ednakuv s purviqt trqbva da go pribavi
    // Product product3("kiwi", 5052020, 5032020, "ivan", "mkr", 21, "tova_e_testov_product");//ima greshna merna edenica
    // Product product4("kiwi", 6052020, 5032020, "ivancho", "kg", 5, "tova_e_testov_product");//ima ednakvo ime s gorniqt product i trvqbva pri printa da go izkara samo kiwi
    // Product product5("honey", 4052021, 3042021, "ivan", "kg", 23, "tova_e_testov_product");
    // Product product6("sugar", 4052021, 3052021, "ivan", "kg", 24, "tova_e_testov_product");//ima ednakva locaciq s gorniqt product i trqbva da zadam nova
    // Product product7("milk", 1234, 3042021, "ivan", "l", 25, "tova_e_testov_product");//ima greshna data trqbva da se zapishe nova
    // Product product8("biscuit", 4042021, 12510, "ivan", "kg", 26, "tova_e_testov_product");//greshno zadadena data trqbva da se zadade nova
    // Product product9("desert", 4032021, 3042021, "ivan", "kg", 27, "tova_e_testov_product");//posle mahame 27 kg trqbva da go iztrie;

    // storage.add_product(product1);
    // storage.add_product(product2);
    // storage.add_product(product3);
    // storage.add_product(product4);
    // storage.add_product(product5);
    // storage.add_product(product6);
    // storage.add_product(product7);
    // storage.add_product(product8);
    // storage.add_product(product9);

    // storage.show_products();
    // storage.remove_product("desert", 27, "kg");//raboti
    // storage.remove_product("biskcuit", 10, "kg");//raboti
    // storage.clean_odd_products();
    // storage.show_products();//raboti
    // storage.in_to_file("product1.txt");
    // storage.log_products(3052020, 5042021);
    // storage.print_file("product1.txt");
    
    cout<<"in the app"<<endl;
    string input;
    getline(cin, input);

    Storage storage;
    Command commands;

    while(input != exit_command){

        vector<string> words = split(input);
        string command = words[0];

        if(command == open_command){

            string file_name = words[1];
            commands.open(file_name);

        }else if(command == close_command){

            commands.close();

        }else if(command == save_command){

            commands.save();

        }else if(command == save_as_command){

            string file_name = words[1];
            commands.save_as(file_name);

        }else if(command == help_command){

            commands.help();

        }else if(command == add_command){

            char* name = new char[words[1].length()+1];
            strcpy(name, words[1].c_str());
            int expiration_date = stoi(words[2]);
            int date_in_storage = stoi(words[3]);
            char* name_of_manufactor = new char[words[4].length()+1];
            strcpy(name_of_manufactor, words[4].c_str());
            char* unit = new char[words[5].length()+1];
            strcpy(unit, words[5].c_str());
            int quantity = stoi(words[6]);
            char* comment = new char[words[7].length()+1];
            strcpy(comment, words[7].c_str());

            Product product(name, expiration_date, date_in_storage, name_of_manufactor, unit, quantity, comment);
            storage.add_product(product);
            storage.in_to_file("product1.txt");

            delete[] name; delete[] name_of_manufactor; delete[] unit, delete[] comment;

        }else if(command == clear_command){

            // char* file_for_save = new char[words[1].length()+1];
            // strcpy(file_for_save, words[1].c_str());

            storage.clean_odd_products();
            storage.in_to_file("product1.txt");

        }else if(command == remove_command){
            
            char* name_of_product = new char[words[1].length()+1];
            strcpy(name_of_product, words[1].c_str());
            int quality = stoi(words[2]);
            char* unit_of_product = new char[words[3].length()+1];
            strcpy(unit_of_product, words[3].c_str());
            // char* file_for_save = new char[words[4].length()+1];
            // strcpy(file_for_save, words[4].c_str());

            storage.remove_product(name_of_product, quality, unit_of_product);
            storage.in_to_file("product1.txt");

            delete[] name_of_product;
            delete[] unit_of_product;

        }else if(command == show_command){

            storage.show_products();

        }else if(command == log_command){

            int start_date = stoi(words[1]);
            int end_date = stoi(words[2]);
            storage.log_products(start_date, end_date);

        }else if(command == print_command){

            //char* file_name_print = new char[words[1].length()+1];
            //strcpy(file_name_print, words[1].c_str());
            storage.print_file("product1.txt");

            //delete[] file_name_print;

        }

        getline(cin, input);

    }

    return 0;
}