//
//  Adapted from http://stackoverflow.com/questions/6168107/how-to-implement-a-good-debug-logging-feature-in-a-project
//  Created by Catalin Voss on 7/14/14.
//

#ifndef __LOG1_H__
#define __LOG1_H__

#include <sstream>
#include <string>
#include <functional>
#include <stdio.h>

// Helpers
#define loge Log().Get(LOG_ERROR)
#define logw Log().Get(LOG_WARN)
#define logi Log().Get(LOG_INFO)
#define logd Log().Get(LOG_DEBUG)
#define Debug if(LOG_DEBUG <= Log().ReportingLevel())

// Android
#ifdef ANDROID_NDK
#include <jni.h>
#include <android/log.h>
#define LOG_TAG "NativeModule"
#define LOGD_ANDROID(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))
#endif

using namespace std;

inline string NowTime();

enum SELogLevel {LOG_ERROR, LOG_WARN, LOG_INFO, LOG_DEBUG};

class Log {
public:
    Log();
    virtual ~Log();
    ostringstream &Get(SELogLevel level = LOG_INFO);
    static SELogLevel &ReportingLevel();
    static string ToString(SELogLevel level);
    static SELogLevel FromString(const string &level);
    void Run(SELogLevel level, function<void()> &block);
protected:
    ostringstream os;
    SELogLevel messageLevel;
private:
    Log(const Log &);
    Log &operator =(const Log &);
};

inline Log::Log() {
    messageLevel = (SELogLevel)-1;
}

inline void Log::Run(SELogLevel level, function<void()> &block) {
    if (level <= ReportingLevel()) {
        block();
    }
}

inline ostringstream &Log::Get(SELogLevel level) {
    messageLevel = level;
    os << NowTime();
    os << " " << ToString(level) << ": ";
    os << string(level > LOG_DEBUG ? level - LOG_DEBUG : 0, '\t');
    return os;
}

inline Log::~Log() {
    if ((int)messageLevel != -1 && messageLevel <= ReportingLevel()) {
        os << endl;
#ifdef ANDROID_NDK
        LOGD_ANDROID("%s", os.str().c_str());
#else
        fprintf(stderr, "%s", os.str().c_str());
        fflush(stderr);
#endif
    }
}

inline SELogLevel &Log::ReportingLevel() {
    static SELogLevel reportingLevel = LOG_DEBUG;
    return reportingLevel;
}

inline string Log::ToString(SELogLevel level) {
    static const char* const buffer[] = {"ERROR", "WARNING", "INFO", "DEBUG"};
    return buffer[level];
}

inline SELogLevel Log::FromString(const string &level) {
    if (level == "DEBUG")
    { return LOG_DEBUG; }
    
    if (level == "INFO")
    { return LOG_INFO; }
    
    if (level == "WARNING")
    { return LOG_WARN; }
    
    if (level == "ERROR")
    { return LOG_ERROR; }
    
    Log().Get(LOG_WARN) << "Unknown logging level '" << level << "'. Using INFO level as default.";
    return LOG_INFO;
}

typedef Log FILELog;

#define FILE_LOG(level) \
if (level > FILELog::ReportingLevel()) ; \
else Log().Get(level)

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
