libtext(3)                       User commands                      libtext(3)



[1mNAME[0m
       libtext - split a line into fields.


[1mSYNOPSIS[0m
       #include <libtext/libtext.h>

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
       template <class T, class... A>
       const char* read(const char* input, const char* sep, T result, const A&... a);
       const char* nextline(const char* input);
       std::string oneline(const char* input);


[1mDESCRIPTION[0m
       read  reads  a  line from input, validates that the line is well formed
       and splits the line to a sequence of fields separated by  the  contents
       of  sep.  The arguments that follow sep are all output arguments.   The
       first field is assigned to the output argument that  follows  sep.   If
       the  variadic overload is used then the second field is assigned to the
       next output argument, etc.  read converts the value of the  read  field
       from  const char* to the type of the output argument.  The line is well
       formed when the line is a sequence of fields separated by the  contents
       of  sep and the line begins and ends with a field and the value of each
       field can be converted to the type of the related output  argument  and
       the  line has enough fields to initialize all output arguments.  If the
       line has fewer fields than there are output arguments  then  read  ini‐
       tializes  as  many  output arguments as there are fields (starting from
       the leftmost) and keeps the subsequent  output  arguments  intact.   An
       output argument can be 0 or an address of a variable of one of the fol‐
       lowing types int8_t,  int16_t,  int32_t,  int64_t,  uint8_t,  uint16_t,
       uint32_t,  uint64_t,  float,  double,  long double, std::string.  If an
       output argument has value 0 then  read  does  not  assign  the  related
       field.   E.g.   std::string  host;  uint16_t port; libtext::read("exam‐
       ple.com:80",  ":",  &host,  &port);  assert(host   ==   "example.com");
       assert(port  == 80); libtext::read("example.com:80", ":", 0, &port); //
       "example.com" is not assinged to any output argument.   assert(port  ==
       80);  Passing  any  integer other than 0 as value of an output argument
       causes undefined behavior.

       While reading a field read skips trailing and leading spaces and  hori‐
       zontal tabs of arbitrary length. While matching a char in sep against a
       char in input read considers space equal horizontal tab.  It is  neces‐
       sary  to  consider  space  equal  tab  to parse hand written files like
       /etc/fstab where space and tab are used interchangeably.


       nextline finds the address of the character immediately  following  the
       first newline character in the input.

       oneline  builds  a  copy of a substring of the input from the beginning
       until the first newline character.


[1mRETURN VALUE[0m
       If a well formed line has more fields than there are  output  arguments
       then  read  returns  the address of the field immediately following the
       rightmost read field. If a well formed line  has  the  same  number  of
       fields  as  there are output arguments then read returns the address of
       the character that terminates the line (a new line character or a  null
       terminator).  If the line is malformed read returns 0.

       nextline returns the address of the character immediately following the
       first newline character in the input. If there is no newline  character
       in the input then nextline returns the address of the null terminator.

       oneline  returns  a copy of a substring of the input from the beginning
       until the first newline character. If there is no newline character  in
       input the one line return a copy of input.


[1mEXAMPLE[0m
       This  program  shows  how to use libtext::read to populate an in memory
       data structure holding a copy of /etc/fstab contents.

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
               if ('#' == *input || '\0' == *input)
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

[1mAUTHORS[0m
       Dmitry Goncharov <dgoncharov@users.sf.net>


[1mCOPYRIGHT[0m
       Copyright (C) 2017 Dmitry Goncharov.
       Distributed under the BSD license.


[1mHOMEPAGE[0m
       https://github.com/dgoncharov/libtext


[1mSEE ALSO[0m
       libtext::read is designed after shell read. Compare
       $ IFS=: read host port <<< "example.com:80"

       std::string host;
       uint16_t port;
       libtext::read("example.com:80", ":", &host, &port);

       Shell Command Language, read.



libtext 1.0.0                     17 May 2017                       libtext(3)
