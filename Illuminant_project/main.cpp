/**
 * @file HoughCircle_Demo.cpp
 * @brief Demo code for Hough Transform
 * @author OpenCV team
 */

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <fstream>
#include <random>

using namespace std;
using namespace cv;

namespace
{
    // windows and trackbars name
    const std::string windowName = "Hough Circle Detection Demo";
    const std::string cannyThresholdTrackbarName = "Canny threshold";
    const std::string accumulatorThresholdTrackbarName = "Accumulator Threshold";

    // initial and max values of the parameters of interests.
    const int cannyThresholdInitialValue = 116;
    const int accumulatorThresholdInitialValue = 54;
    const int maxAccumulatorThreshold = 200;
    const int maxCannyThreshold = 255;
    std::vector<Vec3f> circles;

    void HoughDetection(const Mat& src_gray, const Mat& src_display, int cannyThreshold, int accumulatorThreshold)
    {
        // will hold the results of the detection
        //std::vector<Vec3f> circles; //JL240129 changed to global variable so you can report at end
        // runs the actual detection
        HoughCircles(src_gray, circles, HOUGH_GRADIENT, 1, src_gray.rows / 8, cannyThreshold, accumulatorThreshold, 0, 0);

        // clone the colour, input image for displaying purposes
        Mat display = src_display.clone();
        for (size_t i = 0; i < circles.size(); i++)
        {
            Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
            int radius = cvRound(circles[i][2]);
            // circle center
            circle(display, center, 3, Scalar(0, 255, 0), -1, 8, 0);
            // circle outline
            circle(display, center, radius, Scalar(0, 0, 255), 3, 8, 0);
        }

        // shows the results
        imshow(windowName, display);
    }
}


int main(int argc, char** argv)
{
    Mat src, src_gray;

    // Read the image
    //String imageName("C:/Users/jlim1/OneDrive/Documents/Career/Illuminant/illuminant_standard_take-home_2024/rgb.png"); // by default
    //String imageName("C:/Users/jlim1/OneDrive/Documents/Career/Illuminant/illuminant_standard_take-home_2024/gray.png"); 
    String imageName("C:/Users/jlim1/OneDrive/Documents/Career/Illuminant/illuminant_standard_take-home_2024/smarties.png");
    if (argc > 1)
    {
        imageName = argv[1];
    }
    src = imread(samples::findFile(imageName), IMREAD_COLOR);

    if (src.empty())
    {
        std::cerr << "Invalid input image\n";
        std::cout << "Usage : " << argv[0] << " <path_to_input_image>\n";;
        return -1;
    }

    // Convert it to gray
    cvtColor(src, src_gray, COLOR_BGR2GRAY);

    // Reduce the noise so we avoid false circle detection
    GaussianBlur(src_gray, src_gray, Size(9, 9), 2, 2);

    //declare and initialize both parameters that are subjects to change
    int cannyThreshold = cannyThresholdInitialValue;
    int accumulatorThreshold = accumulatorThresholdInitialValue;

    // create the main window, and attach the trackbars
    namedWindow(windowName, WINDOW_KEEPRATIO);
    createTrackbar(cannyThresholdTrackbarName, windowName, &cannyThreshold, maxCannyThreshold);
    createTrackbar(accumulatorThresholdTrackbarName, windowName, &accumulatorThreshold, maxAccumulatorThreshold);

    // infinite loop to display
    // and refresh the content of the output image
    // until the user presses q or Q
    char key = 0;
    while (key != 'q' && key != 'Q')
    {
        // those parameters cannot be =0
        // so we must check here
        cannyThreshold = std::max(cannyThreshold, 1);
        accumulatorThreshold = std::max(accumulatorThreshold, 1);

        //runs the detection, and update the display
        HoughDetection(src_gray, src, cannyThreshold, accumulatorThreshold);

        // get user key
        key = (char)waitKey(10);
    }

    std::string stickerfile = "sticker_locs.csv";
    cout << "Num stickers identified: " + std::to_string(circles.size()) + "\n";
    cout << "Writing detected sticker coordinates to " + stickerfile + "\n";
    std::ofstream myfile;
    myfile.open(stickerfile);
    myfile << "center_x" << ", " << "center_y" << ", " << "radius" << "\n";
    for (int i = 0; i < circles.size(); i++) 
    { 
        cout << "Coordinate " + std::to_string(i) + ": [" + std::to_string(circles[i][0]) + "][" + std::to_string(circles[i][1]) + "][" + std::to_string(circles[i][2]) + "]\n";
        myfile << circles[i][0] << ", " << circles[i][1] << ", " << circles[i][2] << "\n"; 
            
    }
    myfile.close();


    return 0;
}