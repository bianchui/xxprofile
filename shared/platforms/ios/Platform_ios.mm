// Copyright 2018 bianchui. All rights reserved.
#include "platform/Platform.h"
#import <UIKit/UIDevice.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <pthread.h>

SHARED_NAMESPACE_BEGIN;

static uint32_t OSParseVersion(const char* ver) {
    uint32_t majorVersion, minorVersion, patchVersion;
    char* end;
    majorVersion = (uint32_t)strtol(ver, &end, 10);
    ver = *end != 0 ? end + 1 : "";
    minorVersion = (uint32_t)strtol(ver, &end, 10);
    ver = *end != 0 ? end + 1 : "";
    patchVersion = (uint32_t)strtol(ver, &end, 10);
    return (majorVersion << 24) + (minorVersion << 16) + patchVersion;
}

uint32_t OSGetVersion() {
    static uint32_t s_version;
    if (!s_version) {
        s_version = OSParseVersion([UIDevice currentDevice].systemVersion.UTF8String);
    }
    return s_version;
}

const char* platformAbiName() {
#if defined(__ARM_ARCH)
#  if __ARM_ARCH == 6// __arm__
#    define Platform_ABI "armv6"
#  elif __ARM_ARCH == 7// __arm__
#    if defined(__ARM_ARCH_7S__)
#      define Platform_ABI "armv7s"
#    else
#      define Platform_ABI "armv7"
#    endif
#  elif __ARM_ARCH == 8// __arm64__
#    if defined(__ARM_ARCH_8_3__) || defined(__ARM_ARCH_8_2__)
#      define Platform_ABI "arm64e"
#    else
#      define Platform_ABI "arm64"
#    endif
#  else
#    define Platform_ABI "arm"
#  endif
#elif defined(__i386__)
#  define Platform_ABI "x86"
#elif defined(__x86_64__)
#  define Platform_ABI "x86_64"
#else
#  define Platform_ABI "unknown"
#endif
    return Platform_ABI;
#undef Platform_ABI
}

uint32_t platformGetTid() {
    return ::syscall(SYS_thread_selfid);
}

void platformSetThreadName(const char* name) {
    pthread_setname_np(name);
}

// https://github.com/CaiChenghan/CCAdition/blob/master/Pod/Classes/NSDictionary+Addition.m
static NSDictionary* Dictionary_getDictionaryValueForKey(NSDictionary* dic, NSString* key, NSDictionary* defaultValue) {
    if (dic == nil) {
        return defaultValue;
    }
    id tmpValue = [dic objectForKey:key];
    if (tmpValue == nil || tmpValue == [NSNull null]) {
        return defaultValue;
    }
    if ([tmpValue isKindOfClass:[NSDictionary class]]) {
        return tmpValue;
    } else {
        return defaultValue;
    }
}

static BOOL Dictionary_getBooleanValueForKey(NSDictionary* dic, NSString* key, BOOL defaultValue) {
    if (dic == nil) {
        return defaultValue;
    }
    id tmpValue = [dic objectForKey:key];
    if (tmpValue == nil || tmpValue == [NSNull null]) {
        return defaultValue;
    }
    if ([tmpValue isKindOfClass:[NSNumber class]]) {
        return [tmpValue boolValue];
    } else {
        @try {
            return [tmpValue boolValue];
        } @catch (NSException *exception) {
            return defaultValue;
        } @finally {
            return defaultValue;
        }
    }
}

bool platformIsDebugable() {
    //取出embedded.mobileprovision这个描述文件的内容进行判断
    NSString *mobileProvisionPath = [[NSBundle mainBundle] pathForResource:@"embedded" ofType:@"mobileprovision"];
    NSData *rawData = [NSData dataWithContentsOfFile:mobileProvisionPath];
    if (rawData == nil) {
        return false;
    }
    NSString *rawDataString = [[NSString alloc] initWithData:rawData encoding:NSASCIIStringEncoding];
    NSRange plistStartRange = [rawDataString rangeOfString:@"<plist"];
    NSRange plistEndRange = [rawDataString rangeOfString:@"</plist>"];
    if (plistStartRange.location != NSNotFound && plistEndRange.location != NSNotFound) {
        NSString *tempPlistString = [rawDataString substringWithRange:NSMakeRange(plistStartRange.location, NSMaxRange(plistEndRange))];
        NSData *tempPlistData = [tempPlistString dataUsingEncoding:NSUTF8StringEncoding];
        NSDictionary *plistDic =  [NSPropertyListSerialization propertyListWithData:tempPlistData options:NSPropertyListImmutable format:nil error:nil];
        NSDictionary *entitlementsDic = Dictionary_getDictionaryValueForKey(plistDic, @"Entitlements", nil);
        if (entitlementsDic == nil) {
            return false;
        }
        return Dictionary_getBooleanValueForKey(entitlementsDic, @"get-task-allow", NO) == YES;
    }
    return false;
}

SHARED_NAMESPACE_END;
