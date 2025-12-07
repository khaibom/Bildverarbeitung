

#include <iostream>
#include "opencv2/opencv.hpp"


// >>> Aufgabe 11, 16
std::string getColorSystem(const cv::Mat& img)
{
    if (img.channels() == 1) return "Grayscale";
    if (img.channels() == 3) return "BGR";
    if (img.channels() == 4) return "BGRA";
    return "Unknown";
}

std::string getBitDepth(const cv::Mat& img)
{
    switch (img.depth())
    {
    case CV_8U:  return "8-bit unsigned";
    case CV_8S:  return "8-bit signed";
    case CV_16U: return "16-bit unsigned";
    case CV_16S: return "16-bit signed";
    case CV_32S: return "32-bit signed";
    case CV_32F: return "32-bit float";
    case CV_64F: return "64-bit float";
    default:     return "Unknown";
    }
}

void printMatInfo(const cv::Mat& img)
{
    std::cout << "Image specification: " << getColorSystem(img) << "\n";
    std::cout << "Number of channels: " << img.channels() << "\n";
    std::cout << "Bit depth: " << getBitDepth(img) << "\n";
    std::cout << "Image width: " << img.cols << "\n";
    std::cout << "Image height: " << img.rows << "\n";

    std::vector<cv::Mat> channels;
    cv::split(img, channels);

    double globalMin = DBL_MAX;
    double globalMax = -DBL_MAX;
    int globalMinCount = 0; 
    int globalMaxCount = 0;

    for (const auto& channel : channels)
    {
        double minVal, maxVal;
        cv::minMaxLoc(channel, &minVal, &maxVal);

        // occurrences
        cv::Mat minMask = (channel == minVal);
        cv::Mat maxMask = (channel == maxVal);

        int minCount = cv::countNonZero(minMask);
        int maxCount = cv::countNonZero(maxMask);

        if (minVal < globalMin) {
            globalMin = minVal;
            globalMinCount = minCount;
        }
        else if (minVal == globalMin) {
            globalMinCount += minCount;
        }

        if (maxVal > globalMax) {
            globalMax = maxVal;
            globalMaxCount = maxCount;
        }
        else if (maxVal == globalMax) {
            globalMaxCount += maxCount;
        }
    }

    std::cout << "Minimum value: " << globalMin << " (" << globalMinCount << " occurrences)\n";
    std::cout << "Maximum value: " << globalMax << " (" << globalMaxCount << " occurrences)\n";
}
// <<< Aufgabe 11, 16


int main()
{
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);

    std::string inputPath = "D:\\FH Aachen\\Sem 5\\Bildverarbeitung\\Praktikum\\P2\\hela-cells.tif";
    cv::Mat input48 = cv::imread(inputPath, cv::IMREAD_UNCHANGED);

    std::cout <<"Aufgabe 11: \n";
    printMatInfo(input48);
    cv::imshow("hela-cells 48 bit", input48);


    // convert 48bit to 8bit for display the image
    cv::Mat input8;
    cv::normalize(input48, input8, 0, 255, cv::NORM_MINMAX, CV_8UC3);

    cv::imshow("hela-cells 8 bit", input8);
    std::cout << "\nAufgabe 14: \n";
    printMatInfo(input8);

    // Aufgabe 16
    std::vector<cv::Mat> channels8;
    cv::split(input8, channels8);

    std::cout << "\nAufgabe 17: \n";
    std::cout << "Original Blue Channel (8-bit)\n";
    printMatInfo(channels8[0]);
    std::cout << "Original Green Channel (8-bit)\n";
    printMatInfo(channels8[1]);
    std::cout << "Original Red Channel (8-bit)\n";
    printMatInfo(channels8[2]);


    // Aufgabe 18
    std::vector<cv::Mat> channels48;
    cv::split(input48, channels48);

    std::cout << "\nAufgabe 19: \n";
    std::cout << "Original Blue Channel (48-bit)\n";
    printMatInfo(channels48[0]);
    std::cout << "Original Green Channel (48-bit)\n";
    printMatInfo(channels48[1]);
    std::cout << "Original Red Channel (48-bit)\n";
    printMatInfo(channels48[2]);

    // Aufgabe 20
    cv::Mat B8, G8, R8;
    cv::normalize(channels48[0], B8, 0, 255, cv::NORM_MINMAX, CV_8U);
    cv::normalize(channels48[1], G8, 0, 255, cv::NORM_MINMAX, CV_8U);
    cv::normalize(channels48[2], R8, 0, 255, cv::NORM_MINMAX, CV_8U);

    std::cout << "\nAufgabe 21: \n";
    std::cout << "Normalized Blue Channel (8-bit)\n";
    printMatInfo(B8);
    std::cout << "Normalized Green Channel (8-bit)\n";
    printMatInfo(R8);
    std::cout << "Normalized Red Channel (8-bit)\n";
    printMatInfo(G8);

    /*
    * Aufgabe 22
    * global normalization: only 1 channel reaches 255, good for overall display, some channel could lose contrast
    * individual normalization: each channel reaches 255, good for single analysis, relative intensities between channels are changed
    */

    // Aufgabe 23
    cv::imshow("8 bit channel Blue", B8);
    cv::imshow("8 bit channel Green", G8);
    cv::imshow("8 bit channel Red", R8);
    cv::waitKey(0);

}


