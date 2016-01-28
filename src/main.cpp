/* Build
 * For Linux:
 *     g++ --std='c++11' main.cpp
 * For Windows:
 *     i686-w64-mingw32-g++ --std='c++11' -static main.cpp
 * */

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <errno.h>
#include <cstring>
#include <dirent.h>
#include <list>
#include <algorithm>
#include <fstream>

using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::bad_alloc;
using std::list;
using std::find;
using std::fstream;

string getWorkingDirectoryName();
void waitInput();

int main() {
    string workingDirectoryName = getWorkingDirectoryName();

    if (workingDirectoryName.empty()) {
        std::cerr << "Unknown working directory." << endl;
        waitInput();
        exit(EXIT_FAILURE);
    }
    
    DIR * workingDirectory = opendir(
        workingDirectoryName.c_str()
    );

    if (workingDirectory == nullptr) {
        std::cerr << strerror(errno) << endl;
        waitInput();
        exit(EXIT_FAILURE);
    }

    list<string> fileNames;

    struct dirent * entry = readdir(workingDirectory);
    while (entry != NULL) {
        fileNames.push_back(entry->d_name);
        entry = readdir(workingDirectory);
    };
    closedir(workingDirectory);

    /* Удалить каталоги "." и "..". */
    fileNames.erase(find(fileNames.begin(), fileNames.end(), "."));
    fileNames.erase(find(fileNames.begin(), fileNames.end(), ".."));

    fileNames.sort();

    /* Сохранить список в файл. */

    std::fstream fs ("filelist.txt", fstream::out);

    for (auto & fn: fileNames) {
        cout << fn << endl;
        fs << fn << endl;
    }

    fs.close();
    
    exit(EXIT_SUCCESS);
}

void waitInput() {
    cout << "Press Enter...";
    std::cin.get();
}

string getWorkingDirectoryName() {
    const size_t baseSize = 100;
    size_t size = 0;
    char * workingDirectory = nullptr;

    while(true) {
        size += baseSize;

        try {
            delete [] workingDirectory;
            workingDirectory = new char [size];
        }
        catch(bad_alloc & ba) {
            cerr << "bad_alloc caught: " << ba.what() << endl;
            return string();
        }
        catch(...) {
            cerr << "Unhandled exception!" << endl;
            return string();
        }

        const char * t = getcwd(workingDirectory, size);

        if (t == workingDirectory) {
            break;
        }

        if (t == nullptr && errno == ERANGE) {
            continue;
        }

        std::cerr
            << strerror(errno) << endl
            << workingDirectory << endl;
        return string();
    }

    string s(workingDirectory);
    delete [] workingDirectory;

    return s;
}
