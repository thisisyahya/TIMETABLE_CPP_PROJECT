//#include "../core-cpp/clock/ParseTime.h"
#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<map>


using namespace std;

enum class DAYS {MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY, SATURDAY, SUNDAY};

struct Lectures{
    string name;
    pair<pair<int, int>, pair<int, int>> dur;
};

// Global map declaration (must be defined once in a .cpp file)
extern std::map<DAYS, std::vector<Lectures>> lectureMap;


int main(){

ofstream outFile("../table.txt");
ifstream inFile("../table.txt");

vector<string> sirNames, days;

map<string, string> subjectMap = {{"edc", "dr. m. asif"},{"ledc", "dr. m. asif / hafiz muhammad saqib"},
{"mmp", "engr. zubair butt"},
{"ldsa", "dr. m. khurram"},
{"dsa", "dr. m. khurram"},
{"ed", "engr. zubair butt"},
{"solidWorks", "engr. shahbaz ahmed"},
{"la", "miss tehmina shafiq"}
};

map<DAYS, vector<Lectures>> subjects;
if(!outFile){
    cout<<"No file found!"<<endl;
}

outFile<<"monday|edc(08:30-11:00)|free(11:00-13:30)|mmp(13:30-15:30)\ntuesday|ldsa(08:30-11:00)|free(11:00-12:40)|ed(12:40-15:10)|free(15:10-15:30)\nwednesday|ledc(08:30-11:00)|free(11:00-12:40)|solidWorks(12:40-15:10)|free(15:10-15:30)\nthursday|la(08:30-11:00)|free(11:00-14:20)|dsa(14:20-15:30)";
outFile.close();


if(!inFile){
 cout<<"File not found!"<<endl;
}

string line;
 int day = 1;

while(getline(inFile, line) && day<=5){
    // cout<<line<<endl;

while(line.length() !=0 ){
    
    
     size_t pos1 = line.find("|");
    if (pos1 == string::npos) break;

    size_t pos2 = line.find("|", pos1 + 1);
    if (pos2 == string::npos) break;

    string subject = line.substr(pos1 + 1, pos2 - (pos1 + 1));
    //cout << "Subject: " << subject << endl;

    size_t posClose = line.find(")", pos2);
    if (posClose == string::npos) break;

    string time = line.substr(pos2 + 1, posClose - (pos2 + 1));
   cout << "Time: " << time << endl << endl;


    string start = time.substr(0, time.find("-"));
    string end = time.substr(time.find("-") + 1);

    cout<<start<<" : "<<end<<endl;
    
    line.erase(0, posClose + 1);

     //if(subject.length()!=0) subjects[static_cast<DAYS>(day-1)].push_back({subject, {parseTime(start), parseTime(end)}});
    // cout<<"line not finisged"<<endl;
}


    day++;
}


inFile.close();


// for(const auto& [day, subj] : subjects){
   
//     for(const auto& s : subj){
//         if(s.name == "free") {
//             cout<<"free for "<<s.dur.second.first - s.dur.first.first<<" hours"<<endl;
//         }
//         else{
//         cout<<s.name<<" by "<<subjectMap[s.name]<<" from "<<s.dur.first.first<<":"<<s.dur.first.second<<" to "<<s.dur.second.first<<":"<<s.dur.second.second<<endl;
//         // string time = s.time;
//         //  auto [hours, minutes] = parseTime(time);
//         }
//     }
//     cout<<endl;
// }

cout<<"\n\n-----------------------------\nEnter the day to view subjects (1-5): ";
int d;
cin >> d;

if(d<1 || d>5){
    cout<<"Invalid day!"<<endl; 
    return -1;
}

vector<Lectures> thatDay = subjects[static_cast<DAYS>(d-1)];
for(const auto& s : thatDay){
    if(s.name == "free") cout<<"No subject scheduled"<<endl;
    else
    cout<<s.name<<" by "<<subjectMap[s.name]<<endl;
}

cout<<"Enter slot to reserve for specific day(1 - 5) : ";
int slot;
cin >> slot >> day;

thatDay = subjects[static_cast<DAYS>(day-1)];
if(thatDay[(slot-1)].name == "free"){
    cout<<"✅ Slot is Free for "<<thatDay[slot-1].dur.second.first - thatDay[slot-1].dur.first.first<<" hours"<<endl;
}
else{
     cout<<"❌ Slot already reserved by "<<subjectMap[thatDay[slot-1].name]<<" for : "<<thatDay[slot-1].name<<" for "<<thatDay[slot-1].dur.second.first - thatDay[slot-1].dur.first.first<<" hours"<<endl;
}
    return 0;
}