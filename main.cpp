#include <iostream>
#include <string>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <stdarg.h>
#include <iostream>
#include <tinydir.h>
#include <sys/stat.h>
#include <unistd.h>
#include <filesystem>
#include <iostream>
#include <string>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <stdarg.h>
#include <vector>
#include <argh.h>
using std::filesystem::current_path;
long filesizefunc(std::string filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}


void rgbpf(uint8_t r, uint8_t g, uint8_t b, const std::string fmt, ...) {
    int size = ((int)fmt.size()) * 2 + 50;
    std::string str;
    va_list ap;
    while (1) {
        str.resize(size);
        va_start(ap, fmt);
        int n = vsnprintf((char *)str.data(), size, fmt.c_str(), ap);
        va_end(ap);
        if (n > -1 && n < size) {
            str.resize(n);
            std::cout << "\033[38;2;" << +r << ";" << +g << ";" << +b << "m" << str;
            return;
        }
        if (n > -1) size = n + 1;
        else size *= 2;
    }
}


int main(int argc, char *argv[]) {
    argh::parser cmdl(argv);
    tinydir_dir dir;
    int i;
    double size;
    long sizelong;
    char unit = 'B';
    char str [6];
    int n;

    std::ostream& out = std::cout;
    char workdir[256];
    getcwd(workdir, 256);
    if (argc > 1) {
        char randomdir[256];
        strcpy(randomdir, argv[1]);
        if (randomdir[0] != '/') {
            strcat(workdir, "/");
            strcat(workdir, randomdir);
        } else {
            strcpy(workdir, randomdir);
        }
    }

    tinydir_open_sorted(&dir, workdir);
    rgbpf(255,255,0,"%s%s", dir.path, "\n\n");

    double step = 0;
    if (dir.n_files > 0) {
        step = 5*(double)255 / ((double)dir.n_files-1);
    } else {
        printf("No files in directory\n\n");
        return 0;
    }


    double r = 255;
    double g = 0;
    double b = 0;
    bool step1 = false;
    bool step2 = false;
    bool step3 = false;
    bool step4 = false;

    for (i = 2; i < dir.n_files; i++)
    {
        if (!step1) g += step;
        else if (!step2) r -= step;
        else if (!step3) b += step;
        else if (!step4) g -= step;
        else r += step;
        if (g >= 255) {
            g = 255;
            step1 = true;
        }
        if (r <= 0) {
            r = 0;
            step2 = true;
        }
        if (b >= 255) {
            b = 255;
            step3 = true;
        }
        if (g <= 0) {
            g = 0;
            step4 = true;
        }
        if (r >= 255) {
            r = 255;
        }
        tinydir_file file;
        tinydir_readfile_n(&dir, &file, i);

        if (file.is_dir)
        {
            rgbpf(r,g,b, "%s","  Folder     ");
        } else {
            sizelong = filesizefunc(file.path);
            size = (double)sizelong;
            if (size > 1000000000000) {
                size = size / 1000000000000;
                unit = 'T';
            } else if (size > 1000000000) {
                size = size / 1000000000;
                unit = 'G';
            } else if (size > 1000000) {
                size = size / 1000000;
                unit = 'M';
            } else if (size > 1000) {
                size = size / 1000;
                unit = 'K';
            } else {
                unit = ' ';
            }
            if (size >= 1) {
                n = sprintf(str, "%.2f", size);
                n=5-n;
                char space [n];
                int j = 0;
                while (j < n+1) {
                    space[j] = ' ';
                    j++;
                }
                space[j] = '\0';
                rgbpf(r,g,b,"  %s%s %c   ", space, str, unit);
            }
        }
        rgbpf(r,g,b,"%s%s", file.name, "\n");
    }
    printf("\n\n");
    tinydir_close(&dir);

    return 0;

}
