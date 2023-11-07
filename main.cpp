#include <stdio.h>
#include <string>
#include <cstring>
#include <filesystem>
#include <sys/stat.h>
#include <vector>

void panic(std::string s) {
    fprintf(stderr, "Fatal error: %s\n", s.c_str());
    exit(1);
}

struct rgb {
    double r, g, b;

    void clamp() {
        if (r > 255) r = 255; else if (r < 0) r = 0;
        if (g > 255) g = 255; else if (g < 0) g = 0;
        if (b > 255) b = 255; else if (b < 0) b = 0;
    }

    std::string str(char sep = ';') {
        clamp();
        return std::to_string((int)r) + sep + std::to_string((int)g) + sep + std::to_string((int)b);
    }

    void set(double rr, double gg, double bb) {r = rr;g = gg;b = bb; clamp();}
    void set(rgb c) {r = c.r;g = c.g;b = c.b; clamp();}

    void print(std::string s) {printf("\033[38;2;%sm%s\033[0m", str().c_str(), s.c_str());}
    void printAt(std::string s, int x, int y) {printf("\033[%d;%dH\033[38;2;%sm%s\033[0m", y, x, str().c_str(), s.c_str());}
    void setPx(int x, int y) {printf("\033[%d;%dH\033[48;2;%sm \033[0m", y, x, str().c_str());}

    double operator[](int i) {
        switch (i%3) {
            case 0: return r;
            case 1: return g;
            case 2: return b;
        }
        return 0;
    }
    void indexAdd(int i, double v) {
        switch (i%3) {
            case 0: r += v; break;
            case 1: g += v; break;
            case 2: b += v; break;
        }
        clamp();
    }
};

struct rainbow {
    rgb c;
    double s = 0; // step

    void init(unsigned int len) {
        if (len == 0) panic("Divison by zero.");
        c.set(255, 0, 0);
        s = 5.0*255.0 / (double)len;
    }

    void next() {
        if (s == 0) panic("Empty infinity loop. Rainbow not initialized.");
        for (int i = 0; i < 3; i++) {
            if ((c[i+1]==0||c[i+1]==255)&&(c[i+2]==0||c[i+2]==255)) {
                if (c[i+1]==0&&c[i+2]==255) c.indexAdd(i, s);
                else if (c[i+1]==255&&c[i+2]==0) c.indexAdd(i, -s);
            }
        }
    }
    void print(std::string s) {c.print(s);}
    void print2d(std::string s, int len = 30) {
        rainbow r;
        r.init(len);
        r.c.set(c);
        for (int i = 0; i < s.length(); i++) {
            r.print(s.substr(i, 1));
            r.next();
        }
    }
};

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
        if (std::filesystem::is_directory(entry.path())) {
            item.name += "/";
            item.size = "Folder    ";
            folders.push_back(item);
            continue;
        } else if (std::filesystem::exists(entry.path()))
            item.size = convsize(std::filesystem::file_size(entry.path()));
        else item.size = convsize(0);
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