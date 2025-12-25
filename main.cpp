#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include<stdexcept>
#include<map>
#include<filesystem>


using namespace std;
namespace fs = filesystem;
 

const string professors_folder_path = "professors";
const string students_folder_path = "students";

struct Mapping {
    int mapDetails[4];
    vector<string> groupCodes;
};

struct Professor_Timetable {
    vector<string> subjects;
    vector<string> sessions;
    vector<string> rooms;
    vector<Mapping> days[7];
};

struct Insert{

    string subject;
    vector<pair<string, string>> session;  
    string room;
    int day;
    pair<int, int> dur;

};

struct  Delete{
  int day;
  int start_time;
};




struct Student_timetable_mapping{
    int mapDetails[5];
    vector<char> group;
};

struct Student_Timetable{
    vector<string> profs;
    vector<string> subjects;
    vector<string> rooms;
    vector<Student_timetable_mapping> days[7];
};




class Timetable{   //base abstract class

    private: 
    Student_Timetable s;    //not inherited
    Professor_Timetable p;   // not inherited

    public : 

    Student_Timetable& getStudentTimetable(){  //safe parsing to children classes
        return this->s;
    }

    Professor_Timetable& getProfessorTimetable(){
        return this->p;
    }

    virtual void printTimetable() = 0;
};


class StudentTable : public Timetable{
    
    private:
   

   public:
//    Student_Timetable& s = getStudentTimetable();
   string className;
   Student_Timetable s;

   void  printTimetable() override;

};

    map<string, StudentTable> studentTables;
        

void StudentTable::printTimetable() {
    // Access the student timetable
    const auto& s = this->s;

    // Print general info
    std::cout << "Professors:\n";
    for (const auto& prof : s.profs) {
        std::cout << "  " << prof << "\n";
    }

    std::cout << "\nSubjects:\n";
    for (const auto& subj : s.subjects) {
        std::cout << "  " << subj << "\n";
    }

    std::cout << "\nRooms:\n";
    for (const auto& room : s.rooms) {
        std::cout << "  " << room << "\n";
    }

    std::cout << "\n--- Daily Mapping ---\n";

    // Array of day names
    const std::string daysOfWeek[7] = {
        "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"
    };

    for (int day = 0; day < 7; ++day) {
        std::cout << daysOfWeek[day] << ":\n";

        if (s.days[day].empty()) {
            std::cout << "  no entry\n";
        } else {
            int entryNumber = 1;
            for (const auto& entry : s.days[day]) {
                std::cout << "  Entry " << entryNumber << " : ";

                // Print mapDetails array
                for (int i = 0; i < 5; ++i) {
                    std::cout << entry.mapDetails[i];
                    if (i != 4) std::cout << ", ";
                }

                // Print group vector
                std::cout << " , {";
                for (size_t j = 0; j < entry.group.size(); ++j) {
                    std::cout << entry.group[j];
                    if (j != entry.group.size() - 1) std::cout << ", ";
                }
                std::cout << "}\n";

                ++entryNumber;
            }
        }
        std::cout << "\n";
    }
}



class ProfessorTable : public Timetable
{
private:
    Professor_Timetable& t = getProfessorTimetable();   //internal linking (encapsulation)

public:
static vector<string> professors_timeTable_name;
 string professorName;

    ProfessorTable(const string professorFileName, bool firstCreation, Professor_Timetable t = {});

   // void printTimetable(Timetable& t) override;
     void printTimetable() override;

     void deleteEntry(Delete entry);

     void read(const string& fileName);

     void write(const string filename,  Professor_Timetable& t);
   // void deseriallize(const string& fileName);

    bool handle_conflict(int day, pair<int, int> dur);

    static void create_professor(string name){
        
       ofstream out("professors/"+name+".bin");
       cout<<"File created successfully";

      ProfessorTable* newProf = new ProfessorTable(name+".bin", false);
      professors_timeTable_name.push_back(name+".bin"); //track professors record in class's static variable
      delete newProf;

    }

     void insert(Insert entry);

     void deep_clean_and_realign();
};


ProfessorTable::ProfessorTable(const string professorFileLocation, bool firstCreation, Professor_Timetable newT)
{
    //cout<<"file in construtor is " << professorFileLocation<<endl;
        this->professorName = professorFileLocation.substr(professorFileLocation.find_last_of("/\\") + 1);

        //cout<<"professor name in constructor is : "<< this->professorName<<endl;

        if(!firstCreation){
            
            this->read(professorFileLocation);
             vector<char> groups;
                 
             
            for(const string& session : t.sessions){
              //  cout<<"\n\n---------\n-----------\n-------------\n-----------\n"<<session <<"\n"<<studentTables.size()<<"\n------------\n---------------\n"<<endl;
                // auto& s = getStudentTimetable(); // get student timetable from abstract base class
               

                if(studentTables.find(session) == studentTables.end()){

                    //cout<<"making new object in map "<<session<<endl;

                studentTables.emplace(session, StudentTable());
                }
                // else{
                //     cout<<session<<" already exist"<<endl;
                // }
                

                studentTables[session].s.profs.push_back(this->professorName);


                ofstream out(students_folder_path+"/"+session+".bin");

                if(!out.is_open()){
                    throw runtime_error("file opening error for session");
                }
               
               out.close();
            
                 int n = 0;
                for(const auto& mapping : t.days){
                    
                      int itr = 0;
                    for(const auto& slot : mapping){
                      
                        int sessionIndex;
                        

                        for(string groupCode : slot.groupCodes){

                          //  cout<<"Session is : "<< groupCode<<endl;

                        sessionIndex = int(groupCode[0] - '0');

                        //cout<<"Session index is : "<< groupCode[0] - '0' <<" | "<<  groupCode[0]<<endl;

                        if(t.sessions[sessionIndex] == session){ 


                            bool subjectFound = false;
                            groups.push_back(groupCode[2]);

                           // cout<<"group is : "<< groupCode[2]<<endl;

                          for(string subject :studentTables[session].s.subjects){
                            if(subject != t.subjects[slot.mapDetails[0]]){
                                subjectFound = false;
                            }
                            else if(subject == t.subjects[slot.mapDetails[0]]){
                                subjectFound = true;
                            }
                          }

                          if(!subjectFound) studentTables[session].s.subjects.push_back(t.subjects[slot.mapDetails[0]]);

                          subjectFound = false;
                        }
                    }

                         if(itr == slot.groupCodes.size()-1){
                                 studentTables[session].s.days[n].push_back({{static_cast<int>((studentTables[session].s.profs.size()-1)),slot.mapDetails[0], slot.mapDetails[1], slot.mapDetails[2], slot.mapDetails[3]} ,{groups}});
                         groups = {}; //empty gorup vector
                            }
                     itr++;
                }
                 n++; 
            }


    }

    cout<<"\n\n-----PRINTING ALL STUDENT TIMETABLES LINKED TO THIS PROFESSOR-----\n\n"<<endl;
    for(auto [session, object] : studentTables){
           // object.printTimetable();
        }
}
        else if(firstCreation) this->write(professorFileLocation, newT);
      
             
}

void ProfessorTable:: printTimetable(){
     // --- PRINT TO TERMINAL ---
    cout << "===== TIMETABLE =====\n\n";

   auto& readT = this->t;
    cout << "Subjects:\n";
    for (auto &s : readT.subjects) cout << "  - " << s << "\n";

    cout << "\nSessions:\n";
    for (auto &s : readT.sessions) cout << "  - " << s << "\n";

    cout << "\nRooms:\n";
    for (auto &s : readT.rooms) cout << "  - " << s << "\n";

    cout << "\nSchedule by Days:\n";
    for (int day = 0; day < 7; day++) {
        cout << "\nDay " << day << ":\n";
        if (readT.days[day].empty()) {
            cout << "  No entries\n";
            continue;
        }
        for (size_t i = 0; i < readT.days[day].size(); i++) {
            Mapping &m = readT.days[day][i];
            cout << "  Entry " << i + 1 << ":\n";
            cout << "    Map Details: ";
            for (int k = 0; k < 4; k++) {
                cout << m.mapDetails[k];
                if (k != 3) cout << ", ";
            }
            cout << "\n";
            cout << "    Group Codes: ";
            for (size_t j = 0; j < m.groupCodes.size(); j++) {
                cout << m.groupCodes[j];
                if (j + 1 < m.groupCodes.size()) cout << ", ";
            }
            cout << "\n";
        }
    }

    cout << "\n===== END =====\n";
}




void ProfessorTable :: write(const string profFileLocation,  Professor_Timetable& t){
        // --- WRITE TO BINARY FILE ---
ofstream out;
        if(profFileLocation == ""){
            cout<<"file path : "<<profFileLocation<<endl;
    out.open(professors_folder_path + "/" + this->professorName, ios::binary);
        }
        else{
                cout<<"file path : "<<profFileLocation<<endl;
    out.open(profFileLocation, ios::binary);
        }

    if (!out.is_open()) {
        cerr << "File could not be opened for writing.\n";
       
    }

    // Write string vectors
    vector<string> *vecPtrs[] = { &t.subjects, &t.sessions, &t.rooms };
    for (int v = 0; v < 3; v++) {
        size_t vecSize = vecPtrs[v]->size();
        out.write(reinterpret_cast<const char*>(&vecSize), sizeof(vecSize));
        for (size_t i = 0; i < vecPtrs[v]->size(); i++) {
            size_t len = (*vecPtrs[v])[i].size();
            out.write(reinterpret_cast<const char*>(&len), sizeof(len));
            out.write((*vecPtrs[v])[i].data(), len);
        }
    }

    // Write days[7] and their Mappings
    for (int d = 0; d < 7; d++) {
        size_t numMappings = t.days[d].size();
        out.write(reinterpret_cast<const char*>(&numMappings), sizeof(numMappings));
        for (size_t i = 0; i < t.days[d].size(); i++) {
            Mapping &m = t.days[d][i];

            // Write mapDetails array
            out.write(reinterpret_cast<const char*>(m.mapDetails), 4 * sizeof(int));

            // Write groupCodes vector
            size_t numCodes = m.groupCodes.size();
            out.write(reinterpret_cast<const char*>(&numCodes), sizeof(numCodes));
            for (size_t j = 0; j < m.groupCodes.size(); j++) {
                size_t len = m.groupCodes[j].size();
                out.write(reinterpret_cast<const char*>(&len), sizeof(len));
                out.write(m.groupCodes[j].data(), len);
            }
        }
    }
    out.close();

   // PrintTimetable(t);
}


void ProfessorTable :: read(const string& profFileLocation){
    // --- READ FROM BINARY FILE ---

    auto& readT = this->t;

  
    ifstream in(profFileLocation, ios::binary);
    if (!in.is_open()) {
        cerr << "File could not be opened for reading.\n";
    
    }

    // Read string vectors
    vector<string> *readVecPtrs[] = { &readT.subjects, &readT.sessions, &readT.rooms };
    for (int v = 0; v < 3; v++) {
        size_t vecSize;
        in.read(reinterpret_cast<char*>(&vecSize), sizeof(vecSize));
        readVecPtrs[v]->resize(vecSize);
        for (size_t i = 0; i < vecSize; i++) {
            size_t len;
            in.read(reinterpret_cast<char*>(&len), sizeof(len));
            (*readVecPtrs[v])[i].resize(len);
            in.read((*readVecPtrs[v])[i].data(), len);
        }
    }

    // Read days[7] and their Mappings
    for (int d = 0; d < 7; d++) {
        size_t numMappings;
        in.read(reinterpret_cast<char*>(&numMappings), sizeof(numMappings));
        readT.days[d].resize(numMappings);
        for (size_t i = 0; i < numMappings; i++) {
            Mapping &m = readT.days[d][i];

            // mapDetails
            in.read(reinterpret_cast<char*>(m.mapDetails), sizeof(m.mapDetails));

            // groupCodes
            size_t numCodes;
            in.read(reinterpret_cast<char*>(&numCodes), sizeof(numCodes));
            m.groupCodes.resize(numCodes);
            for (size_t j = 0; j < numCodes; j++) {
                size_t len;
                in.read(reinterpret_cast<char*>(&len), sizeof(len));
                m.groupCodes[j].resize(len);
                in.read(m.groupCodes[j].data(), len);
            }
        }
    }

    t = readT;

   this->printTimetable();   //this -> has timetable

    in.close();
}





bool ProfessorTable::handle_conflict(int day, pair<int, int> dur)
{
    cout << "\n\n-------------------entering conflict resolution mode-----------\n\n";
    bool slotFound = false;
    auto t = this->t;

    if(t.days[day].size() == 0){
        slotFound = true;
        return slotFound;
    }

    for (auto mapping : t.days[day])
    {
        cout << mapping.mapDetails[0] << " " << mapping.mapDetails[1] << " " << mapping.mapDetails[2] << " " << mapping.mapDetails[3] << endl;
    }

    for (auto subjectMeta : t.days[day])
    {
        if (
            (dur.first <= subjectMeta.mapDetails[2] && dur.second >= subjectMeta.mapDetails[2] && dur.second <= subjectMeta.mapDetails[3]) ||
            (dur.first <= subjectMeta.mapDetails[2] && dur.second >= subjectMeta.mapDetails[3]) ||
            (dur.first >= subjectMeta.mapDetails[2] && dur.first <= subjectMeta.mapDetails[3] && dur.second >= subjectMeta.mapDetails[3]) ||
            (dur.first >= subjectMeta.mapDetails[2] && dur.second <= subjectMeta.mapDetails[3])
        )
        {
            slotFound = false;
            break;
        }
        else
        {
            slotFound = true;
        }
    }

    return slotFound;
}


void ProfessorTable::insert(Insert entry){

     try
    {
        cout << "\n\n------------------------\n\n";
        int indexOfSubject;
        int indexOfSession;
        int indexOfRoom;

        bool subjectFound = false;
        bool sessionFound = false;
        bool roomFound = false;

        vector<string> sessionCodes;


  for (int i = 0; i < entry.session.size(); i++) {
    bool found = false;
    int indexOfSession = -1;

    // Check if session already exists in t.sessions
    for (int j = 0; j < t.sessions.size(); j++) {
        if (t.sessions[j] == entry.session[i].first) {
            found = true;
            indexOfSession = j;
            break;
        }
    }

    // If session not found, add it
    if (!found) {
        t.sessions.push_back(entry.session[i].first);
        indexOfSession = t.sessions.size() - 1;
    }

    // Add the code with session index
    sessionCodes.push_back(to_string(indexOfSession) + entry.session[i].second);
}


        
        for (int i = 0; i < t.rooms.size(); i++)
        {         
            if (t.rooms[i] == entry.room)
            {
                roomFound = true;
                 indexOfRoom = i;
                break;
            }
        }

        for (int i = 0; i < t.subjects.size(); i++)
        {         
            if (t.subjects[i] == entry.subject)
            {
                subjectFound = true;
                 indexOfSubject= i;
                break;
            }
        }


        if (!roomFound)
        {
            t.rooms.push_back(entry.room);
            indexOfRoom= t.rooms.size()-1;
        }


        if (!subjectFound)
        {
            t.subjects.push_back(entry.subject);
            indexOfSubject = t.subjects.size()-1;
        }

        // if (!sessionFound)
        // {
        //     t.sessions.push_back(entry.session);
        //     indexOfSession = t.sessions.size()-1;
        // }




        bool conflictResolved = handle_conflict(entry.day, entry.dur);

        if (!conflictResolved)
        {
            cout << "Slot already Reserved !!!" << endl;
            return;
        }
        else
        {
            cout << "slot is free !!!" << endl;
        }

        t.days[entry.day].push_back({indexOfSubject , indexOfRoom, entry.dur.first, entry.dur.second, {sessionCodes}});

        this->write(professors_folder_path + "/" + this->professorName, t);
        this->printTimetable();
        return;
    }
    catch (exception &e)
    {
        cerr << "An error occurred: " << e.what() << endl;
        return;
    }
}


void ProfessorTable::deleteEntry(Delete entry) {
    if (entry.day < 0 || entry.day >= 7) return;

    int n = 0;
    for (const auto& mapping : t.days[entry.day]) {
        if (mapping.mapDetails[2] == entry.start_time) {
            t.days[entry.day].erase(t.days[entry.day].begin() + n);
            this->write(professors_folder_path + "/" + this->professorName, t);
            this->printTimetable();
            return;
        }
        n++;
    }
}

vector<string> ProfessorTable :: professors_timeTable_name;  // static variable


int main() {

   
// Professor_Timetable mam_tehmeena = {
//     {"Linear Algebra"},
//     {"24-mct-a"},
//     {"lh#1"},
//     {
//         {},
//         {},
//         {},
//         {{{0,0,510,660}, {"00f"}}},
//         {  },
//         {}, {}
//     }
// };


// ProfessorTable p("professors/mam_tehmeena.bin", true);
// p.write("professors/mam_tehmeena.bin", mam_tehmeena);

//   Write(sir_asif, "professors/sir_asif.bin");
   //Read(r, "professors/sir_asif.bin");

//    insert(r, {"python programming", "24-mct-A", "lh#2",0,{910, 1000}, "00c"});
//    deleteEntry(r, {0, 700}, "professors/sir_asif.bin");



 if (!fs::exists(professors_folder_path) || !fs::is_directory(professors_folder_path)) {
        throw runtime_error("Error: Folder \"professors\" does not exist!");
    }


     char profch;
  
    cout<<"Create new profesor <<(y / n) : ";
    cin>>profch;
    

        if(profch == 'y'){

                 string name;
        cout<<"Enter professor Name ---> ";
        cin>>name;

          ofstream out("professors/" +name+ ".bin");
          ProfessorTable(professors_folder_path + "/" + name + ".bin", true);
   
          if(!out.is_open()){
            throw runtime_error("erro creating new professor file!");
          }
        }


        cout<<"\n\nRunning deep analysis on all professor timetable files in the folder...\n"<<endl;
    for (const auto& entry : fs::directory_iterator(professors_folder_path)) {

        cout<<"\n---------------------------------------------\n--------------------------------------------------\n";
        if (entry.is_regular_file()) {
            //professors.emplace(entry.path().stem().string(), ProfessorTable(entry.path().filename().string()));
            cout<<"object created for :"<< entry.path().stem().string()<<" successfully!"<<endl;
            ProfessorTable* table = new ProfessorTable(professors_folder_path + "/" + entry.path().filename().string(), false);
            ProfessorTable::professors_timeTable_name.push_back(entry.path().filename().string()); //track professors record in class's static variable

           delete table;
    

        }
    }



while(true){

    int n= 1;
    for(const string name : ProfessorTable::professors_timeTable_name){
        cout<<n<<". "<<name<< endl;
        n++;
    }
    
        cout<<"\nSelect Professor Id : "<<endl;
   
    int id;
    cin>>id;

    ProfessorTable professor(professors_folder_path + "/" + ProfessorTable::professors_timeTable_name[id-1], false);
    cout<<"Professor Selected: "<< professor.professorName <<endl;

cout<<"\n\n------------------MENU------------------\n1. Insert Entry\n2. Delete Entry\n3. Hard Diagnostics\n4. Skip\n---------------------------------------\n";
int choice;
cin>>choice;

switch (choice){
case 1 : {

    Insert i;
    // string subject;
    // string session;
    // string room;
    // int day;
    //     pair<int, int> dur;
    // string sessionCode;


   cout<<"In which day you want to reserve slot : ";
   cin>>i.day;
   cout<<endl;

   cout<<"Enter Subject Name : ";
   cin>>i.subject;  
   cout<<endl;

   string sessionName;
   string group;

   while(true){
    cout<<"Enter Session(s) Name | group (0 to proceed): ";
    cin>>sessionName>>group;
   cout<<endl;
    if(sessionName == "0" || group =="0") break;

    i.session.push_back({sessionName, group});
    cout<<endl;
   }


    cout<<"Enter Room Name : ";
    cin>>i.room;    
    cout<<endl;

    cout<<"Enter Start Time (in 24-hr format without colon, e.g., 1300 for 1 PM): ";
    cin>>i.dur.first;
    cin>>i.dur.second;
    cout<<endl;

    
    professor.insert(i);

    break;
}
case 2 : {
    Delete d;

    cout<<"In which day you want to delete entry : ";
    cin>>d.day;
    cout<<endl;

    cout<<"Enter Start Time of the session to delete (in 24-hr format without colon, e.g., 1300 for 1 PM): ";
    cin>>d.start_time;
    cout<<endl;

    professor.deleteEntry(d);

    break;
}

case 3:{

    //continued

    break;
}
case 4:{
cout<<"skipping ... "<<endl;
  
  break;
}

}

}

    return 0;
}
