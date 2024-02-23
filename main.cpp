#include <stdio.h>
#include <string>
#include <cstring>
#include <filesystem>
#include <sys/stat.h>
#include <vector>
#include "rainbow.h"

typedef unsigned char byte;

struct PATH {
    std::filesystem::path path;
    byte verify();
    std::filesystem::path current(byte abs = 0) {
        if (paths[i].is_absolute()) return paths[i];
        if (abs) return std::filesystem::absolute(paths[i]);
        return std::filesystem::relative(paths[i]);
    }
};

struct PATHS {
    size_t i = 0;;
    std::vector<PATH> paths;
    void add(std::string path) {paths.push_back(PATH{path});}
    PATH operator ()() {return paths[i];}
    void next() {i++;}
    byte end() {return i >= paths.size();}
};

byte PATHS::verify() {
    if (!std::filesystem::exists(path)) {
        prnt(path+" not found.\n");
        return 0;
    }
    if (!std::filesystem::is_directory(path)) {
        prnt(path+" is not a directory.\n");
        return 0;
    }
    return 1;
}

struct FLAGS {
    byte color = 1;
    byte list = 0;
    byte all = 0;
    byte number = 0;
    byte help = 0;
    PATHS path;
    void parse(int argc, char* argv[]);
    void toggle(std::string arg);
} flags;

void FLAGS::toggle(std::string arg) {
    if (arg == "nocolor" || arg == "c") color = 0;
    else if (arg == "list" || arg == "l") list = 1;
    else if (arg == "all" || arg == "a") all = 1;
    else if (arg == "help" || arg == "h") help = 1;
    else if (arg == "number" || arg == "n") number = 1;
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

rainbow r;

void prnt(std::string str) {
    if (flags.color) {r.print2d(str);r.next();}
    else printf("%s", str.c_str());
}

void runHelp(std::string name) {
    prnt("Usage: "+name+" [OPTIONS] <PATH>\n");
    prnt("List content of PATH in rainbow.\n");
    prnt("Options:\n");
    prnt("   -h, --help\t\tdisplay this help message\n");
    prnt("   -c, --nocolor\t\tdisable color output\n");
    prnt("   -l, --list\tmore info.\n");
    prnt("   -a, --all\tlist all files\n");
    prnt("   -n, --number\tcount files\n");
    exit(0);
}

struct diritem {
    std::string name;
    std::string size;
    std::string str() {return size + name + "\n";}
};

std::string convsize(unsigned long n) {
    if (n == 0) return "  0.00    ";
    double f = n;
    char c[] = " KMGT";
    unsigned long i = 0;
    for (;f > 999; i++) f /= 1000;
    if (i > 4) return " >1.00 P  ";
    n = 100*f;
    std::string s = "";
    for (; n > 0; n /= 10) s = std::to_string(n%10) + s;
    for (; s.length() < 5; s = " " + s);
    return s.substr(0, 3) + "." + s.substr(3, 2) + " " + c[i] + "  ";
}

int getdir(std::vector<diritem>& files, std::vector<diritem>& folders, std::string path) {
    int len = 0, i= 0;
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        diritem item;
        item.name = entry.path().filename().string();
        len += item.name.length();
        i++;
        try {
            if (std::filesystem::is_directory(entry.path())) {
                item.name += "/";
                item.size = "Folder    ";
                folders.push_back(item);
                continue;
            }
            if (std::filesystem::exists(entry.path()) && std::filesystem::is_regular_file(entry.path()))
                item.size = convsize(std::filesystem::file_size(entry.path()));
            else item.size = convsize(0);
        } catch (std::exception& e) {
            item.size = "  0.00    ";
        }
        files.push_back(item);
    }
    len /= i;
    return (5+len)*5;
}

int charcmp(char a, char b) {
    if (a > 96) a-=32;
    if (b > 96) b-=32;
    if (b == a) return 2;
    return a>b;
}

int strcomp(std::string a, std::string b) {
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

void sortdir(std::vector<diritem> list, std::vector<std::string>& newlist){
    std::string comp;
    for (;list.size()!=0;) {
        int ii=0;
        comp = list[0].name;
        for (int i = 1; i < list.size(); i++) {
            if (strcomp(comp, list[i].name)) {
                ii = i;
                comp = list[i].name;
            }
        }
        newlist.push_back(list[ii].str());
        list.erase(list.begin()+ii);
    }
}

int main(int argc, char* argv[]) {
    r.init(30);
    flags.parse(argc, argv);
    if (flags.help) runHelp(argv[0]);
    for (;!flags.path.end();flags.path.next()) {

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