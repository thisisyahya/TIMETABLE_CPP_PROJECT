#include "StudentTable.h"
#include "ProfessorTable.h"

#include<map>
#include<filesystem>

namespace fs = filesystem;

map<string, ProfessorTable> professors;
const string folderPath = "professors";

int main()
{

    if (!fs::exists(folderPath) || !fs::is_directory(folderPath)) {
        throw runtime_error("Error: Folder \"professors\" does not exist!");
    }

    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.is_regular_file()) {
            professors.emplace(entry.path().stem().string(), ProfessorTable(entry.path().filename().string()));
            cout<<"object created for :"<< entry.path().stem().string()<<" successfully!"<<endl;
        }
    }

    // for (auto& [professorFileName, professorObject] : professors) {
    //     professorObject.deseriallize(professorFileName);
    // }

    //StudentTable mct_A_24("students/24-mct-A.txt");
    map< string, StudentTable> student_tables;
  //  ProfessorTable p_all = ProfessorTable(); // Also works
    student_tables.emplace("24-mct-A", StudentTable("24-mct-A.txt"));
    auto it = student_tables.find("24-mct-A");
   //it->second.insert("Prof. A", "Math", {1100, 1200}, 0, "2nd-lecture-hall");



    return 0;
}

