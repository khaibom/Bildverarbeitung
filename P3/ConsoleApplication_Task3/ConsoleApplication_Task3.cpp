
#include <iostream>
#include <opencv2/opencv.hpp>


cv::Mat getTestImageWithConcentricCircles(int sideLength, int minWavelength, int maxWavelength) {
    cv::Mat testImage(sideLength, sideLength, CV_8UC1);

    // Mittelpunkt des Bildes
    const double testImageCenterX = (sideLength - 1) / 2.0;
    const double testImageCenterY = (sideLength - 1) / 2.0;

    const double pi = std::acos(-1.0);

    for (int y = 0; y < sideLength; ++y)
    {
        for (int x = 0; x < sideLength; ++x)
        {
            const auto dy = testImageCenterY - y;
            const auto dx = testImageCenterX - x;
            const auto dr = std::sqrt(dx * dx + dy * dy);

            if (dr > (sideLength - 1) / 2.0)
            {
                testImage.at<unsigned char>(y, x) = 0;
            }
            else
            {
                const auto interpolatedWavelength = maxWavelength - (maxWavelength - minWavelength) * dr / ((sideLength - 1) / 2.0);
                const auto pixelValue = static_cast<unsigned char>(127.5 * (1.0 + std::cos(2.0 * pi * dr / interpolatedWavelength)));
                testImage.at<unsigned char>(y, x) = pixelValue;
            }
        }
    }
    return testImage;
}

int g_minWavelength = 1;
int g_maxWavelength = 299;

int main()
{
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);

    const int sideLength = g_maxWavelength * 2 + 1;

    cv::Mat resultImage = getTestImageWithConcentricCircles(sideLength, g_minWavelength, g_maxWavelength);
    const std::string resultImageWindowTitle = "Result image";
    cv::namedWindow(resultImageWindowTitle, cv::WINDOW_AUTOSIZE);
    cv::imshow(resultImageWindowTitle, resultImage);
    cv::waitKey(0);

}


