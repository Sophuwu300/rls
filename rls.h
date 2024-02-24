typedef std::filesystem::path PATH;
typedef std::filesystem::directory_entry DIRENT;
typedef std::vector<DIRENT> DIRENTS;
typedef unsigned int uint;
typedef unsigned long long int UL;
typedef double D;
struct ENTLIST{
    DIRENTS list;
    void print();
};
struct DIRLIST {
    DIRENTS files;
    DIRENTS dirs;
    ENTLIST all();
    int charcomp(char a, char b);
    int strcomp(std::string a, std::string b);
    void sorter(DIRENTS& list, DIRENTS& newlist);
};
struct PATHS {
    size_t i = 0;
    std::vector<PATH> paths;
    size_t size() const;
    void add(std::string str);
    void remove(int n);
    PATH path() const;
    PATH operator ()();
    void first();
    void next();
    bool end();
    void validate();
    DIRLIST read();
    std::string str();
}paths;
struct FLAGS {
    bool color = true;
    bool list = false;
    bool all = false;
    bool number = false;
    bool help = false;
    void parse(int argc, char* argv[]);
    void toggle(std::string arg);
}flags;
struct printer {
    printer() {r.init(30);}
    rainbow r;
    void operator()(std::string str);
    void operator()();
}prnt;
struct ST {
    ST(DIRENT e);
    std::string init(DIRENT e);
    struct stat st;
    std::string perms();
    std::string owner();
    std::string size();
    std::string str();
};