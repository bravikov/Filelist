/* Build
 * For Linux:
 *     g++ --std='c++11' main.cpp
 * For Windows:
 *      i686-w64-mingw32-g++ --std='c++11' -static main.cpp
 * */

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <errno.h>
#include <cstring>

using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::bad_alloc;

string getWorkingDirectory();

void waitInput();

int main() {
    string workingDirectory = getWorkingDirectory();
    cout << workingDirectory << endl;
    waitInput();
    exit(EXIT_SUCCESS);
}

void waitInput() {
    cout << "Press Enter...";
    std::cin.get();
}

string getWorkingDirectory() {
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

        std::cerr << strerror(errno) << endl << workingDirectory << endl;
        return string();
    }

    string s(workingDirectory);
    delete [] workingDirectory;

    return s;
}
