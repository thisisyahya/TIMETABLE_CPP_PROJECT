#include<iostream>
#include<string>
#include<vector>
#include<array>
#include<fstream>

using namespace std;

struct timeTable{
    vector<string> profs;
    vector<string> subjects;
    vector<string> rooms;

    vector<array<int, 5>> days[7];
};

int main(){

   ifstream timetable("students/24-mct-A.txt");


   if(!timetable.is_open()){
    cout<<"cant open file!";
   }
   string line;
 
   while(timetable>>line){

    cout<<line<<endl;

   }
   

    return 0;
}