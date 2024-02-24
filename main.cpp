#include <stdio.h>
#include <string>
#include <cstring>
#include <filesystem>
#include <sys/stat.h>
#include <vector>
#include "rainbow.h"

typedef std::filesystem::path PATH;
typedef std::filesystem::directory_entry DIRENT;
typedef std::vector<DIRENT> DIRENTS;

rainbow r;
void prnt(std::string str);
struct FLAGS;
extern FLAGS flags;

struct ENTLIST {
    DIRENTS list;
    void printall() {
        std::string printer;
        for (int i = 0; i < list.size(); i++) {
            printer = "\t";
            printer += list[i].path().filename().string();

        }
    }
};

struct DIRLIST {
    DIRENTS files;
    DIRENTS dirs;
    void sorter(DIRENTS& list, DIRENTS& newlist);
    ENTLIST all() {
        DIRENTS all;
        sorter(dirs, all);
        sorter(files, all);
        return ENTLIST{all};
    };
    int charcmp(char a, char b);
    int strcomp(std::string a, std::string b);
};

int DIRLIST::charcmp(char a, char b) {
    if (a > 96) a-=32;
    if (b > 96) b-=32;
    if (b == a) return 2;
    return a>b;
}

int DIRLIST::strcomp(std::string a, std::string b) {
    int len = a.length();
    if (b.length()<len) len=b.length();
    char c[len];
    c[0] = 0;
    strncat(c, a.c_str(),len);
    char d[len];
    d[0] = 0;
    strncat(d,b.c_str(),len);
    int cmp;
    for (int i = 0; i < len; i++) {
        cmp = charcmp(c[i], d[i]);
        if (cmp != 2) return cmp;
    }
    return b.length()<a.length();
}

void DIRLIST::sorter(DIRENTS& list, DIRENTS& newlist) {
    std::string comp;
    for (;list.size()!=0;) {
        int ii=0;
        comp = list[0].path().filename().string();
        for (int i = 1; i < list.size(); i++) {
            if (strcomp(comp, list[i].path().filename().string())) {
                ii = i;
                comp = list[i].path().filename().string();
            }
        }
        newlist.push_back(list[ii]);
        list.erase(list.begin()+ii);
    }
}

struct PATHS {
    size_t i = 0;;
    std::vector<PATH> paths;
    void add(std::string str) {paths.push_back(std::filesystem::path(str));}
    PATH path() const {return paths[i];}
    PATH operator ()() {return path();}
    void next() {i++;}
    bool end() {return i >= paths.size();}
    bool isValid() const {return std::filesystem::exists(path()) && std::filesystem::is_directory(path());};
    PATH abs() const {return std::filesystem::absolute(path());}
    DIRLIST read();
};

DIRLIST PATHS::read() {
    DIRLIST list;
    for (const auto& entry : std::filesystem::directory_iterator(path())) {
        if (std::filesystem::is_directory(entry)) list.dirs.push_back(entry);
        else list.files.push_back(entry);
    }
    return list;
}

struct FLAGS {
    bool color = true;
    bool list = false;
    bool all = true;
    bool number = false;
    bool help = false;
    PATHS path;
    void parse(int argc, char* argv[]);
    void toggle(std::string arg);
};

void FLAGS::toggle(std::string arg) {
    if (arg == "nocolor" || arg == "c") color = false;
    else if (arg == "list" || arg == "l") list = true;
    else if (arg == "all" || arg == "a") all = true;
    else if (arg == "help" || arg == "h") help = true;
    else if (arg == "number" || arg == "n") number = true;
    else printf("Unknown flag: %s\n", arg.c_str());
}

void FLAGS::parse(int argc, char* argv[]) {
    std::string arg;
    size_t len;
    int j;
    for (int i = 1; i < argc; i++) {
        arg = std::string(argv[i]);
        len = arg.length();
        if (len >= 2 && arg.substr(0,2) == "--") toggle(arg.substr(2));
        else if (len >= 1 && arg[0] == '-') for (j = 1; j < len; j++) toggle(arg.substr(j,1));
        else path.add(arg);
    }
}

void prnt(std::string str) {
    if (flags.color) {r.print2d(str+"\n");r.next();}
    else printf("%s\n", str.c_str());
}

void runHelp(std::string name) {
    prnt("Usage: "+name+" [OPTIONS] <PATH>");
    prnt("List content of PATH in rainbow.");
    prnt("Options:");
    prnt("   -h, --help\t\tdisplay this help message");
    prnt("   -c, --nocolor\t\tdisable color output");
    prnt("   -l, --list\tmore info.");
    prnt("   -a, --all\tlist all files");
    prnt("   -n, --number\tcount files");
    exit(0);
}

std::string convsize(unsigned long n) {
    if (n == 0) return "  0.00    ";
    double f = n;
    char c[] = " KMGT";
    unsigned long i = 0;
    for (; f > 999; i++) f /= 1000;
    if (i > 4) return " >1.00 P  ";
    n = 100 * f;
    std::string s = "";
    for (; n > 0; n /= 10) s = std::to_string(n % 10) + s;
    for (; s.length() < 5; s = " " + s);
    return s.substr(0, 3) + "." + s.substr(3, 2) + " " + c[i] + "  ";
}

void br(){printf("\n");}

int main(int argc, char* argv[]) {
    r.init(30);
    flags.parse(argc, argv);
    if (flags.help) runHelp(argv[0]);
    for (;!flags.path.end();flags.path.next()) {
        if (!flags.path.isValid()) {
            prnt(flags.path.path().string()+" not valid");
            br();
            continue;
        }
        prnt(flags.path.path().string());
        ENTLIST list = flags.path.read().all();
        list.printall();
        br();
    }
    return 0;
/*
    if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path)) {
        r.print2d(path+"\n");
        r.next();
        r.next();
        r.print2d(" Directory not found.\n");
        return 1;
    }

    std::vector<diritem> files;
    std::vector<diritem> folders;
    int len = getdir(files, folders, path);
    std::vector<std::string> dirlist;
    sortdir(folders, dirlist);
    sortdir(files,dirlist);

    if (dirlist.size()<30) r.init(dirlist.size());

    r.print2d(path+"\n", len);
    r.next();

    for (int i = 0; i < dirlist.size(); i++) {
        r.print2d("   "+dirlist[i], len);
        r.next();
    }
    r.print2d("Folders / Files / Total: " + std::to_string(folders.size()) + '/' + std::to_string(files.size()) + '/' + std::to_string(folders.size()+files.size()) + '\n', len);
    printf("\033[0m");
    return 0;
*/
}