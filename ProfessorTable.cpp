#include "ProfessorTable.h"
#include<stdexcept>
#include <filesystem>
namespace fs = filesystem;
vector<string> ProfessorTable :: professors_timeTable_name;  // static variable

ProfessorTable::ProfessorTable()
{
    
cout<<"contructor of professors ran";
    if (!fs::exists(folderPath) || !fs::is_directory(folderPath)) {
        throw runtime_error("Error: Folder \"professors\" does not exist!");
    }

    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.is_regular_file()) {
            professors_timeTable_name.push_back(entry.path().filename().string());
        }
    }

    if (professors_timeTable_name.empty()) {
        throw runtime_error("Error: Folder is empty!");
    }


    for (const auto& fileName : professors_timeTable_name) {
        ProfessorTable::deseriallize(fileName);
    }

    // You can now use fstream on each file:
    // ifstream file(folderPath + "/" + name);

}

void ProfessorTable::deseriallize(const string& fileName)
{

        cout <<"Deseriallizing " << fileName << "Timetable in memory ..."<<endl;

    ifstream currentInputFile("professors/"+fileName);

    if(!currentInputFile.is_open()){
        cout<<"timetable for : "<<fileName<<" doesnot exist ... exiting"<<endl;
    }

    cout<<"file opened : "<<fileName<<" ... "<<endl;
    
    string line;
    while (getline(currentInputFile, line))
    {
    /*  
    DEBUGGING:
       temp.linesCount++;
       cout << "Line read: \"" << line << "\"" << endl;
    */

        if (line == "")    //safe fallback for ignoring empty lines
            continue;

        if (!temp.hasSubjects)
        {
          //  cout << "making subjects array!!!" << endl;
            if (line == "\\0-0\\")
            {
                temp.hasSubjects = true;
                continue;
            }
            t.subjects.push_back(line);
            continue;
        }

        if (!temp.hasSessions)
        {
          //  cout << "making professors array!!!" << endl;
            if (line == "\\0-0\\")
            {
                temp.hasSessions = true;
                continue;
            }
            t.sessions.push_back(line);


             try {
        // Ensure directory exists
        fs::create_directories("students");

        cout << "Opening / creating " << line << " session timetable (file)" << endl;

        ofstream currentOutputFile("students/" + line);
        if (!currentOutputFile.is_open()) {
            throw runtime_error("Failed to create file");
        }

        // Close immediately if not writing yet
        currentOutputFile.close();
    } 
    catch (const exception &e) {
        cerr << "Error occurred: " << e.what() << endl;
    }

            continue;
        }

        if(!temp.hasRooms){
           // cout << "making Rooms Array!!!" << endl;
            if (line == "\\0-0\\")
            {
                temp.hasRooms = true;
                continue;
            }
            t.rooms.push_back(line);
            continue;
        }

       
        
//         if(temp.day<7 && line=="\\0-0\\"){

//            for(int i = 0; i<temp.meta.size()/5; i++){
//           //  cout<<"slots in day is "<<temp.meta.size()/5<<endl;
//               t.day[temp.day].push_back({
//     temp.meta[i*5],
//     temp.meta[i*5 + 1],
//     temp.meta[i*5 + 2],
//     temp.meta[i*5 + 3],
//     temp.meta[i*5 + 4]
// });

//            }
//        temp.meta.clear();
//        temp.day++;
//         }else{
//             int num = stoi(line);
//              temp.meta.push_back(num);
//             // cout<<"pushing : "<<line<<" in mapping"<<endl;
//         }
    }

}