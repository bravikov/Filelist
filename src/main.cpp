const char * programVersion = "1.1.0";

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
#include <sys/stat.h>

using std::cerr;
using std::cout;
using std::clog;
using std::endl;
using std::string;
using std::bad_alloc;
using std::list;
using std::find;
using std::fstream;
using std::getline;

enum Action {
    ACTION_NOTHING,
    ACTION_CREATE_FILE_LIST,
    ACTION_CREATE_FOLDERS_FROM_LIST,
};

bool createDir(const string & dirName);
string getWorkingDirectoryName();
bool createFoldersFromList(const string & fileName);
bool createFileList();
void waitInput();
void exitFailure();

int main(int argc, char * argv[]) {
    cerr << "Filelist version " << programVersion << "." << endl;

    Action action = ACTION_NOTHING;

    string fileName;

    if (1 == argc) {
        action = ACTION_CREATE_FILE_LIST;
    }
    else if (2 == argc) {
        fileName = argv[1];
        action = ACTION_CREATE_FOLDERS_FROM_LIST;
    }
    else {
        cerr << "Bad arugments." << endl;
    }

    bool ok = false;

    switch (action) {
        default:
            break;
        case ACTION_NOTHING:
            break;
        case ACTION_CREATE_FILE_LIST:
            ok = createFileList();
            break;
        case ACTION_CREATE_FOLDERS_FROM_LIST:
            ok = createFoldersFromList(fileName);
            break;
        
    }

    if (!ok) {
        exitFailure();
    }

    exit(EXIT_SUCCESS);
}

bool createDir(const string & dirName) {
    int r = -1;

    #if defined(os_linux)
        r = mkdir(dirName.c_str(), 0775); // rwxrwxr-x
    #elif defined(os_windows)
        r = mkdir(dirName.c_str());
    #else
        #error "Unknown operating system."
    #endif

    if (0 == r) {
        return true;
    }

    cerr << strerror(errno) << endl;

    return false;
}

bool createFoldersFromList(const string & fileName) {
    clog << "File: " << fileName << endl;

    fstream folderList (fileName, fstream::in);

    if (folderList.fail()) {
        cerr << "File cannot be opened." << endl;
        folderList.close();
        return false;
    }

    list<string> dirNames;

    while (!folderList.eof()) {
        string line;

        getline(folderList, line);

        if (line.empty()) {
            continue;
        }

        dirNames.push_back(line);
    }

    folderList.close();

    dirNames.unique();

    for (auto & dirName: dirNames) {
        clog << "Create folder: " << dirName << endl;
        createDir(dirName);
    }

    return true;
}

bool createFileList() {
    string workingDirectoryName = getWorkingDirectoryName();

    if (workingDirectoryName.empty()) {
        std::cerr << "Unknown working directory." << endl;
        return false;
    }
    
    DIR * workingDirectory = opendir(
        workingDirectoryName.c_str()
    );

    if (workingDirectory == nullptr) {
        std::cerr << strerror(errno) << endl;
        return false;
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

    fstream fs ("filelist.txt", fstream::out);

    for (auto & fn: fileNames) {
        cout << fn << endl;
        fs << fn << endl;
    }

    fs.close();

    return true;
}

void waitInput() {
    cout << "Press Enter...";
    std::cin.get();
}

/* Завершает программу при неудачном выполнении, дает пользователю
 * прочитать сообщение об ошибке. */
void exitFailure() {
    waitInput();
    exit(EXIT_FAILURE);
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

        cerr << strerror(errno) << endl << workingDirectory << endl;
        return string();
    }

    string s(workingDirectory);
    delete [] workingDirectory;

    return s;
}
