#pragma once

#include <iostream>
#include <utility>
#include <vector>
#include <fstream>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <utility>
#include <vector>
#include <fstream>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <algorithm>
#include <cmath>

class RGB {
public:
    RGB() = default;

    RGB(unsigned char R_, unsigned char G_, unsigned char B_);

    RGB(long double R_, long double G_, long double B_);


    RGB &operator*=(long double t);

    RGB operator*(long double t);

    RGB &operator+=(RGB t);

    RGB &operator-=(RGB t);

    RGB operator-() const;

    void normalize();

    long double R = 0;
    long double G = 0;
    long double B = 0;
};


class Image {
public:
    Image() = default;

    Image(std::vector<std::vector<RGB>> pixs) : height(pixs.size()), width(pixs[0].size()), pixels(std::move(pixs)) {}

    void Read(std::istream &input);

    void Write(std::ostream &output);

    size_t GetHeight() const;

    size_t GetWidth() const;

    RGB GetPixel(size_t x, size_t y) const;

    void AddToPixel(size_t x, size_t y, RGB newPixel);

    void SetPixel(size_t x, size_t y, RGB newPixel);

    void NormalizePixel(size_t x, size_t y);

private:
    size_t height = 0;
    size_t width = 0;
    size_t padding = 0;
    std::vector<std::vector<RGB>> pixels;
};





