#include "image.h"

const int HEADER_SIZE = 14;
const int INFO_HEADER_SIZE = 40;

void Image::Read(std::istream &input) {
    unsigned char header[HEADER_SIZE];
    try {
        input.read(reinterpret_cast<char *>(&header), sizeof(header));
    } catch (const std::exception &e) {
        std::cout << "Incorrect input file\n";
        exit(0);
    }
    if (header[0] != 'B' || header[1] != 'M') {
        std::cout << "Input file is not bmp\n";
    }
    //int info_size = header[2];
   /* if (info_size != INFO_HEADER_SIZE + HEADER_SIZE) {
        std::cout << "Invalid input file\n";
        exit(0);
    }*/
    unsigned char info_header[INFO_HEADER_SIZE];

    try {
        input.read(reinterpret_cast<char *>(&info_header), sizeof(info_header));
    } catch (const std::exception &e) {
        std::cout << "Invalid input file\n";
        exit(0);
    }
    width = info_header[4] + (info_header[5] << 8) + (info_header[6] << 16) + (info_header[7] << 24);
    height = info_header[8] + (info_header[9] << 8) + (info_header[10] << 16) + (info_header[11] << 24);
    padding = (4 - width * 3 % 4) % 4;
    unsigned char buffer[3] = {0, 0, 0};
    pixels.assign(height, std::vector<RGB>(width));
    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {
            try {
                input.read(reinterpret_cast<char *>(&buffer), sizeof(buffer));
            } catch (const std::exception &e) {
                std::cout << "Invalid input file\n";
                exit(0);
            }
            pixels[i][j] = RGB(buffer[2], buffer[1], buffer[0]);
        }
        try {
            input.seekg(static_cast<int>(padding), std::ios_base::cur);
        } catch (const std::exception &e) {
            std::cout << "Invalid input file\n";
            exit(0);
        }
    }
    std::reverse(pixels.begin(), pixels.end());
}

void Image::Write(std::ostream &output) {
    std::reverse(pixels.begin(), pixels.end());
    padding = (4 - width * 3 % 4) % 4;
    const int file_size = HEADER_SIZE + INFO_HEADER_SIZE + (3 * width + padding) * height;
    unsigned char header[HEADER_SIZE];
    memset(header, 0, sizeof(header));
    header[0] = 'B';
    header[1] = 'M';
    header[2] = file_size;
    header[3] = file_size >> 8;
    header[4] = file_size >> 16;
    header[5] = file_size >> 24;
    header[10] = HEADER_SIZE + INFO_HEADER_SIZE;
    unsigned char info_header[INFO_HEADER_SIZE];
    memset(info_header, 0, sizeof(info_header));
    info_header[0] = INFO_HEADER_SIZE;
    info_header[4] = width;
    info_header[5] = width >> 8;
    info_header[6] = width >> 16;
    info_header[7] = width >> 24;
    info_header[8] = height;
    info_header[9] = height >> 8;
    info_header[10] = height >> 16;
    info_header[11] = height >> 24;
    info_header[12] = 1;
    info_header[HEADER_SIZE] = 24;
    output.write(reinterpret_cast<char *>(header), HEADER_SIZE);
    output.write(reinterpret_cast<char *>(info_header), INFO_HEADER_SIZE);
    unsigned char pass[3] = {0, 0, 0};
    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {
            unsigned char buffer[3] = {0, 0, 0};
            RGB cur_pix = pixels[i][j];
            buffer[0] = static_cast<unsigned char>(std::round(cur_pix.B * 255));
            buffer[1] = static_cast<unsigned char>(std::round(cur_pix.G * 255));
            buffer[2] = static_cast<unsigned char>(std::round(cur_pix.R * 255));
            output.write(reinterpret_cast<char *>(buffer), 3);
        }
        output.write(reinterpret_cast<char *>(pass), padding);
    }
}

size_t Image::GetHeight() const {
    return height;
}

size_t Image::GetWidth() const {
    return width;
}

RGB Image::GetPixel(size_t x, size_t y) const {
    if (x < height && y < width) {
        return pixels[x][y];
    } else {
        return {};
    }
}

void Image::SetPixel(size_t x, size_t y, RGB newPixel) {
    pixels[x][y] = newPixel;
}

void Image::AddToPixel(size_t x, size_t y, RGB newPixel) {
    pixels[x][y] += newPixel;
}

void Image::NormalizePixel(size_t x, size_t y) {
    pixels[x][y].normalize();
}

RGB::RGB(long double R_, long double G_, long double B_) : R(R_), G(G_), B(B_) {
}

RGB::RGB(unsigned char R_, unsigned char G_, unsigned char B_) : R((R_ + 0.0) / 255), G((G_ + 0.0) / 255),
                                                                 B((B_ + 0.0) / 255) {
}

RGB RGB::operator-() const {
    return {-R, -G, -B};
}

void RGB::normalize() {
    R = std::max(static_cast<long double>(0), R);
    R = std::min(static_cast<long double>(1), R);
    G = std::max(static_cast<long double>(0), G);
    G = std::min(static_cast<long double>(1), G);
    B = std::max(static_cast<long double>(0), B);
    B = std::min(static_cast<long double>(1), B);
}

RGB &RGB::operator-=(RGB t) {
    R -= t.R;
    G -= t.G;
    B -= t.B;
    return *this;
}

RGB &RGB::operator+=(RGB t) {
    R += t.R;
    G += t.G;
    B += t.B;
    return *this;
}

RGB RGB::operator*(long double t) {
    RGB ret = *this;
    ret.R *= t;
    ret.G *= t;
    ret.B *= t;
    return ret;
}

RGB &RGB::operator*=(long double t) {
    R *= t;
    G *= t;
    B *= t;
    return *this;
}