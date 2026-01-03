
#include <iostream>
#include <opencv2/opencv.hpp>
#include <Windows.h>
#include <AtlBase.h>


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

// >>> Aufgabe 27
cv::Mat getAxisAlignedMask(int sideLength)
{
    cv::Mat mask = cv::Mat::zeros(sideLength, sideLength, CV_8UC1);
    const int c = sideLength / 2;

    mask(cv::Range(0, c), cv::Range(0, c)).setTo(0);                  // oben links
    mask(cv::Range(0, c), cv::Range(c + 1, sideLength)).setTo(1);     // oben rechts
    mask(cv::Range(c + 1, sideLength), cv::Range(0, c)).setTo(2);     // unten links
    mask(cv::Range(c + 1, sideLength), cv::Range(c + 1, sideLength)).setTo(0); // unten rechts

    mask(cv::Range(0, c), cv::Range(c, c + 1)).setTo(3);
    mask(cv::Range(c + 1, sideLength), cv::Range(c, c + 1)).setTo(4);

    mask(cv::Range(c, c + 1), cv::Range(0, c)).setTo(4);
    mask(cv::Range(c, c + 1), cv::Range(c + 1, sideLength)) .setTo(3);

    // Mittelpunkt
    mask(cv::Range(c, c + 1), cv::Range(c, c + 1)).setTo(5);

    return mask;
}
// <<< Aufgabe 27

// >>> Aufgabe 29
cv::Mat getResultImage(cv::Mat testImage, cv::Mat firstProcessedTestImage, cv::Mat secondProcessedTestImage, cv::Mat mask)
{
    cv::Mat result = cv::Mat::zeros(testImage.size(), CV_8UC3);
    // Graubilder nach BGR konvertieren
    cv::Mat testImageBGR, firstProcessedTestImageBGR, secondProcessedTestImageBGR;
    cv::cvtColor(testImage, testImageBGR, cv::COLOR_GRAY2BGR);
    cv::cvtColor(firstProcessedTestImage, firstProcessedTestImageBGR, cv::COLOR_GRAY2BGR);
    cv::cvtColor(secondProcessedTestImage, secondProcessedTestImageBGR, cv::COLOR_GRAY2BGR);

    cv::Mat ones = cv::Mat::ones(testImageBGR.size(), testImageBGR.type());
    cv::Mat temp = cv::Mat::zeros(testImageBGR.size(), testImageBGR.type());

    cv::bitwise_and(testImageBGR, testImageBGR, temp, mask == 0);
    result += temp;

    temp.setTo(0);
    cv::bitwise_and(firstProcessedTestImageBGR, firstProcessedTestImageBGR, temp, mask == 1);
    result += temp;

    temp.setTo(0);
    cv::bitwise_and(secondProcessedTestImageBGR, secondProcessedTestImageBGR, temp, mask == 2);
    result += temp;

    result.setTo(cv::Scalar(0, 255, 255), mask == 3);

    result.setTo(cv::Scalar(255, 0, 0), mask == 4);

    result.setTo(cv::Scalar(255, 255, 255), mask == 5);
    return result;
}
// <<< Aufgabe 29

// Aufgabe 33
int g_rotationAngleDeg = 0;

// >>> Aufgabe 34
static void onRotationAngleDegTrackbar(int pos, void*){
    g_rotationAngleDeg = pos;
    std::cout << "g_rotationAngleDeg = " << g_rotationAngleDeg << std::endl;
}
// <<< Aufgabe 34

// >>> Aufgabe 36
cv::Mat getMask(cv::Mat axisAlignedMask, int rotationAngleDeg){
    cv::Mat rotatedMask;
    const cv::Point2f center(axisAlignedMask.cols / 2.0f, axisAlignedMask.rows / 2.0f);
    cv::Mat rotationMatrix = cv::getRotationMatrix2D(center, rotationAngleDeg, 1.0);
    cv::warpAffine(
        axisAlignedMask,
        rotatedMask,
        rotationMatrix,
        axisAlignedMask.size(),
        cv::INTER_NEAREST,
        cv::BORDER_CONSTANT,
        cv::Scalar(0)
    );
    return rotatedMask;
}
// <<< Aufgabe 36

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

    // >>> Aufgabe 20
    const std::wstring resultImageWindowTitleW = CA2W(resultImageWindowTitle.c_str()).m_psz;
    // <<< Aufgabe 20

    // >>> Aufgabe 28
    const int axisMaskSideLength = g_maxWavelength * 2 + 1;
    cv::Mat axisAlignedMask = getAxisAlignedMask(axisMaskSideLength);
    // <<< Aufgabe 28

    // >>> Aufgabe 35
    cv::createTrackbar("rotation angle", resultImageWindowTitle, nullptr, 360, onRotationAngleDegTrackbar);
    // <<< Aufgabe 35

    // >>> Aufgabe 19, 20, 21
    while (FindWindow(NULL, resultImageWindowTitleW.c_str())){
        const int sideLength = g_maxWavelength * 2 + 1;
        axisAlignedMask = getAxisAlignedMask(sideLength);

        cv::Mat resultImage = getTestImageWithConcentricCircles(sideLength, g_minWavelength, g_maxWavelength);
        cv::imshow(resultImageWindowTitle, resultImage);

        // >>> Aufgabe 23
        cv::Mat firstProcessedTestImage;
        cv::blur(resultImage, firstProcessedTestImage, cv::Size(5, 5));
        //cv::imshow("5x5 Box Filter", firstProcessedTestImage);
        // <<< Aufgabe 23

        // >>> Aufgabe 24
        cv::Mat secondProcessedTestImage;
        cv::blur(resultImage, secondProcessedTestImage, cv::Size(9, 9));
        //cv::imshow("9x9 Box Filter", secondProcessedTestImage);
        // <<< Aufgabe 24

        // >>> Aufgabe 30, 31
        cv::Mat partitionedResultImage = getResultImage(resultImage, firstProcessedTestImage, secondProcessedTestImage, axisAlignedMask);
        cv::imshow(resultImageWindowTitle, partitionedResultImage);
        // <<< Aufgabe 30, 31

        // Aufgabe 37
        cv::Mat mask = getMask(axisAlignedMask, g_rotationAngleDeg);

        // >>> Aufgabe 38
        cv::Mat partitionedResultImage38 = getResultImage(resultImage, firstProcessedTestImage, secondProcessedTestImage, mask);
        cv::imshow(resultImageWindowTitle, partitionedResultImage38);
        // <<< Aufgabe 38

        int key = cv::waitKey(30);
        if (key == 27) break;
    }
    // <<< Aufgabe 19, 20, 21

    /*
    * Aufgabe 25:
    * 5x5 Box Filter:
    * - leichter gegl�ttet
    *  
    * 9x9 Box Filter: 
    * - st�rker gegl�ttet
    * - Kontraste werden reduziert
    */
}


