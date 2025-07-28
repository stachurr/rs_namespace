#ifndef RS_LOG_PRIVATE_H
#define RS_LOG_PRIVATE_H

#include <cstdint>
#include <type_traits>
#include <memory>
#include <source_location>
#include <filesystem>
#include "rs_log_level.h"
#include "../ansi/rs_ansi.h"
#include "../rs_source_location_capture.h"


#ifdef _WIN32
    #define RS_RESTRICT __restrict
#elif defined(__GNUC__)
    #define RS_RESTRICT __restrict__
#else
    #define RS_RESTRICT
#endif


#pragma push_macro("ERROR_COLOR")
#pragma push_macro("WARN_COLOR")
#pragma push_macro("INFO_COLOR")
#pragma push_macro("TRACE_COLOR")
#if RS_LOG_ENABLE_COLOR
    #define ERROR_COLOR     RS_RGB(239, 71, 111)
    #define WARN_COLOR      RS_RGB(255, 209, 102)
    #define INFO_COLOR      RS_RGB(17, 138, 178)
    #define TRACE_COLOR     RS_RGB(100, 100, 100) //RS_GRAY //RS_RGB(7, 59, 76)
#else
    #define ERROR_COLOR     ""
    #define WARN_COLOR      ""
    #define INFO_COLOR      ""
    #define TRACE_COLOR     ""
#endif


namespace rs {
namespace log {
namespace _private {
    static const char* LOG_LEVEL_TITLES[] = {
        ERROR_COLOR "ERROR" RS_DEFAULT,
        WARN_COLOR  "WARN"  RS_DEFAULT,
        INFO_COLOR  "INFO"  RS_DEFAULT,
        TRACE_COLOR "TRACE" RS_DEFAULT
    };

    constexpr const char *const get_level_string (level_t level) {
        return LOG_LEVEL_TITLES[static_cast<std::underlying_type_t<level_t>>(level) - static_cast<std::underlying_type_t<level_t>>(level_t::MIN)];
    }

    template<typename... ArgTypes>
    std::unique_ptr<char[]> format_string (const char *const RS_RESTRICT format, ArgTypes&&... args) {
        auto s = std::unique_ptr<char[]>(nullptr);

        if constexpr (sizeof...(ArgTypes) > 0) {
            // Check format validity.
            // https://en.cppreference.com/w/cpp/io/c/snprintf
            const int size_req = 1 + std::snprintf(nullptr, 0, format, args...);    // +1 for null-terminator

            // Format is valid!
            if (size_req > 0) {
                s.reset(new char[size_req]);
                const int a = std::snprintf(s.get(), size_req, format, args...);
            }
            // Format is invalid.
            else {
                throw std::invalid_argument("Invalid format string");
            }
        }
        else {
            // No formatting required. Maybe we can avoid copying somehow...
            const auto N = strlen(format);
            s.reset(new char[N + 1]);
            std::memcpy(s.get(), format, N + 1);
        }

        return s;
    }

    template<typename... ArgTypes>
    void log (level_t level, source_location_capture_cstring&& loc, ArgTypes&&... args) {
        // Enabled logging if global log level is not NONE.
        if constexpr (RS_GLOBAL_LOG_LEVEL > level_t::NONE && RS_GLOBAL_LOG_LEVEL < level_t::_N_LEVELS) {
            // Don't log anything above the global log level.
            if (level <= RS_GLOBAL_LOG_LEVEL) {
                // Format and print.
                if (auto s = format_string(loc.value(), args...)) {
                    RS_LOG_FUNC(RS_DEFAULT "[%s]\t%s\t" RS_GRAY "%s: %u" RS_DEFAULT "\n", get_level_string(level), s.get(), loc.source_location_short_file_name(), loc.source_location().line());
                }
                // Report formatting error.
                else {
                    auto buffer = std::unique_ptr<char[]>(new char[512]);
                    if (0 != strerror_s(buffer.get(), 512, errno)) {
                        memcpy(buffer.get(), "nullptr", 8);
                    }
                    RS_LOG_FUNC(RS_DEFAULT "[%s] An error occurred formatting a string: %s\n", get_level_string(level_t::ERROR), buffer.get());
                }
            }
        }
    }
} // rs::log::_private
} // rs::log
} // rs

#undef ERROR_COLOR
#undef WARN_COLOR
#undef INFO_COLOR
#undef TRACE_COLOR
#pragma pop_macro("ERROR_COLOR")
#pragma pop_macro("WARN_COLOR")
#pragma pop_macro("INFO_COLOR")
#pragma pop_macro("TRACE_COLOR")

#endif // RS_LOG_PRIVATE_H
