#include "../platform.hpp"
#include <sys/stat.h>

XX_NAMESPACE_BEGIN(xxprofile);

static void mkdirs(const std::string& path) {
    size_t index = 0;
    for (;;) {
        index = path.find('/', index);
        if (index == std::string::npos) {
            break;
        }
        if (index != 0) {
            mkdir(path.substr(0, index).c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
        }
        ++index;
    }
}

std::string systemGetWritablePath() {
    static bool check_sdcard = true;
    static std::string sdcardPath;
    if (check_sdcard) {
        check_sdcard = false;
        sdcardPath = "/sdcard/data/xxprofile/";
        mkdirs(sdcardPath);
        struct stat s;
        if (stat(sdcardPath.c_str(), &s) != 0 || !S_ISDIR(s.st_mode)) {
            sdcardPath.clear();
        }
    }
    return sdcardPath;
}

XX_NAMESPACE_END(xxprofile);
