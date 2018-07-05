#include "../platform.hpp"
#import <Foundation/Foundation.h>

XX_NAMESPACE_BEGIN(xxprofile);

std::string systemGetWritablePath() {
    // save to document folder
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    std::string strRet = [documentsDirectory UTF8String];
    strRet.append("/");
    return strRet;
}

XX_NAMESPACE_END(xxprofile);
