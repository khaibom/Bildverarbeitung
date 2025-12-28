
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

// >>> Aufgabe 15
static void onMinWavelengthTrackbar(int pos, void*) {
    g_minWavelength = pos;
    if (pos > g_maxWavelength) g_maxWavelength = pos;
    std::cout << "g_minWavelength = " << g_minWavelength << ", g_maxWavelength = " << g_maxWavelength << std::endl;
}
// <<< Aufgabe 15

// >>> Aufgabe 16
static void onMaxWavelengthTrackbar(int pos, void*) {
    g_maxWavelength = pos;
    if (pos < g_minWavelength) g_minWavelength = pos;
    std::cout << "g_minWavelength = " << g_minWavelength << ", g_maxWavelength = " << g_maxWavelength << std::endl;
}
// <<< Aufgabe 16


int main()
{
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);
    const std::string resultImageWindowTitle = "Result image";
    cv::namedWindow(resultImageWindowTitle, cv::WINDOW_AUTOSIZE);

    // >>> Aufgabe 17
    cv::createTrackbar("min wavelength", resultImageWindowTitle, &g_minWavelength, g_maxWavelength, onMinWavelengthTrackbar);
    cv::createTrackbar("max wavelength", resultImageWindowTitle, &g_maxWavelength, g_maxWavelength, onMaxWavelengthTrackbar);
    // <<< Aufgabe 17
    
    // >>> Aufgabe 18
    cv::setTrackbarMin("min wavelength", resultImageWindowTitle, 1);
    cv::setTrackbarMin("max wavelength", resultImageWindowTitle, 1);
    // <<< Aufgabe 18

    // >>> Aufgabe 19
    while (true){
        const int sideLength = g_maxWavelength * 2 + 1;

        cv::Mat resultImage = getTestImageWithConcentricCircles(sideLength, g_minWavelength, g_maxWavelength);
        cv::imshow(resultImageWindowTitle, resultImage);

        int key = cv::waitKey(30);
        if (key == 27) break;
    }
    // <<< Aufgabe 19

}


