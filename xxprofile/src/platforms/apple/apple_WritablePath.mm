#include "../platform.hpp"
#import <Foundation/Foundation.h>
#include <TargetConditionals.h>
#include <sys/stat.h>
#include <dlfcn.h>

XX_NAMESPACE_BEGIN(xxprofile);

std::string systemGetWritablePath() {
    // save to document folder
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    std::string strRet = [documentsDirectory UTF8String];
    strRet.append("/");

#if TARGET_OS_IOS == 0
    NSString *name = [[[NSBundle mainBundle] infoDictionary] objectForKey:(NSString*)kCFBundleIdentifierKey];
    if (name != nil) {
        strRet.append("xxprofile/");
        mkdir(strRet.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
        strRet.append([name UTF8String]);
        strRet.append("/");
        mkdir(strRet.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
    } else {
        static int value_ = 0;
        Dl_info info;
        dladdr(&value_, &info);
        const char* fname = strrchr(info.dli_fname, '/');
        if (fname) {
            strRet.assign(info.dli_fname, fname + 1 - info.dli_fname);
        } else {
            strRet.append("xxprofile/");
            mkdir(strRet.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
            strRet.append(info.dli_fname);
            strRet.append("/");
            mkdir(strRet.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
        }
    }
#endif//TARGET_OS_MAC == 1
    return strRet;
}

XX_NAMESPACE_END(xxprofile);
