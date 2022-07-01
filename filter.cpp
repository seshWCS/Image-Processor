#include "filter.h"
#include "image.h"

Corp::Corp(size_t h, size_t w) : newHeight(h), newWidth(w) {}

void Corp::Apply(Image &image) {
    size_t height = std::min(newHeight, image.GetHeight());
    size_t width = std::min(newWidth, image.GetWidth());
    std::vector<std::vector<RGB>> pix(height, std::vector<RGB>(width));
    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {
            pix[i][j] = image.GetPixel(i, j);
        }
    }
    image = Image(pix);
}

void Grayscale::Apply(Image &image) {
    for (size_t i = 0; i < image.GetHeight(); ++i) {
        for (size_t j = 0; j < image.GetWidth(); ++j) {
            RGB current_pixel = image.GetPixel(i, j);
            long double newColor = current_pixel.R * 0.299 + current_pixel.G * 0.587 + current_pixel.B * 0.114;
            current_pixel.R = newColor, current_pixel.G = newColor, current_pixel.B = newColor;
            image.SetPixel(i, j, current_pixel);
        }
    }
}

void Negative::Apply(Image &image) {
    for (size_t i = 0; i < image.GetHeight(); ++i) {
        for (size_t j = 0; j < image.GetWidth(); ++j) {
            RGB current_pixel = image.GetPixel(i, j);
            current_pixel.R = 1 - current_pixel.R;
            current_pixel.G = 1 - current_pixel.G;
            current_pixel.B = 1 - current_pixel.B;
            image.SetPixel(i, j, current_pixel);
        }
    }
}

void Sharpening::Apply(Image &image) {
    Image newImage(image);

    for (size_t i = 0; i < image.GetHeight(); ++i) {
        for (size_t j = 0; j < image.GetWidth(); ++j) {
            newImage.SetPixel(i, j, image.GetPixel(i, j) * 5.0);
            newImage.AddToPixel(i, j, -image.GetPixel(i - 1, j));
            newImage.AddToPixel(i, j, -image.GetPixel(i + 1, j));
            newImage.AddToPixel(i, j, -image.GetPixel(i, j - 1));
            newImage.AddToPixel(i, j, -image.GetPixel(i, j + 1));
            newImage.NormalizePixel(i, j);
        }
    }
    image = newImage;
}

EdgeDetection::EdgeDetection(long double threshold) : threshold(threshold) {
}

void EdgeDetection::Apply(Image &image) {
    Grayscale::Apply(image);

    Image newImage(image);

    for (size_t i = 0; i < image.GetHeight(); ++i) {
        for (size_t j = 0; j < image.GetWidth(); ++j) {
            newImage.SetPixel(i, j, image.GetPixel(i, j) * 4.0);
            newImage.AddToPixel(i, j, -image.GetPixel(i - 1, j));
            newImage.AddToPixel(i, j, -image.GetPixel(i + 1, j));
            newImage.AddToPixel(i, j, -image.GetPixel(i, j - 1));
            newImage.AddToPixel(i, j, -image.GetPixel(i, j + 1));
            newImage.NormalizePixel(i, j);

            if (newImage.GetPixel(i, j).R > threshold) {
                newImage.SetPixel(i, j, RGB(1.L, 1.L, 1.L));
            } else {
                newImage.SetPixel(i, j, RGB(0.L, 0.L, 0.L));
            }
        }
    }
    image = newImage;
}

GaussianBlur::GaussianBlur(int sigma) : sigma(sigma) {
}

void GaussianBlur::MakeGaussianCount(std::vector<long double> &coefficients, long double &coefficients_sum) {
    for (int i = -RADIUS; i < RADIUS + 1; ++i) {
        coefficients[i + RADIUS] = std::exp(-(i * i + 0.0) / (2.0 * sigma * sigma)) /
                                   std::sqrt(2.0 * std::numbers::pi_v<long double> * sigma * sigma);
        coefficients_sum += coefficients[i + RADIUS];
    }
}

void GaussianBlur::Apply(Image &image) {
    long double coefficients_sum = 0.l;
    std::vector<long double> coefficients;
    coefficients.assign(2 * RADIUS + 1, 0.l);
    MakeGaussianCount(coefficients, coefficients_sum);

    Image newImage(std::vector<std::vector<RGB>>(image.GetHeight(), std::vector<RGB>(image.GetWidth())));
    for (size_t i = 0; i < image.GetHeight(); ++i) {
        for (size_t j = 0; j < image.GetWidth(); ++j) {
            for (size_t i_ = std::max(static_cast<int>(i) - RADIUS, 0);
                 i_ < std::min(i + RADIUS + 1, image.GetHeight()); ++i_) {
                newImage.AddToPixel(i, j, image.GetPixel(i_, j) *
                                          (coefficients[std::abs(static_cast<int>(i_) - static_cast<int>(i))] /
                                           coefficients_sum));
                newImage.NormalizePixel(i, j);
            }
        }
    }
    image = Image(std::vector<std::vector<RGB>>(image.GetHeight(), std::vector<RGB>(image.GetWidth())));
    for (size_t i = 0; i < image.GetHeight(); ++i) {
        for (size_t j = 0; j < image.GetWidth(); ++j) {
            for (size_t j_ = std::max(static_cast<int>(j) - RADIUS, 0);
                 j_ < std::min(j + RADIUS + 1, image.GetWidth()); ++j_) {
                image.AddToPixel(i, j, newImage.GetPixel(i, j_) *
                                       (coefficients[std::abs(static_cast<int>(j_) - static_cast<int>(j))] /
                                        coefficients_sum));
                image.NormalizePixel(i, j);
            }
        }
    }
}

Brightness::Brightness(int percentage) : percentage((percentage + 0.l) / 100.l) {
}

void Brightness::Apply(Image &image) {
    for (size_t i = 0; i < image.GetHeight(); ++i) {
        for (size_t j = 0; j < image.GetWidth(); ++j) {
            RGB current_pixel = image.GetPixel(i, j);
            long double colorMultiplier = percentage + 1.l;
            current_pixel *= colorMultiplier;
            image.SetPixel(i, j, current_pixel);
            image.NormalizePixel(i, j);
        }
    }
}
