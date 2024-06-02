#include "command.h"

/**
 * по подадено име на файл, който се намира в папката database се зарежда в системата 
 * може да се отваря само по един файл в даден момент
 * @param file_name - името на фаелът с които искаме да работим
 */
void Command::openFile(const string &file_name){

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

/**
 * това, че фаелът е обект на класът ни дава възможност да не подаваме името на фаелът, за да го затворим 
 */
void Command::closeFile(){

    if(file.is_open()){
        cout<<"the file was successfuly closed\n";
        file.close();
    }else{
        cout<<"can't open the file\n";
    }

}

void Command::helpInterface(){

    cout << "The following commands are supported:\n"
            "open <file>        opens <file>\n"
            "close			    closes currently opened file\n"
            "save			    saves the currently open file\n"
            "saveas <file>	    saves the currently open file in <file>\n"
            "add			    add product to storage \n"
            "exit			    exit the program\n";

}