#include "exeption.cpp"
#include "storage.h"

/**
 * пълни данните в файл
 * @param file_path - името на фаелът с които искаме да работим
 */

void Storage::in_to_file(const char* file_path){

    ofstream outputFileStream;
    outputFileStream.open(file_path, ios::out);

    if(outputFileStream.is_open()) {
        outputFileStream << "storage:"<<endl;
        Location location;
         for (size_t k = 0; k < currentNumber; k++){
            outputFileStream<<name_of_product << k << " is: " << (*products[k]).getName() << endl;
            outputFileStream<<expiration_date_of_product << k << " is: " << (*products[k]).getExpiration_date() << endl;
            outputFileStream<<date_in_storage_of_product << k << " is: " << (*products[k]).getDate_in_storage() << endl;
            outputFileStream<<name_of_manufactor_of_product << k << " is: " << (*products[k]).getName_of_manufactor() << endl;
            outputFileStream<<unit_of_product << k << " is: " << (*products[k]).getUnit() << endl;
            outputFileStream<<quantity_of_product<< k << " is: " << (*products[k]).getAvailable_quantity() << endl;
            //outputFileStream<<"location of the product in storage " << k << " is: " << location.get_location(k) << endl;
            outputFileStream<<comment_of_product<< k << " is: " << (*products[k]).getComment() << endl;
            outputFileStream<<endl;
            cout<<"product " << k << " " << (*products[k]).getName() << " is successufly added to file" << endl;
         }
         outputFileStream.close();
     }else{
         cout<<open_problem;
     } 
}

/**
 * принтира фаелът в конзолата
 * @param file_path - името на фаелът с които искаме да работим
 */

void Storage::print_file(const char* file_path){

    ifstream inputFileStream;
    inputFileStream.open(file_path, ios::in);

    if(inputFileStream.is_open()) {
        char lineFromFile[1000];
        while(!inputFileStream.eof()) {
            inputFileStream.getline(lineFromFile, 1000);
            cout << lineFromFile << endl;
        }
        inputFileStream.close();
    }else{
        cout<<open_problem;
    }
}

vector<string> split(const string &input){

    vector<string> words; 
    string word = ""; 
    for (auto x : input){ 
        if (x == ' '){ 
            words.push_back(word);
            word = ""; 
        }else{ 
            word = word + x; 
        } 
    }  
    words.push_back(word);
    
    return words;
}