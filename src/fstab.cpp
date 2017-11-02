#include <libtext.h>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

struct fsent {
    std::string fs_spec;
    std::string fs_file;
    std::string fs_type;
    std::string fs_opts;
    int fs_freq;
    int fs_passno;
};

int main(int argc, char* argv[])
{
    using std::cerr;
    using std::endl;

    std::vector<char> buf;
    std::ifstream is("/etc/fstab");
    if (!is) {
        cerr << "cannot open /etc/fstab: " << strerror(errno) << endl;
        return EXIT_FAILURE;
    }
    is.seekg(0, is.end);
    buf.resize(is.tellg() + 1l);
    is.seekg(0, is.beg);
    is.read(&buf[0], buf.size() - 1);
    buf.back() = '\0';
    if (!is.good()) {
        cerr << "cannot read /etc/fstab: " << strerror(errno) << endl;
        return EXIT_FAILURE;
    }
    is.close();
    const char* input = &buf[0];

    size_t lineno = 1;
    std::vector<fsent> v;
    v.reserve(64);
    for (; *input; input = libtext::nextline(input), ++lineno) {
        // Skip empty lines and comments.
        const char* s = input;
        input += strspn(input, " \t\n");
        lineno += std::count(s, input, '\n');
        if ('#' == *input)
            continue;
        v.resize(v.size() + 1);
        fsent& m = v.back();
        s = input;
        input = libtext::read(input, " ", &m.fs_spec, &m.fs_file,
                        &m.fs_type, &m.fs_opts, &m.fs_freq, &m.fs_passno);
        if (!input) {
            cerr
                << "malformed line /etc/fstab:" << lineno << " "
                << libtext::oneline(s) << endl;
            return EXIT_FAILURE;
        }
    }
    for (size_t k = 0, len = v.size(); k < len; ++k)
        std::cout
            << v[k].fs_spec << " " << v[k].fs_file << " "
            << v[k].fs_type << " " << v[k].fs_opts << " "
            << v[k].fs_freq << " " << v[k].fs_passno
            << std::endl;
    return 0;
}