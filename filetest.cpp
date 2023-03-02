// basic file operations
#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
using namespace std;

string dirname;

void createdir(){
    const int dir_err = mkdir(dirname.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (-1 == dir_err)
    {
        printf("Error creating directory!n");
        exit(1);
    }
}

int main () {
    dirname = "algorithmdata/" + to_string((unsigned) time(NULL));
    createdir();
    ofstream myfile;
    myfile.open (dirname + "/example.txt");
    myfile << "Writing this to a file.\n";
    myfile.close();
    return 0;
}