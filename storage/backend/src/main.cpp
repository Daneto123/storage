#include <iostream>
#include "storage.cpp"
#include "product.cpp"
#include "command.cpp"
#include <QApplication>
#include "mainwindow.h"
#include "file.cpp"

int main(){
    
    cout<<"Hello my storage system. Enter help for more infomation how to use it."<<endl;
    string input;
    getline(cin, input);

    Storage storage;
    Command commands;

    while(input != "exit"){

        string* words = split(input);
        string command = words[0];

        if(command == "open"){

            string file_name = words[1];
            commands.openFile(file_name);

        }else if(command == "close"){

            commands.closeFile();

        }else if(command == "save"){

            storage.in_to_file("product1.txt");

        }else if(command == "save_as"){

            string file_name = words[1];
            storage.in_to_file(file_name);

        }else if(command == "help"){

            commands.helpInterface();

        }else if(command == "add"){

            cout << "name expiration_date date_in_storage name_of_manufactor unit quantity comment \n";

            char* _name = new char[words[1].length()+1];
            strcpy(_name, words[1].c_str());
            int expiration_date = stoi(words[2]);
            int date_in_storage = stoi(words[3]);
            char* name_of_manufactor = new char[words[4].length()+1];
            strcpy(name_of_manufactor, words[4].c_str());
            char* _unit = new char[words[5].length()+1];
            strcpy(_unit, words[5].c_str());
            int quantity = stoi(words[6]);
            char* _comment = new char[words[7].length()+1];
            strcpy(_comment, words[7].c_str());
            int _location = stoi(words[8]);

            Product product(_name, expiration_date, date_in_storage, name_of_manufactor, _unit, quantity, _comment);
            storage.add_product(product, _location);
            storage.in_to_file("product1.txt");

            delete[] _name; delete[] name_of_manufactor; delete[] _unit, delete[] _comment;

        }else if(command == "clean"){

            int _date = stoi(words[1]);
            storage.clean_odd_products(_date);
            storage.show_products();
            storage.in_to_file("product1.txt");

        }else if(command == "remove"){
            
            char* name_of_product = new char[words[1].length()+1];
            strcpy(name_of_product, words[1].c_str());
            int quality = stoi(words[2]);
            char* unit_of_product = new char[words[3].length()+1];
            strcpy(unit_of_product, words[3].c_str());

            storage.remove_product(name_of_product, quality, unit_of_product);
            storage.in_to_file("product1.txt");

            delete[] name_of_product;
            delete[] unit_of_product;

        }else if(command == "show"){

            storage.show_products();

        }else if(command == "log"){

            int start_date = stoi(words[1]);
            int end_date = stoi(words[2]);
            storage.log_products(start_date, end_date);

        }else if(command == "print"){

            storage.print_file("product1.txt");

        }

        delete[] words;
        getline(cin, input);

    }

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();


    return 0;
}