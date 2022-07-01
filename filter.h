#pragma once

#include <vector>
#include <numbers>

class Image;

class Filter {
public:
    virtual void Apply(Image &image) = 0;
};

class Corp : public Filter {
private:
    size_t newHeight;
    size_t newWidth;
public:
    Corp(size_t h, size_t w);

    void Apply(Image &image) override;
};

class Grayscale : public Filter {
public:
    void Apply(Image &image) override;
};

class Negative : public Filter {
public:
    void Apply(Image &image) override;
};

class Sharpening : public Filter {
public:
    void Apply(Image &image) override;
};

class EdgeDetection : public Grayscale {
private:
    long double threshold;
public:
    explicit EdgeDetection(long double threshold);

    void Apply(Image &image) override;
};

class GaussianBlur : public Filter {
private:
    int sigma;

    void MakeGaussianCount(std::vector<long double> &coefficients, long double &sum_coefficients);

    static const int RADIUS = 10;
public:
    explicit GaussianBlur(int sigma);

    void Apply(Image &image) override;
};

class Brightness : public Filter {
private:
    long double percentage;
public:
    explicit Brightness(int percentage);

    void Apply(Image &image) override;
};