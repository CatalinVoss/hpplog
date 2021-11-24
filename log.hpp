//
//  Open source universal logging tool (from https://github.com/CatalinVoss/hpplog/)
//  Created by Catalin Voss on 7/14/14.
// 
//  Usage:
//      logd << "Some debug log message as if this was cout." << std::endl;
//

#ifndef __LOG1_H__
#define __LOG1_H__

#include <sstream>
#include <string>
#include <functional>
#include <stdio.h>

// Helpers
#define loge SELog().Get(LOG_ERROR)
#define logw SELog().Get(LOG_WARN)
#define logi SELog().Get(LOG_INFO)
#define logd SELog().Get(LOG_DEBUG)

// Android
#ifdef ANDROID
#include <jni.h>
#include <android/log.h>
#define LOG_TAG "NativeModule"
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#endif

using namespace std;

inline string NowTime();

enum SELogLevel {LOG_ERROR, LOG_WARN, LOG_INFO, LOG_DEBUG};

class SELog {
public:
    SELog();
    virtual ~SELog();
    ostringstream &Get(SELogLevel level = LOG_INFO);
    static SELogLevel &ReportingLevel();
    static string ToString(SELogLevel level);
    static SELogLevel FromString(const string &level);
    void Run(SELogLevel level, function<void()> &block);
protected:
    ostringstream os;
    SELogLevel messageLevel;
private:
    SELog(const SELog &);
    SELog &operator =(const SELog &);
};

inline SELog::SELog() {
    messageLevel = (SELogLevel)-1;
}

inline void SELog::Run(SELogLevel level, function<void()> &block) {
    if (level <= ReportingLevel()) {
        block();
    }
}

inline ostringstream &SELog::Get(SELogLevel level) {
    messageLevel = level;
    os << NowTime();
    os << " " << ToString(level) << ": ";
    os << string(level > LOG_DEBUG ? level - LOG_DEBUG : 0, '\t');
    return os;
}

inline SELog::~SELog() {
    if ((int)messageLevel != -1 && messageLevel <= ReportingLevel()) {
        // If we want to default to the line break, uncomment
        // os << endl;
#ifdef ANDROID
        if (messageLevel == LOG_ERROR)
            LOGE("%s", os.str().c_str());
        else if (messageLevel == LOG_WARN)
            LOGW("%s", os.str().c_str());
        else if (messageLevel == LOG_INFO)
            LOGI("%s", os.str().c_str());
        else
            LOGD("%s", os.str().c_str());
#else
        fprintf(stderr, "%s", os.str().c_str());
        fflush(stderr);
#endif
    }
}

inline SELogLevel &SELog::ReportingLevel() {
    static SELogLevel reportingLevel = LOG_DEBUG;
    return reportingLevel;
}

inline string SELog::ToString(SELogLevel level) {
    static const char* const buffer[] = {"ERROR", "WARNING", "INFO", "DEBUG"};
    return buffer[level];
}

inline SELogLevel SELog::FromString(const string &level) {
    if (level == "DEBUG")
    { return LOG_DEBUG; }
    
    if (level == "INFO")
    { return LOG_INFO; }
    
    if (level == "WARNING")
    { return LOG_WARN; }
    
    if (level == "ERROR")
    { return LOG_ERROR; }
    
    SELog().Get(LOG_WARN) << "Unknown logging level '" << level << "'. Using INFO level as default.";
    return LOG_INFO;
}

typedef SELog FILELog;

#define FILE_LOG(level) \
if (level > FILELog::ReportingLevel()) ; \
else SELog().Get(level)

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)

#include <windows.h>

inline string NowTime() {
    const int MAX_LEN = 200;
    char buffer[MAX_LEN];
    
    if (GetTimeFormatA(LOCALE_USER_DEFAULT, 0, 0,
                       "HH':'mm':'ss", buffer, MAX_LEN) == 0)
    { return "Error in NowTime()"; }
    
    char result[100] = {0};
    static DWORD first = GetTickCount();
    sprintf(result, "%s.%03ld", buffer, (long)(GetTickCount() - first) % 1000);
    return result;
}

#else

#include <sys/time.h>

inline string NowTime() {
    char buffer[11];
    time_t t;
    time(&t);
    tm r = {0};
    strftime(buffer, sizeof(buffer), "%X", localtime_r(&t, &r));
    struct timeval tv;
    gettimeofday(&tv, 0);
    char result[100] = {0};
    sprintf(result, "%s.%03ld", buffer, (long)tv.tv_usec / 1000);
    return result;
}

#endif //WIN32

#endif //__LOG1_H__
