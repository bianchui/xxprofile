#include "../platform.hpp"
#import <Foundation/Foundation.h>
#include <sys/stat.h>

XX_NAMESPACE_BEGIN(xxprofile);

std::string systemGetWritablePath() {
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    std::string strRet = [documentsDirectory UTF8String];
    strRet.append("/");
    NSString *name = [[[NSBundle mainBundle] infoDictionary] objectForKey:(NSString*)kCFBundleIdentifierKey];
    if (name != nil) {
        strRet.append([name UTF8String]);
        strRet.append("/");
        mkdir(strRet.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
    }
    return strRet;
}

XX_NAMESPACE_END(xxprofile);
