//
// Copyright (c) Sharon Fox (sharon at sharonfox dot dev)
//
// Distributed under the MIT License. (See accompanying file LICENSE)
//

#pragma once

#include <iostream>
#include <fstream>

/**
** Exceptions:
** -191 = string too long
** -192 = data variable invalid
** -193 = Stream read out of bounds
**/

class file_write
{
public:
    file_write(std::ofstream & input)
        : file(input)
    {
    };
    ~file_write()
    {
        file.close();
    }
    std::ofstream & file;

    void write_byte(char value)
    {
        file.write(&value, 1);
    }

    void write_bytes(char * value, int sz)
    {
        file.write(value, sz);
    }

    void write_int16(int16_t value)
    {
        char sc[2] = {};
        int16_t * sp = (int16_t *)sc;
        *sp = value;
        file.write(sc, 2);
    }

    void write_uint16(uint16_t value)
    {
        char sc[2] = {};
        uint16_t * sp = (uint16_t *)sc;
        *sp = value;
        file.write(sc, 2);
    }

    void write_int(int32_t value)
    {
        char ic[4] = {};
        int32_t * ip = (int32_t *)ic;
        *ip = value;
        file.write(ic, 4);
    }

    void write_uint(uint32_t value)
    {
        char ic[4] = {};
        uint32_t * ip = (uint32_t *)ic;
        *ip = value;
        file.write(ic, 4);
    }

    void write_int64(int64_t value)
    {
        char ic[8] = {};
        int64_t * ip = (int64_t *)ic;
        *ip = value;
        file.write(ic, 8);
    }

    void write_uint64(uint64_t value)
    {
        char ic[8] = {};
        uint64_t * ip = (uint64_t *)ic;
        *ip = value;
        file.write(ic, 8);
    }

    void write_float(float value)
    {
        char ic[4] = {};
        memcpy(ic, (char *)&value, 4);
        file.write(ic, 4);
    }

    void write_double(double value)
    {
        char ic[8] = {};
        memcpy(ic, (char *)&value, 8);
        file.write(ic, 8);
    }

    void write_string(std::string value)
    {
        write_int(uint32_t(value.length()));
        file.write(value.c_str(), value.length());
    }
};

#define verify_data if (data == 0) throw -192
#define verify_size(a) if (position+a > size) { std::size_t oldsize = size; if ((std::size_t)(float(size)*1.15) < oldsize+a) { size = (std::size_t)(float(size)*1.15)+a; } else { size = (std::size_t)(float(size)*1.15); } char * temp = new char[size]; memset(temp, 0, size); memcpy(temp, data, oldsize); delete[] data; data = temp; }
#define verify_length(a) if (position+a > size) { throw -193; }
class stream_write
{
public:
    stream_write(std::size_t initsize = 100)
    {
        size = initsize;
        position = 0;
        data = new char[size];
        memset(data, 0, size);
    }

    ~stream_write()
    {
        if (data)
            delete[] data;
    }

    char * data;
    std::size_t position;
    std::size_t size;

    void clear()
    {
        if (data)
            delete[] data;
        size = 100;
        data = new char[size];
        position = 0;
    }

    std::size_t write_size()
    {
        verify_data;
        memcpy(data, &position, 2);
        return position;
    }

    void write_bytes(const char * value, int sz)
    {
        verify_data;
        verify_size(sz);
        memcpy(data + position, value, sz);
        position += sz;
    }

    void write_array(const char * value, int sz)
    {
        verify_data;
        write_byte(sz);
        verify_size(sz);
        memcpy(data + position, value, sz);
        position += sz;
    }

    void write_byte(const char value)
    {
        verify_data;
        verify_size(1);
        memcpy(data + position, &value, 1);
        position++;
    }

    void write_int16(const int16_t value)
    {
        verify_data;
        verify_size(2);
        memcpy(data + position, &value, 2);
        position += 2;
    }

    void write_uint16(const uint16_t value)
    {
        verify_data;
        verify_size(2);
        memcpy(data + position, &value, 2);
        position += 2;
    }

    void write_int32(const int32_t value)
    {
        verify_data;
        verify_size(4);
        memcpy(data + position, &value, 4);
        position += 4;
    }

    void write_uint32(const uint32_t value)
    {
        verify_data;
        verify_size(4);
        memcpy(data + position, &value, 4);
        position += 4;
    }

    template<typename T>
    void write_enum(const T value)
    {
        write_int32(static_cast<int32_t>(value));
    }

    template<typename T>
    void write_enum16(T value)
    {
        write_uint16(static_cast<int16_t>(value));
    }

    void write_int64(const int64_t value)
    {
        verify_data;
        verify_size(8);
        memcpy(data + position, &value, 8);
        position += 8;
    }

    void write_uint64(const uint64_t value)
    {
        verify_data;
        verify_size(8);
        memcpy(data + position, &value, 8);
        position += 8;
    }

    void write_float(const float value)
    {
        verify_data;
        verify_size(4);
        memcpy(data + position, &value, 4);
        position += 4;
    }

    void write_double(const double value)
    {
        verify_data;
        verify_size(8);
        memcpy(data + position, &value, 8);
        position += 8;
    }

    void write_string(const std::string & value)
    {
        verify_data;
        write_uint16(char(value.length()));
        verify_size(value.length());
        memcpy(data + position, value.c_str(), value.length());
        position += value.length();
    };

    void write_string(const std::string & value, std::size_t length)
    {
        verify_data;
        verify_size(length);
        if (value.length() > length)
            memcpy(data + position, value.c_str(), length);
        else
            memcpy(data + position, value.c_str(), value.length());
        position += length;
    };
};

class file_read
{
public:
    file_read(char * input, std::size_t in)
        : position(0)
        , data(input)
        , size(in)
    {
    }

    ~file_read()
    {
    }

    char * data;
    std::size_t position;
    std::size_t size;

    char read_byte()
    {
        verify_data;
        verify_length(1);
        char * cp = (char *)(data + position);
        position++;
        return *cp;
    }

    void read_bytes(char * p, std::size_t sz)
    {
        verify_data;
        verify_length(sz);
        memcpy(p, (char *)(data + position), sz);
        position += sz;
    }

    int16_t read_int16()
    {
        verify_data;
        verify_length(2);
        int16_t * sp = (int16_t *)(data + position);
        position += 2;
        return *sp;
    }

    uint16_t read_uint16()
    {
        verify_data;
        verify_length(2);
        uint16_t * sp = (uint16_t *)(data + position);
        position += 2;
        return *sp;
    }

    int32_t read_int32()
    {
        verify_data;
        verify_length(4);
        int32_t * ip = (int32_t *)(data + position);
        position += 4;
        return *ip;
    }

    uint32_t read_uint32()
    {
        verify_data;
        verify_length(4);
        uint32_t * ip = (uint32_t *)(data + position);
        position += 4;
        return *ip;
    }

    int64_t read_int64()
    {
        verify_data;
        verify_length(8);
        int64_t * ip = (int64_t *)(data + position);
        position += 8;
        return *ip;
    }

    uint64_t read_uint64()
    {
        verify_data;
        verify_length(8);
        uint64_t * ip = (uint64_t *)(data + position);
        position += 8;
        return *ip;
    }

    float read_float()
    {
        verify_data;
        verify_length(4);
        float * fp = (float *)(data + position);
        position += 4;
        return *fp;
    }

    double read_double()
    {
        verify_data;
        verify_length(4);
        double * dp = (double *)(data + position);
        position += 4;
        return *dp;
    }

    std::string read_string()
    {
        std::size_t sz = read_int16();
        if (sz == 0)
        {
            return "";
        }
        verify_data;
        verify_length(sz);
        char * c = new char[sz + 1];
        memset(c, 0, sz + 1);
        memcpy(c, data + position, sz);
        position += sz;
        std::string str;
        str = c;
        delete[] c;
        return str;
    }
};
class stream_read
{
public:
    stream_read(const char * input, std::size_t in)
        : position(0)
        , size(in)
    {
        data = new char[size];
        memcpy(data, input, size);
    }

    ~stream_read()
    {
        if (data) delete[] data;
    }

    char * data{};
    std::size_t position{};
    std::size_t size{};

    void reset_pos()
    {
        position = 0;
    }

    std::size_t read_size()
    {
        verify_data;
        return *(uint32_t *)(data);
    }

    char * read_array()
    {
        verify_data;
        int sz = read_int16();
        verify_length(sz);
        char * temp = new char[sz];
        memcpy(temp, (char *)(data + position), sz);
        position += sz;
        return temp;
    }

    void read_bytes(char * p, std::size_t sz)
    {
        verify_data;
        verify_length(sz);
        memcpy(p, (char *)(data + position), sz);
        position += sz;
    }

    char read_byte()
    {
        verify_data;
        verify_length(1);
        char cp = (char)*(data + position);
        position++;
        return cp;
    }

    int16_t read_int16()
    {
        verify_data;
        verify_length(2);
        int16_t * sp = (int16_t *)(data + position);
        position += 2;
        return *sp;
    }

    uint16_t read_uint16()
    {
        verify_data;
        verify_length(2);
        uint16_t * sp = (uint16_t *)(data + position);
        position += 2;
        return *sp;
    }

    int32_t read_int32()
    {
        verify_data;
        verify_length(4);
        int32_t * ip = (int32_t *)(data + position);
        position += 4;
        return *ip;
    }

    uint32_t read_uint32()
    {
        verify_data;
        verify_length(4);
        uint32_t * ip = (uint32_t *)(data + position);
        position += 4;
        return *ip;
    }

    int64_t read_int64()
    {
        verify_data;
        verify_length(8);
        int64_t * ip = (int64_t *)(data + position);
        position += 8;
        return *ip;
    }

    uint64_t read_uint64()
    {
        verify_data;
        verify_length(8);
        uint64_t * ip = (uint64_t *)(data + position);
        position += 8;
        return *ip;
    }

    float read_float()
    {
        verify_data;
        verify_length(4);
        float * fp = (float *)(data + position);
        position += 4;
        return *fp;
    }

    double read_double()
    {
        verify_data;
        verify_length(8);
        double * dp = (double *)(data + position);
        position += 8;
        return *dp;
    }

    struct return_result
    {
        return_result(uint32_t v) : v(v) {}
        uint32_t v;
        template <typename T>
        operator T() const
        {
            return static_cast<T>(v);
        }
    };

    return_result read_enum()
    {
        return static_cast<return_result>(read_int32());
    }

    std::string read_string()
    {
        std::size_t sz = read_uint16();
        if (sz == 0)
        {
            return "";
        }
        verify_data;
        verify_length(sz);
        char * c = new char[sz + 1];
        memset(c, 0, sz + 1);
        memcpy(c, data + position, sz);
        position += sz;
        std::string str;
        str = c;
        delete[] c;
        return str;
    }

    std::string read_string(std::size_t sz)
    {
        verify_data;
        verify_length(sz);
        char * c = new char[sz + 1];
        memset(c, 0, sz + 1);
        memcpy(c, data + position, sz);
        position += sz;
        std::string str;
        str = c;
        delete[] c;
        return str;
    }
};
