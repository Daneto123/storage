#include<iostream>
#include<fstream>
#include<vector>
using namespace std;

class Command{
protected:
    vector<string> file_read;
    int size_of_vector;
    fstream file;

public:

    void open(string file_name);
    void close();
    void save();
    void save_as(string file_name);
    void help();

};