#ifndef RS_LOG_PRIVATE_H
#define RS_LOG_PRIVATE_H

#include <cstdint>
#include <type_traits>
#include <memory>
#include <source_location>
#include <filesystem>
#include "rs_log_level.h"
#include "../ansi/rs_ansi.h"


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
    #define ERROR_COLOR     RS_RGB(239,71,111)
    #define WARN_COLOR      RS_RGB(255,209,102)
    #define INFO_COLOR      RS_RGB(17,138,178)
    #define TRACE_COLOR     RS_RGB(7,59,76)
#else
    #define ERROR_COLOR     ""
    #define WARN_COLOR      ""
    #define INFO_COLOR      ""
    #define TRACE_COLOR     ""
#endif


namespace rs {
namespace log {
    static consteval size_t consteval_strlen (const char *string) {
        size_t len = 0;
        while (*string++) {
            len++;
        }
        return len;
    }

    static consteval const char* consteval_get_shortname (const std::source_location& loc) {
        const auto l = consteval_strlen(loc.file_name());
        const char* shortname = loc.file_name() + l;

        while (*shortname != '\\' && *shortname != '/') {
            shortname--;
        }
        shortname++;

        return shortname;
    }

    // Based on https://stackoverflow.com/a/66402319/19514931
    struct format_string_auto_location {
        _NODISCARD_CTOR consteval format_string_auto_location (const char* const format, const std::source_location &loc = std::source_location::current())
            : m_format (format)
            , m_loc (loc)
            , m_file_name_short (consteval_get_shortname(loc))
        {}

        _NODISCARD constexpr auto format (void) const noexcept {
            return m_format;
        }
        _NODISCARD constexpr auto source_location (void) const noexcept {
            return m_loc;
        }
        _NODISCARD constexpr auto short_file_name (void) const noexcept {
            return m_file_name_short;
        }

    private:
        const char* const           m_format;
        const std::source_location  m_loc;
        const char*                 m_file_name_short;
    };

namespace _private {
    static const char* STRINGS[] = {
        ERROR_COLOR "ERROR" RS_DEFAULT,
        WARN_COLOR  "WARN"  RS_DEFAULT,
        INFO_COLOR  "INFO"  RS_DEFAULT,
        TRACE_COLOR "TRACE" RS_DEFAULT
    };

    constexpr const char *const get_level_string (level_t level) {
        return STRINGS[static_cast<std::underlying_type_t<level_t>>(level) - 1];
    }


    template<typename... ArgTypes>
    void log (level_t level, format_string_auto_location&& fmt_loc, ArgTypes&&... args); // format_string needs to know about this

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
                log(level_t::ERROR, "Invalid format string");
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
    void log (level_t level, format_string_auto_location&& fmt_loc, ArgTypes&&... args) {
        // Enabled logging if global log level is not NONE.
        if constexpr (RS_GLOBAL_LOG_LEVEL > level_t::NONE && RS_GLOBAL_LOG_LEVEL < level_t::_N_LEVELS) {
            // Don't log anything above the global log level.
            if (level <= RS_GLOBAL_LOG_LEVEL) {
                // Format and print.
                if (auto s = format_string(fmt_loc.format(), args...)) {
                    RS_LOG_FUNC(RS_DEFAULT "[%s] %s -- line %u, %s\n", get_level_string(level), s.get(), fmt_loc.source_location().line(), fmt_loc.short_file_name());
                }
                // Report formatting error
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
