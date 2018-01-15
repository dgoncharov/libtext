## Shell style text parsing in c++.


- No dependencies other than the standard library.
- Does not allocate memory for parsing.
- Input validation.
- Type conversion from const char* to the target type.
- Parsing in a single pass.

##### Examples
libtext::read is designed after shell read. Compare

```
$ IFS=: read host port <<< example.com:80

std::string host;
uint16_t port;
libtext::read("example.com:80", ":", &host, &port);
assert("example.com" == host);
assert(80 == port);
```

In the following example 0 is passes as the first output argument and
"example.com" is therefore skipped.

```
libtext::read("example.com:80", ":", 0, &port);
assert(80 == port);
```

This example shows how to parse contents of /etc/net/udp line by line.
```
void read_udp(const char* input)
{
    input = libtext::nextline(input); // Skip the header.
    std::string ip, addr, m, d;
    int type, flags;
    while (*input) {
        input = libtext::read(input, " ", &ip, &type, &flags, &addr, &m, &d);
        if (!input)
            break;
        // libtext::nextline allows to append new fields after the last field.
        // Old binary will still be able to parse.
        input = libtext::nextline(input);
    }
}
```

##### Differences from shell read.

- Multicharacter field delimiter.

    Shell read uses any character in IFS as a field delimiter.
    libtext::read uses the full contents of sep as a field delimiter.
    ```
    libtext::read("example.com~|~80", "~|~", &host, &port);
    ```
    It is necessary to use the full contents of sep as a delimiter to parse data
    that may contain any single character, for example names of securities.
    Also, ability to use any character in IFS as a field delimiter prevents input validation.
    E.g. it is convenient to use IFS=@: to parse input of the format user@host:path.
    However, using IFS=@: accepts invalid input like user@host@path,
    user:host@path or user:host:path.


- Extra fields.

    When input has more fields than there are output arguments shell read assigns
    the rest of the line to the last output argument.

    When input has more fields than there are output arguments libtext::read still
    assigns one field to one output argument and returns the address of the next
    field. This allows to use different delimiters for parsing different parts of
    a line.

    ```
    std::string user, host, path;
    const char* input = "kthompson@example.com:~/bin";
    const char* s = libtext::read(input, "@", &user);
    assert(s);
    assert(strstr(input, "example") == s);
    assert(user == "kthompson");
    s = libtext::read(s, ":", &host, &path);
    assert(s);
    assert(host == "example.com");
    assert(path == "~/bin");
    ```

- Input validation.

    Shell read can leave an argument uninitialized if there is no field to
    initialize the argument.
    libtext::read fails parsing if any field cannot be initialized.

    ```
    $ IFS=: read host port <<< example.com
    const char* s = libtext::read("example.com", ":", &host, &port);
    assert(!s);
    ```

    libtext::read fails parsing if the value of a field cannot be converted to the
    type of the output argument.

    ```
    uint8_t u8;
    const char* s = libtext::read("1024", ":", &u8);
    assert(!s);
    ```

##### To build and install
```
$ configure
$ make
$ make install
```
##### To enable std::string_view overload with g++
```
$ make CXXFLAGS=-std=c++17
```

##### To build example program that reads /etc/fstab
```
$ make fstab
```

##### To build the test suite
```
$ make check
```

##### Also see
[libtext(3)](doc/libtext.html)

Copyright (C) 2017 Dmitry Goncharov.

Distributed under the BSD license.
