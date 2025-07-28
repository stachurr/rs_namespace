#ifndef RS_HELP_H
#define RS_HELP_H

namespace rs {
namespace help {
    static consteval size_t consteval_strlen (const char* string) {
        size_t len = 0;
        while (*string++) {
            len++;
        }
        return len;
    }

    template <size_t N>
    static consteval size_t constexpr_strlen (const char (&string_literal)[N]) {
        return N - 1;
    }


} // rs::help
} // rs

#endif // RS_HELP_H
