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
            if (ret && errno != 17) {
                ndk_log("mkdir error %d:%s", errno, path.substr(0, index).c_str());
            }
        }
        ++index;
    }
}

std::string android_getDlName();

static std::string filesDirFromBid(const std::string& bid) {
    return std::string("/data/data/") + bid + "/files";
}

static bool testIsBid(const std::string& bid) {
    std::string files = filesDirFromBid(bid);
    struct stat s;
    return (stat(files.c_str(), &s) == 0 && S_ISDIR(s.st_mode));
}

std::string systemGetWritablePath() {
    static bool checked_sdcard = false;
    static std::string writablePath;
    if (!checked_sdcard) {
        checked_sdcard = true;
        writablePath = "/sdcard/xxprofile/";
        writablePath += android_getDlName();
        writablePath += "/";
        mkdirs(writablePath);
        struct stat s;
        if (stat(writablePath.c_str(), &s) != 0 || !S_ISDIR(s.st_mode)) {
            writablePath.clear();
        }
    }
    static bool checked_bid = false;
    if (writablePath.empty() && !checked_bid) {
        checked_bid = true;
        static int value_ = 0;
        Dl_info info;
        dladdr(&value_, &info);
        const char* data_app = "/data/app/";
        const auto data_app_len = strlen(data_app);
        if (strlen(info.dli_fname) > data_app_len) {
            if (memcmp(info.dli_fname, data_app, data_app_len) == 0) {
                const char* sname = info.dli_fname + data_app_len;
                const char* ename0 = strchr(sname, '-');
                const char* ename = strchr(sname, '/');
                if (ename) {
                    std::string bid(sname, ename0 < ename ? ename0 - sname : ename - sname);
                    if (testIsBid(bid)) {
                        writablePath = filesDirFromBid(bid) + "/";
                    }
                }
            }
        }
    }
    ndk_log("WritablePath:%s", writablePath.c_str());
    return writablePath;
}

XX_NAMESPACE_END(xxprofile);
