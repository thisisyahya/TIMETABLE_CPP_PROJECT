#include "ProfessorTable.h"
#include<stdexcept>
#include <filesystem>

vector<string> ProfessorTable :: professors_timeTable_name;  // static variable

ProfessorTable::ProfessorTable(const string professorFileName)
{
        this->professorName = professorFileName;
        professors_timeTable_name.push_back(professorFileName); //track professors record in class's static variable
        ProfessorTable::deseriallize(professorFileName);

}

void ProfessorTable::deseriallize(const string& fileName)
{

        cout <<"\n----------------------------------\nDeseriallizing " << fileName << "Timetable in memory ..."<<endl;

    ifstream currentProfessorFile("professors/"+fileName);

    if(!currentProfessorFile.is_open()){
        cout<<"timetable for : "<<fileName<<" doesnot exist ... exiting"<<endl;
    }

    cout<<"file opened : "<<fileName<<" ... "<<endl;
    
    string line;
    while (getline(currentProfessorFile, line))
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

        cout << "Opening / creating " << line << " session timetable (file)" << endl;

        ofstream currentSessionFile("students/" + line);  //file in output mode
       
        if (!currentSessionFile.is_open()) {
            throw runtime_error("Failed to create file");
        }

        // Close immediately if not writing yet
        currentSessionFile.close();
   
            continue;   //continue to skip the current iteration
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

       
        
        if(temp.day<7 && line=="\\0-0\\"){

            if(temp.counter>0){
                          t.day[temp.day>0 ? temp.day-1 : temp.day].push_back({temp.details ,temp.sessionCodes}); //push only when the slot ends
                  cout<<"\nThe day is "<<temp.day<<endl;
                 temp.details = {};
                 temp.sessionCodes.clear();
                 temp.counter = 0;            
            }

           temp.day++;  //just increment day on day end
             
           continue;

        }else{
    
            if(temp.counter < 4){
                   int num = stoi(line);
          
                  temp.details[temp.counter] = num;  //capture only first 4 nums
                  cout<<"\ncounter : "<<temp.counter <<" ,value : ("<<temp.details[temp.counter]<<")"<< " | "<<num<<endl;
                  temp.counter++;  //increment counter to push to another array index in next run
                  continue;   //continue after grabbing the integer detail
            }



            if(line != "-"){
               temp.sessionCodes.push_back(line);  //get the session code till line becomes "-"
               cout<<"session Code(s) : "<<temp.sessionCodes.front()<<" | "<<line<<endl;
               continue;   //dont reach next code if line != "-"
            } 
            
            if(line == "-"){
                 t.day[temp.day>0 ? temp.day-1 : temp.day].push_back({temp.details ,temp.sessionCodes}); //push only when the slot ends
                  cout<<"\nThe day is "<<temp.day<<endl;
                 temp.details = {};
                 temp.sessionCodes.clear();
                 temp.counter = 0;

                 continue;         
            }
        }

        
    }





     temp.subjects.clear();
    temp.rooms.clear();
    temp.sessions.clear();

    temp.hasSessions = false;
    temp.hasSubjects = false;
    temp.hasRooms = false;

    temp.day = 0;

    temp.details.fill(0);
    temp.sessionCodes.clear();

    temp.counter = 0;
        
   


cout<<"\n";
     for (const auto& s : t.subjects)
        cout << s << '\n';

    cout << "\\0-0\\\n";

    for (const auto& s : t.sessions)
        cout << s << '\n';

    cout << "\\0-0\\\n";

    for (const auto& r : t.rooms)
        cout << r << '\n';

    cout << "\\0-0\\\n";

    for (int d = 0; d < 7; ++d)
    {
        cout << "\\0-0\\\n"; // start of day

        const auto& mappings = t.day[d];

        for (size_t i = 0; i < mappings.size(); ++i)
        {
            const Mapping& m = mappings[i];

            for (int v : m.details)
                cout << v << '\n';

            for (const auto& code : m.sessionsCode)
                cout << code << '\n';

            // print '-' only between slots, not after the last one
            if (i + 1 < mappings.size())
                cout << "-\n";
        }
    }


    
   
}



 

