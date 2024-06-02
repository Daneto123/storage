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

    void openFile(const string &file_name);
    void closeFile();
    void helpInterface();

};