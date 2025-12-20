// ConsoleApplication_Task3.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <opencv2/opencv.hpp>

cv::Mat getTestImageWithConcentricCircles(int sideLength,
    int minWavelength,
    int maxWavelength)
{
    // Graustufenbild erzeugen
    cv::Mat testImage(sideLength, sideLength, CV_8UC1);

    // Mittelpunkt des Bildes
    const double testImageCenterX = (sideLength - 1) / 2.0;
    const double testImageCenterY = (sideLength - 1) / 2.0;

    const double maxRadius = (sideLength - 1) / 2.0;
    const double pi = std::acos(-1.0);

    // Über alle Pixel iterieren
    for (int y = 0; y < sideLength; ++y)
    {
        for (int x = 0; x < sideLength; ++x)
        {
            const auto dy = testImageCenterY - y;
            const auto dx = testImageCenterX - x;
            const auto dr = std::sqrt(dx * dx + dy * dy);

            if (dr > maxRadius)
            {
                testImage.at<unsigned char>(y, x) = 0;
            }
            else
            {
                const auto interpolatedWavelength =
                    maxWavelength
                    - (maxWavelength - minWavelength) * dr / maxRadius;

                const auto pixelValue =
                    static_cast<unsigned char>(
                        127.5 * (1.0 + std::cos(2.0 * pi * dr / interpolatedWavelength))
                        );

                testImage.at<unsigned char>(y, x) = pixelValue;
            }
        }
    }

    return testImage;
}

int main()
{
    std::cout << "Hello World!\n";
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
