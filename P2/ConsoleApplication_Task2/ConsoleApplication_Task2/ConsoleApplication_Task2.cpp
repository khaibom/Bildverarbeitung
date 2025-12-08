

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

// >>> Aufgabe 25
int g_thresholdB = 60;
int g_thresholdG = 60;
int g_thresholdR = 60;

int g_transparencyB = 100;
int g_transparencyG = 100;
int g_transparencyR = 100;

cv::Mat B8_original, G8_original, R8_original;
cv::Mat B8_display, G8_display, R8_display;

void onThresholdBlueTrackbar(int, void*) {
    cv::Mat threshold;
    cv::threshold(B8_original, threshold, g_thresholdB, 255, cv::THRESH_TOZERO);
    double alpha = g_transparencyB / 100.0;
    cv::addWeighted(threshold, alpha, cv::Mat::zeros(threshold.size(), threshold.type()), 1.0 - alpha, 0, B8_display);
    cv::imshow("8-bit B channel", B8_display);
    std::cout << "[Blue] Threshold: " << g_thresholdB << ",  Transparency: " << g_transparencyB << "%\n";
}

void onTransparencyBlueTrackbar(int, void*) {
    onThresholdBlueTrackbar(0, nullptr);
}

void onThresholdGreenTrackbar(int, void*) {
    cv::Mat threshold;
    cv::threshold(G8_original, threshold, g_thresholdG, 255, cv::THRESH_TOZERO);
    double alpha = g_transparencyG / 100.0;
    cv::addWeighted(threshold, alpha, cv::Mat::zeros(threshold.size(), threshold.type()), 1.0 - alpha, 0, G8_display);
    cv::imshow("8-bit G channel", G8_display);
    std::cout << "[Green] Threshold: " << g_thresholdG << ",  Transparency: " << g_transparencyG << "%\n";
}

void onTransparencyGreenTrackbar(int, void*) {
    onThresholdGreenTrackbar(0, nullptr);
}

void onThresholdRedTrackbar(int, void*) {
    cv::Mat threshold;
    cv::threshold(R8_original, threshold, g_thresholdR, 255, cv::THRESH_TOZERO);
    double alpha = g_transparencyR / 100.0;
    cv::addWeighted(threshold, alpha, cv::Mat::zeros(threshold.size(), threshold.type()), 1.0 - alpha, 0, R8_display);
    cv::imshow("8-bit R channel", R8_display);
    std::cout << "[Red] Threshold: " << g_thresholdR << ",  Transparency: " << g_transparencyR << "%\n";
}

void onTransparencyRedTrackbar(int, void*) {
    onThresholdRedTrackbar(0, nullptr);
}
// <<< Aufgabe 25

// >>> Aufgabe 26
cv::Mat createBinaryImage(const cv::Mat& gray, int threshold){
    cv::Mat binary = (gray > threshold);
    return binary;
}
// <<< Aufgabe 26

// >>> Aufgabe 27
cv::Mat applyThresholdColor(const cv::Mat& gray, int threshold, const cv::Scalar& color){
    uchar grauWert = std::max({(uchar)color[0], (uchar)color[1], (uchar)color[2]}); // V von HSV
    cv::Mat colored(gray.size(), CV_8UC3);
    cv::Mat mask = createBinaryImage(gray, threshold);

    // alle Pixel bis zum Schwellwert einen Grauwert erhalten
    colored.setTo(cv::Scalar(grauWert, grauWert, grauWert));
    // der bei Betrachtung des HSV-Farbsystems etwa als gleich hell wahrgenommen würde wie die vorgegebene Farbe
    colored.setTo(color, mask);

    return colored;
}
// <<< Aufgabe 27



int main()
{
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);

    std::string inputPath = "D:\\FH Aachen\\Sem 5\\Bildverarbeitung\\Praktikum\\P2\\hela-cells.tif";
    cv::Mat input48 = cv::imread(inputPath, cv::IMREAD_UNCHANGED);

    std::cout <<"Aufgabe 11: \n";
    printMatInfo(input48);
    //cv::imshow("hela-cells 48 bit", input48);


    // convert 48bit to 8bit for display the image
    cv::Mat input8;
    cv::normalize(input48, input8, 0, 255, cv::NORM_MINMAX, CV_8UC3);

    //cv::imshow("hela-cells 8 bit", input8);
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
    //cv::imshow("8-bit B channel", B8);
    //cv::imshow("8-bit G channel", G8);
    //cv::imshow("8-bit R channel", R8);

    // Aufgabe 24
    B8_original = B8.clone();
    G8_original = G8.clone();
    R8_original = R8.clone();
    B8_display = B8.clone();
    G8_display = G8.clone();
    R8_display = R8.clone();

    cv::imshow("8-bit B channel", B8_display);
    cv::imshow("8-bit G channel", G8_display);
    cv::imshow("8-bit R channel", R8_display);

    cv::createTrackbar("Threshold B", "8-bit B channel", &g_thresholdB, 255, onThresholdBlueTrackbar);
    cv::createTrackbar("Alpha B (%)", "8-bit B channel", &g_transparencyB, 100, onTransparencyBlueTrackbar);
    cv::createTrackbar("Threshold G", "8-bit G channel", &g_thresholdG, 255, onThresholdGreenTrackbar);
    cv::createTrackbar("Alpha G (%)", "8-bit G channel", &g_transparencyG, 100, onTransparencyGreenTrackbar);
    cv::createTrackbar("Threshold R", "8-bit R channel", &g_thresholdR, 255, onThresholdRedTrackbar);
    cv::createTrackbar("Alpha R (%)", "8-bit R channel", &g_transparencyR, 100, onTransparencyRedTrackbar);

    // Aufgabe 26
    cv::Mat binaryB = createBinaryImage(B8, g_thresholdB);
    cv::Mat binaryG = createBinaryImage(G8, g_thresholdG);
    cv::Mat binaryR = createBinaryImage(R8, g_thresholdR);

    //cv::imshow("Binary B8", binaryB);
    //cv::imshow("Binary G8", binaryG);
    //cv::imshow("Binary R8", binaryR);

    // Aufgabe 27
    cv::Scalar color(70, 120, 170);
    cv::Mat aufgabe27 = applyThresholdColor(G8_display, g_thresholdG, color);
    cv::imshow("Threshold Color Aufgabe 27", aufgabe27);



    cv::waitKey(0);

}


