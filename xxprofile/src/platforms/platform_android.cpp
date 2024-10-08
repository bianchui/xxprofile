// Copyright 2017 bianchui. All rights reserved.
#include "platform_android.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <dlfcn.h>
#include <string>
#include <sys/stat.h>
#include <android/log.h>
#include <errno.h>

XX_NAMESPACE_BEGIN(xxprofile);

void log(const char* format, ...) {
    va_list ap;
    va_start(ap, format);
    __android_log_vprint(ANDROID_LOG_INFO, "xxprofile", format, ap);
    va_end(ap);
}

uint32_t systemGetTid() {
    return ::gettid();
}

static const std::string& getBid();

static void getDlInfo(Dl_info* info) {
    static int value_ = 0;
    dladdr(&value_, info);
}

static std::string android_getDlName() {
    Dl_info info;
    getDlInfo(&info);
    const char* fname = strrchr(info.dli_fname, '/');
    if (fname) {
        ++fname;
    } else {
        fname = info.dli_fname;
    }
    return fname;
}

static std::string android_getDlPath() {
    Dl_info info;
    getDlInfo(&info);
    const char* fname = strrchr(info.dli_fname, '/');
    if (!fname) {
        fname = info.dli_fname;
    }
    return std::string(info.dli_fname, fname - info.dli_fname);
}

std::string systemGetAppName() {
    const std::string& bid = getBid();
    if (!bid.empty()) {
        return bid;
    }
    return android_getDlName();
}

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
                log("mkdir error %d:%s", errno, path.substr(0, index).c_str());
            }
        }
        ++index;
    }
}

static std::string filesDirFromBid(const std::string& bid) {
    return std::string("/data/data/") + bid + "/files/";
}

static bool testIsBid(const std::string& bid) {
    std::string files = filesDirFromBid(bid);
    struct stat s;
    return (stat(files.c_str(), &s) == 0 && S_ISDIR(s.st_mode));
}

static const std::string& getBid() {
    static bool checked_bid = false;
    static std::string gBid;
    if (!checked_bid) {
        checked_bid = true;
        Dl_info info;
        getDlInfo(&info);
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
                        gBid = bid;
                    }
                }
            }
        }
    }
    return gBid;
}

static bool dirIsWritable(const std::string& dir) {
    struct stat s;
    if (stat(dir.c_str(), &s) != 0 || !S_ISDIR(s.st_mode)) {
        return false;
    }
    std::string tests = dir + ".xxtest";
    FILE* fp = fopen(tests.c_str(), "wb");
    if (fp) {
        fclose(fp);
        unlink(tests.c_str());
    }
    return fp != nullptr;
}

std::string systemGetWritablePath() {
    static bool checked_sdcard = false;
    static std::string writablePath;
    if (!checked_sdcard) {
        checked_sdcard = true;
        writablePath = "/sdcard/xxprofile/";
        writablePath += systemGetAppName();
        writablePath += "/";
        mkdirs(writablePath);
        if (!dirIsWritable(writablePath)) {
            writablePath.clear();
        }
    }
    static bool checked_bid = false;
    if (writablePath.empty() && !checked_bid) {
        checked_bid = true;
        const std::string& bid = getBid();
        if (!bid.empty()) {
            std::string files = filesDirFromBid(bid);
            if (dirIsWritable(files)) {
                writablePath = files;
            }
        }
    }
    static bool checked_dlPath = false;
    if (writablePath.empty() && !checked_dlPath) {
        checked_dlPath = false;
        std::string dlPath = android_getDlPath();
        if (!dlPath.empty() && dirIsWritable(dlPath)) {
            writablePath = dlPath;
        }
    }
    log("WritablePath:%s", writablePath.c_str());
    return writablePath;
}

XX_NAMESPACE_END(xxprofile);
