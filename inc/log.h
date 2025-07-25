#ifndef RS_NAMESPACE_LOG
#define RS_NAMESPACE_LOG

#include <cstdint>
#include <type_traits>
#include <memory>


#ifndef RS_LOG_FUNC
    #define RS_LOG_FUNC printf
#endif

#ifndef RS_LOG_ENABLE_COLOR
    #define RS_LOG_ENABLE_COLOR 0
#endif

#ifndef RS_GLOBAL_LOG_LEVEL
    #define RS_GLOBAL_LOG_LEVEL level_t::WARN
#endif

#pragma push_macro("ERROR_COLOR")
#pragma push_macro("WARN_COLOR")
#pragma push_macro("INFO_COLOR")
#pragma push_macro("TRACE_COLOR")
#pragma push_macro("DEFAULT_COLOR")
#if RS_LOG_ENABLE_COLOR
    #define ERROR_COLOR     RGB(239,71,111)
    #define WARN_COLOR      RGB(255,209,102)
    #define INFO_COLOR      RGB(17,138,178)
    #define TRACE_COLOR     RGB(7,59,76)
    #define DEFAULT_COLOR   DEFAULT
#else
    #define ERROR_COLOR     ""
    #define WARN_COLOR      ""
    #define INFO_COLOR      ""
    #define TRACE_COLOR     ""
    #define DEFAULT_COLOR   ""
#endif

namespace rs {
namespace log {

    enum class level_t : uint8_t {
        ERROR = 0,
        WARN,
        INFO,
        TRACE,
        NONE,

        _N_LEVELS
    };

    constexpr level_t global_log_level = RS_GLOBAL_LOG_LEVEL;

    namespace priv {
        constexpr const char *const get_level_string (level_t level) {
            constexpr const char* STRINGS[] = {
                ERROR_COLOR "ERROR" DEFAULT_COLOR,
                WARN_COLOR  "WARN"  DEFAULT_COLOR,
                INFO_COLOR  "INFO"  DEFAULT_COLOR,
                TRACE_COLOR "TRACE" DEFAULT_COLOR
            };
            return STRINGS[static_cast<std::underlying_type_t<level_t>>(level)];
        }

        template<size_t N, typename... ArgTypes>
        std::unique_ptr<char[]> string_format (const char (&&format)[N], ArgTypes... args) {
            auto s = std::unique_ptr<char[]>(nullptr);

            if constexpr (sizeof...(ArgTypes) > 0) {
                // Check format validity.
                // https://en.cppreference.com/w/cpp/io/c/fprintf
                const int size_req = 1 + std::snprintf(nullptr, 0, format, args...);
                if (size_req > 0) {
                    // Format it and write to string.
                    s.reset(new char[size_req]);
                    std::snprintf(s.get(), size_req, format, args...);
                }
            }
            else {
                // No formatting required. Maybe we can avoid copying somehow...
                s.reset(new char[N]);
                std::memcpy(s.get(), format, N);
            }

            return s;
        }

        template<size_t _, typename... ArgTypes>
        void log (level_t level, const char (&&format)[_], ArgTypes... args) {
            // If the global log level is NONE, do not log anything.
            if constexpr (global_log_level < level_t::NONE) {

                // Don't log anything above the global log level.
                if (level <= global_log_level) {
                    if (auto s = string_format(format, args...)) {
                        RS_LOG_FUNC(DEFAULT_COLOR "[%s] %s\n", get_level_string(level), s.get());
                    }
                    else {
                        auto buffer = std::unique_ptr<char[]>(new char[256]);
                        if (0 != strerror_s(buffer.get(), 256, errno)) {
                            memcpy(buffer.get(), "nullptr", 8);
                        }
                        RS_LOG_FUNC(DEFAULT_COLOR "[%s] An error occurred formatting a string: %s\n", get_level_string(level_t::ERROR), buffer.get());
                    }
                }
            }
        }
    } // rs::log::priv


    template<size_t _, typename... ArgTypes>
    std::false_type error (const char (&format)[_], ArgTypes... args) {
        priv::log(level_t::ERROR, std::move(format), args...);
        return std::false_type();
    }

    template<size_t _, typename... ArgTypes>
    void warn (const char (&format)[_], ArgTypes... args) {
        priv::log(level_t::WARN, std::move(format), args...);
    }

    template<size_t _, typename... ArgTypes>
    void info (const char (&format)[_], ArgTypes... args) {
        priv::log(level_t::INFO, std::move(format), args...);
    }

    template<size_t _, typename... ArgTypes>
    void trace (const char (&format)[_], ArgTypes... args) {
        priv::log(level_t::TRACE, std::move(format), args...);
    }
} // rs::log
} // rs

#undef ERROR_COLOR
#undef WARN_COLOR
#undef INFO_COLOR
#undef TRACE_COLOR
#undef DEFAULT_COLOR
#pragma pop_macro("ERROR_COLOR")
#pragma pop_macro("WARN_COLOR")
#pragma pop_macro("INFO_COLOR")
#pragma pop_macro("TRACE_COLOR")
#pragma pop_macro("DEFAULT_COLOR")

#endif // RS_NAMESPACE_LOG
