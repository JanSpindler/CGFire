////
////Created by vincent on 26.05.2021.
////
//#include "util/TextureLoading.h"
//
//namespace util {
//    float *
//    load_texture_data(std::string filename, int *width, int *height) {
//        int channels;
//        unsigned char *file_data = stbi_load(filename.c_str(), width, height, &channels, 3);
//
//        int w = *width;
//        int h = *height;
//
//        float *data = new float[4 * w * h];
//        for (int j = 0; j < h; ++j) {
//            for (int i = 0; i < w; ++i) {
//                data[j * w * 4 + i * 4 + 0] = static_cast<float>(file_data[j * w * 3 + i * 3 + 0]) / 255;
//                data[j * w * 4 + i * 4 + 1] = static_cast<float>(file_data[j * w * 3 + i * 3 + 1]) / 255;
//                data[j * w * 4 + i * 4 + 2] = static_cast<float>(file_data[j * w * 3 + i * 3 + 2]) / 255;
//                data[j * w * 4 + i * 4 + 3] = 1.f;
//            }
//        }
//
//        delete[] file_data;
//
//        return data;
//    }
//
//    float *
//    random_texture_data(int width, int height) {
//        float *data = new float[4 * width * height];
//        std::mt19937 rng;
//        std::uniform_real_distribution<float> dist;
//        for (int j = 0; j < height; ++j) {
//            for (int i = 0; i < width; ++i) {
//                data[j * width * 4 + i * 4 + 0] = dist(rng);
//                data[j * width * 4 + i * 4 + 1] = dist(rng);
//                data[j * width * 4 + i * 4 + 2] = dist(rng);
//                data[j * width * 4 + i * 4 + 3] = 1.f;
//            }
//        }
//
//        return data;
//    }
//
//    float *
//    gradient_texture_data(int width, int height) {
//        float *data = new float[4 * width * height];
//        for (int j = 0; j < height; ++j) {
//            float red = static_cast<float>(j) / (height - 1);
//            for (int i = 0; i < width; ++i) {
//                float green = static_cast<float>(i) / (width - 1);
//                data[j * width * 4 + i * 4 + 0] = red;
//                data[j * width * 4 + i * 4 + 1] = green;
//                data[j * width * 4 + i * 4 + 2] = 0.f;
//                data[j * width * 4 + i * 4 + 3] = 1.f;
//            }
//        }
//
//        return data;
//    }
//
//    unsigned int
//    create_texture_rgba32f(int width, int height, float *data) {
//        unsigned int handle;
//        glCreateTextures(GL_TEXTURE_2D, 1, &handle);
//        glTextureStorage2D(handle, 1, GL_RGBA32F, width, height);
//        glTextureSubImage2D(handle, 0, 0, 0, width, height, GL_RGBA, GL_FLOAT, data);
//
//        return handle;
//    }
//
//    void
//    set_texture_filter_mode(unsigned int texture, GLenum mode) {
//        glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, mode);
//    }
//
//    void
//    set_texture_wrap_mode(unsigned int texture, GLenum mode) {
//        glTextureParameteri(texture, GL_TEXTURE_WRAP_S, mode);
//        glTextureParameteri(texture, GL_TEXTURE_WRAP_T, mode);
//    }
//}