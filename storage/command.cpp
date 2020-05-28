#include "exeption.cpp"
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
        cout<<open_successfuly;
    }else{
        cout<<open_problem;
    }

}

void Command::close(){

    if(file.is_open()){
        cout<<close_successfuly;
        file.close();
    }else{
        cout<<open_problem;
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
        cout<<save_successfuly;
    }else{
        cout<<open_problem;
    }

}

void Command::help(){

    cout<<help_list;

}

// int main(){

//     Command command;
//     //command.open("product1.txt");
//     //command.close();
//     //command.save();
//     //command.save_as("product1.txt");
//     //command.help();

//     return 0;
// }