#include "storage.h"

/**
 * пълни данните в файл
 * @param file_path - името на фаелът с които искаме да работим
 */

void Storage::in_to_file(const char* file_path){

    ofstream output_file_stream;
    output_file_stream.open(file_path, ios::out);

    if(output_file_stream.is_open()) {
        output_file_stream << "storage:"<<endl;
        Location locat;
         for (size_t k = 0; k < current_number; k++){
            output_file_stream<<"name of the product " << k << " is: " << (*products[k]).get_name() << endl;
            output_file_stream<<"the expiration date of the product "<< k << " is: " << (*products[k]).get_expiration_date() << endl;
            output_file_stream<<"date in storage of the product " << k << " is: " << (*products[k]).get_date_in_storage() << endl;
            output_file_stream<<"name of manufactor of the product " << k << " is: " << (*products[k]).get_name_of_manufactor() << endl;
            output_file_stream<<"unit of the product " << k << " is: " << (*products[k]).get_unit() << endl;
            output_file_stream<<"available quantity of the product " << k << " is: " << (*products[k]).get_available_quantity() << endl;
            // outputFileStream<<"location of the product in storage " << k << " is: " << locat.locations[k].get_section()<<
            //                                                                            locat.locations[k].get_shelf()<<
            //                                                                            locat.locations[k].get_num()<< endl;
            output_file_stream<<"comment for the product "<< k << " is: " << (*products[k]).get_comment() << endl;
            output_file_stream<<endl;
            cout<<"product " << k << " " << (*products[k]).get_name() << " is successufly added to file" << endl;
         }
         output_file_stream.close();
     }else{
         cout<<"can't open the file\n";
     } 
}

/**
 * принтира фаелът в конзолата
 * @param file_path - името на фаелът с които искаме да работим
 */

void Storage::print_file(const char* file_path){

    ifstream input_file_stream;
    input_file_stream.open(file_path, ios::in);

    if(input_file_stream.is_open()) {
        char line_from_file[1000];
        while(!input_file_stream.eof()) {
            input_file_stream.getline(line_from_file, 1000);
            cout << line_from_file << endl;
        }
        input_file_stream.close();
    }else{
        cout<<"can't open the file\n";
    }
}

string* split(const string &input){
    
    string* words = new string[256];
    int counter;

    string word = "";
    for(auto x : input){
        if(x == ' '){
            words[counter] = word;
            counter++;
            word = "";
        }else{
            word = word + x;
        }
        words[counter] = word;
    }

    return words;

    delete[] words;
}