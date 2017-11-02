#ifndef LIBTEXT_INCLUDE_GUARD
#define LIBTEXT_INCLUDE_GUARD

#include <string>
#include <stdint.h>

namespace libtext {
const char* read(const char* input, const char* sep, std::string* result);
const char* read(const char* input, const char* sep, uint8_t* result);
const char* read(const char* input, const char* sep, uint16_t* result);
const char* read(const char* input, const char* sep, uint32_t* result);
const char* read(const char* input, const char* sep, uint64_t* result);
const char* read(const char* input, const char* sep, int8_t* result);
const char* read(const char* input, const char* sep, int16_t* result);
const char* read(const char* input, const char* sep, int32_t* result);
const char* read(const char* input, const char* sep, int64_t* result);
const char* read(const char* input, const char* sep, float* result);
const char* read(const char* input, const char* sep, double* result);
const char* read(const char* input, const char* sep, long double* result);
const char* read(const char* input, const char* sep, int result);
// This definition is not static to have the generated function be an extern
// weak symbol to have only one in the final binary, which reduces the image
// size.  Having this definition static'd result in the generated function be a
// static strong symbol with multiple of these in the final binary (for a
// combination of arguments).
template <class T, class... U>
const char* read(const char* input, const char* sep, T result,  U... u)
{
    const char* s = read(input, sep, result);
    if (!s || !*s || *s == '\n')
        return 0; // The number of arguments exceeds the number of fields.
    return read(s, sep, u...);
}
const char *nextline(const char* input);
std::string oneline(const char* input);
void strip(std::string* input, const char* reject);
char* strip(char* input, const char* reject);
} // libtext
#endif