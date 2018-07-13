#include "../platform.hpp"
#include <sys/stat.h>
#include <android/log.h>
#include <errno.h>
#include <dlfcn.h>

#define ndk_log(...) __android_log_print(ANDROID_LOG_INFO, "xxprofile", __VA_ARGS__)

XX_NAMESPACE_BEGIN(xxprofile);

static void mkdirs(const std::string& path) {
    size_t index = 0;
    for (;;) {
        index = path.find('/', index);
        if (index == std::string::npos) {
            break;
        }
        if (index != 0) {
            int ret = mkdir(path.substr(0, index).c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
            if (ret) {
                ndk_log("mkdir error %d:%s", errno, path.substr(0, index).c_str());
            }
        }
        ++index;
    }
}

std::string systemGetWritablePath() {
    static bool check_sdcard = true;
    static std::string sdcardPath;
    if (check_sdcard) {
        check_sdcard = false;
        static int value_ = 0;
        Dl_info info;
        dladdr(&value_, &info);
        const char* fname = strrchr(info.dli_fname, '/');
        if (fname) {
            ++fname;
        } else {
            fname = info.dli_fname;
        }
        sdcardPath = "/sdcard/xxprofile/";
        sdcardPath += fname;
        sdcardPath += "/";
        mkdirs(sdcardPath);
        struct stat s;
        if (stat(sdcardPath.c_str(), &s) != 0 || !S_ISDIR(s.st_mode)) {
            sdcardPath.clear();
        }
    }
    ndk_log("WritablePath:%s", sdcardPath.c_str());
    return sdcardPath;
}

XX_NAMESPACE_END(xxprofile);
