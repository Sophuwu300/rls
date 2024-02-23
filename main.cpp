#include <stdio.h>
#include <string>
#include <cstring>
#include <filesystem>
#include <sys/stat.h>
#include <vector>
#include "rainbow.h"

typedef unsigned char byte;

byte COLOR = 1;

void prnt(rainbow&r, std::string str) {
    if (COLOR) {r.print2d(str);r.next();}
    else printf("%s", str.c_str());
}

void help(std::string name) {
    rainbow r;
    r.init(30);
    prnt(r, "Usage: "+name+" [OPTIONS] <PATH>\n");
    prnt(r, "List content of PATH in rainbow.\n\n");
    prnt(r, "Options:\n");
    prnt(r, "   -h, --help\t\tDisplay this help message.\n");
    prnt(r, "   -i, --info\t\tDisplay the number of items and absolute path.\n");
    prnt(r, "   -f, --format=string\tFormat the output. Default: \"%s %n\"\n");

    prnt(r, "Format string:\n");
    prnt(r, "   %n   name\n");
    prnt(r, "   %N   full name\n");
    prnt(r, "   %s   size\n");
    prnt(r, "   %S   size in bytes\n");
    prnt(r, "   %P   permission bits\n");
    prnt(r, "   %p   permission string\n");
    prnt(r, "   %o   owner name\n");
    prnt(r, "   %O   owner number\n");
    prnt(r, "   %g   group name\n");
    prnt(r, "   %G   group number\n");
    prnt(r, "   %t   mod time\n");
    prnt(r, "   %T   unix mod time\n");
}

std::string getpath(int argc, char* argv[]) {
    std::string path = std::filesystem::current_path();
    if (argc > 1) {
        if (argv[1][0] == '/') path = std::string(argv[1]);
        else path += "/" + std::string(argv[1]);
    }
    return path;
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
    help(std::string(argv[0]));
    return 0;
    rainbow r;
    r.init(30);

    std::string path = getpath(argc, argv);

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
}