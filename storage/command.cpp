#include "command.h"

void Command::open(string file_name){

    file.open(file_name);
    if (file.is_open()){
        string line;
        while (!file.eof()){
            getline(file, line);
            file_read.push_back(line);
            size_of_vector++;
        }
        cout<<"successfuly opened\n";
    }else{
        cout<<"can't open the file\n";
    }

}

void Command::close(){

    if(file.is_open()){
        cout<<"the file was successfuly closed\n";
        file.close();
    }else{
        cout<<"can't open the file\n";
    }

}
    
void Command::save(){
    string delault_name = "product1.txt"; 
    save_as("product1.txt");
}

void Command::save_as(string file_name){

    file.open(file_name);
    if (file.is_open()){
        for(int i = 0; i<size_of_vector;i++){
            file << file_read[i] << endl;
        }
        cout<<"successfuly saved\n";
    }else{
        cout<<"can't open the file\n";
    }

}

void Command::help(){

    cout << "The following commands are supported:\n"
            "open <file>	    opens <file>\n"
            "close			closes currently opened file\n"
            "save			    saves the currently open file\n"
            "saveas <file>	saves the currently open file in <file>\n"
            "help			    prints this information\n"
            "exit			    exists the program\n";

}