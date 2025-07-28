#ifndef RS_SOURCE_LOCATION_CAPTURE_H
#define RS_SOURCE_LOCATION_CAPTURE_H

#include <source_location>
#include "rs_help.h"

namespace rs {
    namespace help {
        static consteval const char* consteval_get_shortname (const std::source_location& loc) {
            const size_t length = consteval_strlen(loc.file_name());
            const char* shortname = loc.file_name() + length;

            while (*shortname != '\\' && *shortname != '/') {
                shortname--;
            }
            shortname++;

            return shortname;
        }
    }

    template<typename T>
    struct source_location_capture {
        _NODISCARD_CTOR consteval source_location_capture (T value)
            : source_location_capture(std::move(value), std::source_location::current()) // capture current source file location
        {}

        // The original value which was captured.
        _NODISCARD constexpr std::remove_reference_t<T>& value (void) const noexcept {
            return m_value;
        }
        // The std::source_location.
        _NODISCARD constexpr const std::source_location& source_location (void) const noexcept {
            return m_loc;
        }
        // The std::source_location::file_name() sans parent path.
        _NODISCARD constexpr const char* source_location_short_file_name (void) const noexcept {
            return m_loc_short_file_name;;
        }

    private:
        _NODISCARD_CTOR consteval source_location_capture (T&& value, std::source_location&& loc)
            : m_value (std::move(value))
            , m_loc (std::move(loc))
            , m_loc_short_file_name (help::consteval_get_shortname(loc))
        {}

        T                           m_value;
        const std::source_location  m_loc;
        const char* const           m_loc_short_file_name;
    };


    using source_location_capture_cstring = source_location_capture<const char* const>;
} // rs

#endif // RS_SOURCE_LOCATION_CAPTURE_H
