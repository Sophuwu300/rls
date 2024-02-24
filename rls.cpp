#include <stdio.h>
#include <string>
#include <cstring>
#include <filesystem>
#include <sys/stat.h>
#include <vector>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <cmath>
#include "rainbow.h"
#include "rls.h"

/////// DIRLIST DEFINITIONS ///////
ENTLIST DIRLIST::all() {
    DIRENTS all;
    sorter(dirs, all);
    sorter(files, all);
    int len = all.size();
    if (len < 10) len = 10;
    else if (len > 50) len = 50;
    prnt.r.init(len);
    return ENTLIST{all};
}

int DIRLIST::charcomp(char a, char b) {
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
        cmp = charcomp(c[i], d[i]);
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
/////// PATHS DEFINITIONS ///////
void PATHS::add(std::string str) {paths.push_back(std::filesystem::path(str));}

void PATHS::remove(int n) {paths.erase(paths.begin()+n);}
PATH PATHS::path() const {return paths[i];}
PATH PATHS::operator ()()  {return path();}
void PATHS::next() {i++;}
bool PATHS::end() {return i >= paths.size();}
void PATHS::first() {i = 0;}
void PATHS::validate() {
    if (size()==0) add(".");
    for (first();!end(); next()) {
        if (!std::filesystem::exists(path()) || !std::filesystem::is_directory(path())) {
            prnt(path().string()+" not valid; skipping.");
        }
    }
    first();
}
DIRLIST PATHS::read() {
    DIRLIST list;
    for (const auto& entry : std::filesystem::directory_iterator(path())) {
        if (std::filesystem::is_directory(entry)) list.dirs.push_back(entry);
        else list.files.push_back(entry);
    }
    return list;
}
size_t PATHS::size() const {return paths.size();}
std::string PATHS::str() {
    std::string p = path().string();
    if (p[p.length()-1] != '/') p.append("/");
    return p;
}
/////// FLAGS DEFINITIONS ///////
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
        else paths.add(arg);
    }
    if (paths.size() == 0) paths.add(".");
}
/////// printer DEFINITIONS ///////
void printer::operator()() {printf("\n");}

void printer::operator()(std::string str) {
    if (flags.color) {
        r.print2d(str + "\n");
        r.next();
    }
    else printf("%s\n", str.c_str());
}

/////// ENTLIST DEFINITIONS ///////
void ENTLIST::print() {
    for (DIRENT e : list) {
        ST{e};
    }
}

/////// ST DEFINITIONS ///////
std::string ST::perms() {
    std::string permissions;
    permissions += (S_ISDIR(st.st_mode)) ? 'd' : '-';
    permissions += (st.st_mode & S_IRUSR) ? 'r' : '-';
    permissions += (st.st_mode & S_IWUSR) ? 'w' : '-';
    permissions += (st.st_mode & S_IXUSR) ? 'x' : '-';
    permissions += (st.st_mode & S_IRGRP) ? 'r' : '-';
    permissions += (st.st_mode & S_IWGRP) ? 'w' : '-';
    permissions += (st.st_mode & S_IXGRP) ? 'x' : '-';
    permissions += (st.st_mode & S_IROTH) ? 'r' : '-';
    permissions += (st.st_mode & S_IWOTH) ? 'w' : '-';
    permissions += (st.st_mode & S_IXOTH) ? 'x' : '-';
    return permissions;
}
std::string pad(std::string s, int len) {
    for(; s.length() < len; s=" "+s);
    return s;
}
std::string ST::owner() {
    struct passwd *pw = getpwuid(st.st_uid);
    if (pw == nullptr) {
        return "Unknown";
    }
        struct group *grp = getgrgid(st.st_gid);
    if (grp == nullptr) {
        return "Unknown";
    }
    return pad(std::string(pw->pw_name), 8) + " " + pad(std::string(grp->gr_name), 8);
}
std::string ST::size() {
    if (st.st_mode & S_IFDIR) return "folder  ";
    const char* suffix = "BKMGT";
    D s = D(st.st_size);
    uint size = uint(log(s)/log(1024));
    char b[50];
    char d[50];
    sprintf(b, "%.2lf", D(D(s) / D(pow(uint(1024), size))));
    sprintf(d, "%6s %c", std::string(b).c_str(), suffix[size]);
    return std::string(d);
}
std::string ST::time() {
    char time[50];
    strftime(time, 50, "%b %d %H:%M", localtime(&st.st_mtime));
    return std::string(time);
}
std::string ST::init(const DIRENT& e) {
    std::string outstr = "";
    if (stat(e.path().c_str(), &st) != 0) return "err"; //error reading file
    if (flags.list) outstr += perms() + "  " + owner() + "  ";
    outstr += size() + "  ";
    if (flags.list) outstr += time() + "  ";
    outstr += e.path().filename().string();
    if (e.is_directory()) outstr += "/";
    return outstr;
}
ST::ST(const DIRENT &e) {
    prnt(init(e));
}

/////// MAIN ///////
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

int main(int argc, char* argv[]) {
    flags.parse(argc, argv);
    if (flags.help) runHelp(argv[0]);
    for (paths.first(); !paths.end(); paths.next()) {
        prnt(paths.str());
        ENTLIST list = ENTLIST{paths.read().all()};
        list.print();
    }
    return 0;
}