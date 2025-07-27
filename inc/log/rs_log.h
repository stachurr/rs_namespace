#ifndef RS_LOG_H
#define RS_LOG_H
#include "../../rs_config_private.h"
#if RS_ENABLE_LOG

#include <iostream>
#include <source_location>
#include "rs_log_private.h"


namespace rs {
namespace log {
    // template<size_t N, typename... ArgTypes>
    // std::false_type error (const char (&RS_RESTRICT format)[N], ArgTypes... args) {
    //     printf("%s\n", __FUNCDNAME__);
    //     _private::log(level_t::ERROR, format, args...);
    //     return std::false_type();
    // }
    // 
    // template<size_t N, typename... ArgTypes>
    // void warn (const char (&RS_RESTRICT format)[N], ArgTypes... args) {
    //     printf("%s\n", __FUNCDNAME__);
    //     _private::log(level_t::WARN, format, args...);
    // }
    // 
    // template<size_t N, typename... ArgTypes>
    // void info (const char (&RS_RESTRICT format)[N], ArgTypes... args) {
    //     _private::log(level_t::INFO, format, args...);
    // }
    // 
    // template<size_t N, typename... ArgTypes>
    // void trace (const char (&RS_RESTRICT format)[N], ArgTypes... args) {
    //     _private::log(level_t::TRACE, format, args...);
    // }


    // // https://stackoverflow.com/a/71082768/19514931
    // 
    // template<size_t N, typename... Ts>
    // struct error {
    //     error (const char (&format)[N], Ts&&... ts, const std::source_location &loc = std::source_location::current()) {
    //         _private::log(level_t::ERROR, loc, format, ts...);
    //     }
    // };
    // template<size_t N, typename... Ts>
    // error(const char (&)[N], Ts&&...) -> error<N, Ts...>;
    // 
    // template<size_t N, typename... Ts>
    // struct warn {
    //     warn (const char (&format)[N], Ts&&... ts, const std::source_location &loc = std::source_location::current()) {
    //         _private::log(level_t::WARN, loc, format, ts...);
    //     }
    // };
    // template<size_t N, typename... Ts>
    // warn(const char (&)[N], Ts&&...) -> warn<N, Ts...>;
    // 
    // template<size_t N, typename... Ts>
    // struct info {
    //     info (const char (&format)[N], Ts&&... ts, const std::source_location &loc = std::source_location::current()) {
    //         _private::log(level_t::INFO, loc, format, ts...);
    //     }
    // };
    // template<size_t N, typename... Ts>
    // info(const char (&)[N], Ts&&...) -> info<N, Ts...>;
    // 
    // template<size_t N, typename... Ts>
    // struct trace {
    //     trace (const char (&format)[N], Ts&&... ts, const std::source_location &loc = std::source_location::current()) {
    //         _private::log(level_t::TRACE, loc, format, ts...);
    //     }
    // };
    // template<size_t N, typename... Ts>
    // trace(const char (&)[N], Ts&&...) -> trace<N, Ts...>;


    template<typename... Args>
    void error (format_string_auto_location&& fmt, Args&&... args) {
        _private::log(level_t::ERROR, std::move(fmt), args...);
    }

    template<typename... Args>
    void warn (format_string_auto_location&& fmt, Args&&... args) {
        _private::log(level_t::WARN, std::move(fmt), args...);
    }

    template<typename... Args>
    void info (format_string_auto_location&& fmt, Args&&... args) {
        _private::log(level_t::INFO, std::move(fmt), args...);
    }

    template<typename... Args>
    void trace (format_string_auto_location&& fmt, Args&&... args) {
        _private::log(level_t::TRACE, std::move(fmt), args...);
    }
} // rs::log
} // rs

#endif // RS_ENABLE_LOG
#endif // RS_LOG_H
