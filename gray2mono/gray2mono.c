#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp.h"

// 二值化函数
void binarize(unsigned char *data, int width, int height, int threshold, int windowSize)
{
    int padding = (4 - (width % 4)) % 4;
    int rowSize = width + padding;

    // 创建临时缓冲区 (考虑补位)
    unsigned char *tempData = (unsigned char *)malloc(rowSize * height);
    if (!tempData)
    {
        printf("Memory allocation failed for tempData.\n");
        return;
    }
    memcpy(tempData, data, rowSize * height);

    int halfWindow = windowSize / 2;

    // 对每个像素进行窗口均值计算
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int sum = 0;
            int count = 0;

            // 计算窗口内像素平均值
            for (int wy = -halfWindow; wy <= halfWindow; wy++)
            {
                for (int wx = -halfWindow; wx <= halfWindow; wx++)
                {
                    int ny = y + wy;
                    int nx = x + wx;

                    if (ny >= 0 && ny < height && nx >= 0 && nx < width)
                    {
                        sum += tempData[ny * rowSize + nx];
                        count++;
                    }
                }
            }

            // 计算平均值并二值化
            int average = sum / count;
            data[y * rowSize + x] = (average > threshold) ? 255 : 0;
        }
        // 补位部分填充0
        for (int p = 0; p < padding; p++)
        {
            data[y * rowSize + width + p] = 0;
        }
    }

    free(tempData);
}

int main(int argc, char *argv[])
{
    // 参数检测
    if (argc != 5)
    {
        printf("Usage: %s <input image> <output image> <threshold> <window size>\n", argv[0]);
        return 1;
    }

    // 文件打开
    FILE *fp = fopen(argv[1], "rb");
    if (!fp)
    {
        printf("Cannot open file: %s\n", argv[1]);
        return 1;
    }

    int threshold, windowSize;

    // 参数输入
    if (strstr(argv[3], "-t") != NULL)
    {
        strtok(argv[3], "=");
        threshold = atoi(strtok(NULL, "="));
        strtok(argv[4], "=");
        windowSize = atoi(strtok(NULL, "="));
    }
    else
    {
        strtok(argv[3], "=");
        windowSize = atoi(strtok(NULL, "="));
        strtok(argv[4], "=");
        threshold = atoi(strtok(NULL, "="));
    }

    BITMAPFILEHEADER fHeader;
    BITMAPINFOHEADER iHeader;

    // 信息头文件头获取
    if (fread(&fHeader, sizeof(BITMAPFILEHEADER), 1, fp) != 1 ||
        fread(&iHeader, sizeof(BITMAPINFOHEADER), 1, fp) != 1)
    {
        printf("Failed to read file or info header.\n");
        fclose(fp);
        return 1;
    }

    // 文件位深检测
    if (iHeader.biBitCount != 8)
    {
        printf("Only 8-bit grayscale images are supported.\n");
        fclose(fp);
        return 1;
    }

    // 阈值检测
    if (threshold < 0 || threshold > 255)
    {
        printf("Threshold must be between 0 and 255.\n");
        return 1;
    }

    // 窗口检测
    if (windowSize <= 0 || windowSize > iHeader.biWidth || windowSize > iHeader.biHeight || windowSize % 2 == 0)
    {
        printf("Invalid window size.\n");
        return 1;
    }

    // 临时打印
    printf("WindowSize: %d, Threshold: %d\n", windowSize, threshold);

    // 调色板读取
    int paletteCount = 1 << iHeader.biBitCount;
    RGBQUAD palette[paletteCount];
    if (fread(palette, sizeof(RGBQUAD), paletteCount, fp) != paletteCount)
    {
        printf("Failed to read palette.\n");
        fclose(fp);
        return 1;
    }

    // 计算相关的值
    int padding = (4 - (iHeader.biWidth % 4)) % 4;
    int rowSize = iHeader.biWidth + padding;
    int imageSize = rowSize * iHeader.biHeight;
    unsigned char *imageData = (unsigned char *)malloc(imageSize);
    if (!imageData)
    {
        printf("Memory allocation failed for imageData.\n");
        return 1;
    }

    // 逐行读取图像数据（包含补位）
    for (int y = 0; y < iHeader.biHeight; y++)
    {
        if (fread(&imageData[y * rowSize], 1, iHeader.biWidth, fp) != iHeader.biWidth)
        {
            printf("Failed to read image data.\n");
            fclose(fp);
            return 1;
        }
        if (padding > 0)
        {
            if (fread(&imageData[y * rowSize + iHeader.biWidth], 1, padding, fp) != padding)
            {
                printf("Failed to read image data.\n");
                fclose(fp);
                return 1;
            }
        }
    }
    fclose(fp);

    // 二值化
    binarize(imageData, iHeader.biWidth, iHeader.biHeight, threshold, windowSize);

    // 文件创建
    fp = fopen(argv[2], "wb");
    if (!fp)
    {
        printf("Cannot create output file\n");
        free(imageData);
        return 1;
    }

    // 设置二值化后的调色板
    for (int i = 0; i < paletteCount; i++)
    {
        palette[i].rgbBlue = palette[i].rgbGreen = palette[i].rgbRed = i;
        palette[i].rgbReserved = 0;
    }

    // 文件头部信息写入
    fwrite(&fHeader, sizeof(BITMAPFILEHEADER), 1, fp);
    fwrite(&iHeader, sizeof(BITMAPINFOHEADER), 1, fp);
    fwrite(palette, sizeof(RGBQUAD), paletteCount, fp);

    // 逐行写入图像数据（包含补位）
    for (int y = 0; y < iHeader.biHeight; y++)
    {
        fwrite(&imageData[y * rowSize], 1, rowSize, fp);
    }

    fclose(fp);
    free(imageData);
    printf("Binarization completed, output file: %s\n", argv[2]);

    return 0;
}
