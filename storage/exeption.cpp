#pragma once
#include <string>
using namespace std;

string open_problem = "can't open the file\n";
string open_successfuly = "successfuly opened\n";
string close_successfuly = "the file was successfuly closed\n";
string save_successfuly = "successfuly saved\n";

string open_command = "open";
string close_command = "close";
string save_command = "save";
string save_as_command = "save_as";
string help_command = "help";
string help_list =   "The following commands are supported:\n"
                "open <file>	    opens <file>\n"
                "close			closes currently opened file\n"
                "save			    saves the currently open file\n"
                "saveas <file>	saves the currently open file in <file>\n"
                "help			    prints this information\n"
                "exit			    exists the program\n";
string exit_command = "exit";

string add_command = "add";
string clear_command = "clear";
string remove_command = "remove";
string show_command = "show"; 
string log_command = "log";  
string print_command = "print";

string valide_date = "please enter the valid date year/month/date/ - ****/**/**\n";
string valide_unit = "it allow only : gr,kg,ml,l,lbs | please enter the valid unit ";
string not_valid_expiration_date = "please enter valid expiration date of product\n";
string clean_odd_products_date = "please enter date to clean odd product ";
string clean_odd_successfuly = "the storage is successfuly cleaned from odd products\n";
string cleaned_location_successfuly = "you successfuly removed location\n";

string name_of_product = "name of the product ";
string expiration_date_of_product = "the expiration date of the product ";
string date_in_storage_of_product = "date in storage of the product ";
string name_of_manufactor_of_product = "name of manufactor of the product ";
string unit_of_product = "unit of the product ";
string quantity_of_product = "available quantity of the product ";
//outputFileStream<<"location of the product in storage ";
string comment_of_product = "comment for the product ";

