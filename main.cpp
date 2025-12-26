#include <iostream>  
#include <fstream>
#include <string>
#include <vector>
#include<map>        
#include<stdexcept>
#include<algorithm>
#include<filesystem>


using namespace std;
namespace fs = filesystem;



const string professors_folder_path = "professors";
const string students_folder_path = "students";

struct Slot {
   
   int subjectIndex;
    int roomIndex; 

    int startTime;
   int endTime;
    vector<string> groupCodes;
};

struct Professor_Timetable {
    vector<string> subjects;
    vector<string> sessions;
    vector<string> rooms;
    vector<Slot> days[7];
  
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


    public : 

    virtual void printTimetable() = 0;
};


class ProfessorTable : public Timetable
{
private:
    Professor_Timetable t;

public:
static vector<string> professors_timeTable_name;
 string professorName;

    ProfessorTable(const string professorFileName, bool firstCreation);

   // void printTimetable(Timetable& t) override;
     void printTimetable() override;

     void printLowLevelTimetable();

     void deleteEntry(Delete entry);

     void load_binary_file_into_memory(const string& fileName);

     void write_timetable_into_binary_file(const string filename);
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

    //  void deep_clean_and_realign();   --> removedn now
};


ProfessorTable::ProfessorTable(const string professorFileLocation, bool firstCreation)
{
   
        this->professorName = professorFileLocation.substr(professorFileLocation.find_last_of("/\\") + 1);


        if(!firstCreation){  
         
            cout<<"timetable exists already for : "<< this->professorName <<endl;
                 
        }

        else if(firstCreation){

            this->write_timetable_into_binary_file(professorFileLocation);   //creating professor timetbale file wiht proper binary configuraitons ( empty file)

        }
      
             
}


void ProfessorTable::printTimetable() {
    cout << "\n===== PROFESSOR TIMETABLE =====\n";

    const string daysOfWeek[7] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};

    for (int day = 0; day < 7; ++day) {
        cout << "\n" << daysOfWeek[day] << ":\n";

        if (t.days[day].empty()) {
            cout << "  No entries\n";
            continue;
        }

        // Group slots by subject
        map<int, vector<Slot>> subjectSlots;
        for (const auto& slot : t.days[day]) {
            subjectSlots[slot.subjectIndex].push_back(slot);
        }

        for (const auto& subjPair : subjectSlots) {
            int subjIndex = subjPair.first;
            string subjectName = (subjIndex >= 0 && subjIndex < t.subjects.size()) ? t.subjects[subjIndex] : "Unknown Subject";
            cout << "\nSubject: " << subjectName << "\n";

            // For each slot of this subject
            for (const auto& slot : subjPair.second) {
                // Room
                string roomName = (slot.roomIndex >= 0 && slot.roomIndex < t.rooms.size()) ? t.rooms[slot.roomIndex] : "Unknown Room";
                cout << "  Room: " << roomName << "\n";

                // Time in HH:MM format
                int startHour = slot.startTime / 100;
                int startMin  = slot.startTime % 100;
                int endHour   = slot.endTime / 100;
                int endMin    = slot.endTime % 100;
                printf("  Time: %02d:%02d - %02d:%02d\n", startHour, startMin, endHour, endMin);

                // Sessions & Groups
                cout << "  Sessions & Groups:\n";
                for (const auto& code : slot.groupCodes) {
                    string sessionName = "Unknown";
                    string groupStr;

                    // Extract session index
                    if (!code.empty() && isdigit(code[0])) {
                        int sessionIdx = code[0] - '0'; // assuming single-digit index
                        if (sessionIdx >= 0 && sessionIdx < t.sessions.size()) sessionName = t.sessions[sessionIdx];
                    }

                    // Group
                    if (code.find('f') != string::npos) groupStr = "Full Class";
                    else groupStr = "Group " + string(1, code.back());

                    cout << "       " << sessionName << " (" << groupStr << ")\n";
                }
            }
        }
    }

    cout << "\n===== END OF TIMETABLE =====\n";
}

void ProfessorTable::printLowLevelTimetable() {
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
            Slot &m = readT.days[day][i];
            cout << "  Entry " << i + 1 << ":\n";

            cout << "    Subject Index: " << m.subjectIndex << "\n";
            cout << "    Room Index: " << m.roomIndex << "\n";
            cout << "    Start Time: " << m.startTime << "\n";
            cout << "    End Time: " << m.endTime << "\n";

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




void ProfessorTable :: write_timetable_into_binary_file(const string profFileLocation){
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

    
    // Write Slot members in correct order
for (int d = 0; d < 7; d++) {
    size_t numSlots = t.days[d].size();
    out.write(reinterpret_cast<const char*>(&numSlots), sizeof(numSlots));
    for (size_t i = 0; i < t.days[d].size(); i++) {
        Slot &m = t.days[d][i];

         // Correct order: subject → room → start → end
            out.write(reinterpret_cast<const char*>(&m.subjectIndex), sizeof(m.subjectIndex));
            out.write(reinterpret_cast<const char*>(&m.roomIndex), sizeof(m.roomIndex));
            out.write(reinterpret_cast<const char*>(&m.startTime), sizeof(m.startTime));
            out.write(reinterpret_cast<const char*>(&m.endTime), sizeof(m.endTime));

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


void ProfessorTable :: load_binary_file_into_memory(const string& profFileLocation){
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

    // Read days[7] and their Slots
for (int d = 0; d < 7; d++) {
    size_t numSlots;
    in.read(reinterpret_cast<char*>(&numSlots), sizeof(numSlots));
    readT.days[d].resize(numSlots);
    for (size_t i = 0; i < numSlots; i++) {
        Slot &m = readT.days[d][i];

        // Read individual Slot members
        in.read(reinterpret_cast<char*>(&m.subjectIndex), sizeof(m.subjectIndex));
        in.read(reinterpret_cast<char*>(&m.roomIndex), sizeof(m.roomIndex));
        in.read(reinterpret_cast<char*>(&m.startTime), sizeof(m.startTime));
        in.read(reinterpret_cast<char*>(&m.endTime), sizeof(m.endTime));

        // Read groupCodes vector
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
        cout << mapping.subjectIndex << " " << mapping.roomIndex << " " << mapping.startTime << " " << mapping.endTime << endl;
    }

    for (auto subjectMeta : t.days[day])
    {
        if (
            (dur.first <= subjectMeta.startTime && dur.second >= subjectMeta.startTime && dur.second <= subjectMeta.endTime) ||
            (dur.first <= subjectMeta.startTime && dur.second >= subjectMeta.endTime) ||
            (dur.first >= subjectMeta.startTime && dur.first <= subjectMeta.endTime && dur.second >= subjectMeta.endTime) ||
            (dur.first >= subjectMeta.startTime && dur.second <= subjectMeta.endTime)
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

        this->write_timetable_into_binary_file(professors_folder_path + "/" + this->professorName);
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
        if (mapping.startTime == entry.start_time) {
            t.days[entry.day].erase(t.days[entry.day].begin() + n);
            this->write_timetable_into_binary_file(professors_folder_path + "/" + this->professorName);
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


    for (const auto& entry : fs::directory_iterator(professors_folder_path)) {

        if (entry.is_regular_file()) {
    
            ProfessorTable::professors_timeTable_name.push_back(entry.path().filename().string()); //track professors record in class's static variable
    

        }
    }



while(true){


    cout<<"\n\n-----PROFESSOR SELECTION MENU (1)-----\n-----CREATE A NEW PROFESSOR (2)-----\n"<<endl;

    int menu_choice;
    cin>>menu_choice;

    if(menu_choice == 2){
        string prof_name;
        cout<<"Enter Professor Name (without spaces): ";
        cin>>prof_name;

        ProfessorTable(professors_folder_path + "/" + prof_name + ".bin", true);
        cout<<"Professor "<< prof_name <<" created successfully!"<<endl;

       
        bool professorFound = false;

for (const string& prof : ProfessorTable::professors_timeTable_name) {
    if (prof == prof_name + ".bin") {
        professorFound = true;
        break;
    }
}

if (!professorFound) {
    ProfessorTable::professors_timeTable_name.push_back(prof_name + ".bin");
}


  
    continue;
    }

    cout<<"\nList of Professors Available: \n"<<endl;
    int n= 1;
    for(const string name : ProfessorTable::professors_timeTable_name){
        cout<<n<<". "<<name<< endl;
        n++;
    }
    
        cout<<"\nSelect Professor Id : "<<endl;
   
    int id;
    cin>>id;

    ProfessorTable* professor = new ProfessorTable(professors_folder_path + "/" + ProfessorTable::professors_timeTable_name[id-1], false);
    professor->load_binary_file_into_memory(professors_folder_path + "/" + ProfessorTable::professors_timeTable_name[id-1]);
    cout<<"Professor Selected: "<< professor->professorName <<endl;

cout<<"\n\n------------------MENU------------------\n1. Insert Entry\n2. Delete Entry\n3. Print Timetable \n4. Skip (exit the professor)\n---------------------------------------\n";
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

    
    professor->insert(i);

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

    professor->deleteEntry(d);

    break;
}

case 3:{

    professor->printTimetable();

    break;
}
case 4:{
cout<<"skipping ... "<<endl;
  delete professor;
  break;
}

case 0:{
    professor->printLowLevelTimetable();   //low level debugging
    break;
}
default : {
    cout<<"Invalid choice!!!";
}

}

}

    return 0;
}
