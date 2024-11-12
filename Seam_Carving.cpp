#include <iostream> // For standard input/output operations
#include <opencv2/opencv.hpp> // For OpenCV functions for image processing

using namespace cv; // To avoid prefixing cv:: before OpenCV classes and functions
using namespace std; // To avoid prefixing std:: before standard library classes and functions

// Function to calculate the energy of a pixel at position (i, j)
double calculate_energy_matrix(int i, int j, int w, int h, Mat image) {
    double horizontal; // Variable to store horizontal energy
    double vertical; // Variable to store vertical energy

    // Get vertical neighbors, wrapping around at edges
    int prev_x = (i - 1 + h) % h; // Previous row index with wrap-around
    int next_x = (i + 1) % h; // Next row index with wrap-around

    // Calculate color difference in the vertical direction
    int r1 = image.at<Vec3b>(prev_x, j)[0]; // Red channel at previous row
    int g1 = image.at<Vec3b>(prev_x, j)[1]; // Green channel at previous row
    int b1 = image.at<Vec3b>(prev_x, j)[2]; // Blue channel at previous row
    int r2 = image.at<Vec3b>(next_x, j)[0]; // Red channel at next row
    int g2 = image.at<Vec3b>(next_x, j)[1]; // Green channel at next row
    int b2 = image.at<Vec3b>(next_x, j)[2]; // Blue channel at next row

    int diff_r = abs(r1 - r2); // Absolute difference in red channel
    int diff_g = abs(g1 - g2); // Absolute difference in green channel
    int diff_b = abs(b1 - b2); // Absolute difference in blue channel

    vertical = (double) diff_r * diff_r + (double) diff_g * diff_g + (double) diff_b * diff_b; // Squared vertical energy

    // Get horizontal neighbors, wrapping around at edges
    int prev_y = (j - 1 + w) % w; // Previous column index with wrap-around
    int next_y = (j + 1) % w; // Next column index with wrap-around

    // Calculate color difference in the horizontal direction
    r1 = image.at<Vec3b>(i, prev_y)[0];
    g1 = image.at<Vec3b>(i, prev_y)[1];
    b1 = image.at<Vec3b>(i, prev_y)[2];
    r2 = image.at<Vec3b>(i, next_y)[0];
    g2 = image.at<Vec3b>(i, next_y)[1];
    b2 = image.at<Vec3b>(i, next_y)[2];

    diff_r = abs(r1 - r2);
    diff_g = abs(g1 - g2);
    diff_b = abs(b1 - b2);

    horizontal = (double) diff_r * diff_r + (double) diff_g * diff_g + (double) diff_b * diff_b; // Squared horizontal energy

    double res_energy = sqrt(horizontal + vertical); // Total energy by combining horizontal and vertical energy
    return res_energy; // Return energy value
}

// Function to perform vertical seam carving (remove one vertical seam)
Mat seam_carving_vertical(int w, int h, Mat image) {
    int *x_path_vertical = new int[h]; // Array to store the x-coordinates of the seam path
    int *y_path_vertical = new int[h]; // Array to store the y-coordinates of the seam path
    double **seam_carving = new double *[h]; // 2D array to store cumulative energy for each pixel

    for(int i = 0; i < h; i++) { // Allocate memory for each row in seam_carving array
        seam_carving[i] = new double[w];
    }

    for(int i = 0; i < w; i++) { // Initialize first row of seam_carving with energy values
        seam_carving[0][i] = calculate_energy_matrix(0, i, w, h, image);
    }

    for(int i = 1; i < h; i++) { // Loop through each row
        for(int j = 0; j < w; j++) { // Loop through each column
            double sum = seam_carving[i - 1][j]; // Start with the energy from the pixel directly above

            // Check the top-left diagonal pixel if it exists
            if(j - 1 >= 0 && sum > seam_carving[i - 1][j - 1]) {
                sum = seam_carving[i - 1][j - 1];
            }

            // Check the top-right diagonal pixel if it exists
            if(j + 1 < w && sum > seam_carving[i - 1][j + 1]) {
                sum = seam_carving[i - 1][j + 1];
            }

            // Add the current pixel's energy
            seam_carving[i][j] = sum + calculate_energy_matrix(i, j, w, h, image);
        }
    }

    // Find the pixel in the bottom row with the minimum cumulative energy to start the seam
    double mini = seam_carving[h - 1][0];
    int x = h - 1; // Row index for the start of the seam
    int y = 0; // Column index for the start of the seam
    for(int i = 1; i < w; i++) { // Loop through last row to find minimum energy value
        if(mini > seam_carving[h - 1][i]) {
            mini = seam_carving[h - 1][i];
            y = i; // Update column index of minimum energy
        }
    }

    x_path_vertical[h - 1] = x; // Store the starting row index of the seam
    y_path_vertical[h - 1] = y; // Store the starting column index of the seam

    // Mark seam pixels in red in the image
    image.at<Vec3b>(x, y) = Vec3b(255, 0, 0);

    // Backtrack up the seam from the bottom to the top row
    for(int j = h - 2; j >= 0; j--) {
        int res_x = x - 1; // Move to the previous row
        int res_y = y;
        double res_sum = seam_carving[res_x][res_y]; // Initialize with directly above pixel's energy

        // Check left diagonal pixel
        if(y - 1 >= 0 && res_sum > seam_carving[x - 1][y - 1]) {
            res_sum = seam_carving[x - 1][y - 1];
            res_y = y - 1;
        }

        // Check right diagonal pixel
        if(y + 1 < w && res_sum > seam_carving[x - 1][y + 1]) {
            res_sum = seam_carving[x - 1][y + 1];
            res_y = y + 1;
        }

        x = res_x; // Update x-coordinate
        y = res_y; // Update y-coordinate
        x_path_vertical[j] = x; // Store x-coordinate of seam
        y_path_vertical[j] = y; // Store y-coordinate of seam

        // Mark seam pixels in red
        image.at<Vec3b>(x, y) = Vec3b(255, 0, 0);
    }

    // Create a resized image by removing the seam
    Mat res_image;
    cv::resize(image, res_image, Size(w - 1, h), INTER_LINEAR);

    // Shift pixels to the left to remove the seam
    for(int i = 0; i < h; i++) {
        int flag = 0; // Flag to track shift due to seam removal
        for(int j = 0; j < w; j++) {
            if(x_path_vertical[i] == i && y_path_vertical[i] == j) {
                flag = 1; // Set flag when seam pixel is reached
                continue;
            }
            // Copy pixels, shifting them left to close the gap
            res_image.at<Vec3b>(i, j - flag) = image.at<Vec3b>(i, j);
        }
    }

    imshow("Display Image", image); // Display intermediate image with seam marked in red
    cv::waitKey(1); // Wait briefly to display image

    return res_image; // Return the resized image with one seam removed
}

// ... [Code continues with horizontal seam carving and main function]

// Function to perform horizontal seam carving (remove one horizontal seam)
Mat seam_carving_horizontal(int w, int h, Mat image) {
    int *x_path_horizontal = new int[w]; // Array to store the x-coordinates of the seam path
    int *y_path_horizontal = new int[w]; // Array to store the y-coordinates of the seam path
    double **seam_carving = new double *[h]; // 2D array to store cumulative energy for each pixel

    for(int i = 0; i < h; i++) { // Allocate memory for each row in seam_carving array
        seam_carving[i] = new double[w];
    }

    // Initialize first column of seam_carving with energy values
    for(int i = 0; i < h; i++) {
        seam_carving[i][0] = calculate_energy_matrix(i, 0, w, h, image);
    }

    // Populate seam_carving matrix with cumulative energy values from left to right
    for(int j = 1; j < w; j++) { // Loop through each column
        for(int i = 0; i < h; i++) { // Loop through each row
            double sum = seam_carving[i][j - 1]; // Start with the energy from the pixel directly to the left

            // Check the top-left diagonal pixel if it exists
            if(i - 1 >= 0 && sum > seam_carving[i - 1][j - 1]) {
                sum = seam_carving[i - 1][j - 1];
            }

            // Check the bottom-left diagonal pixel if it exists
            if(i + 1 < h && sum > seam_carving[i + 1][j - 1]) {
                sum = seam_carving[i + 1][j - 1];
            }

            // Add the current pixel's energy
            seam_carving[i][j] = sum + calculate_energy_matrix(i, j, w, h, image);
        }
    }

    // Find the pixel in the last column with the minimum cumulative energy to start the seam
    double mini = seam_carving[0][w - 1];
    int x = 0; // Row index for the start of the seam
    int y = w - 1; // Column index for the start of the seam
    for(int i = 1; i < h; i++) { // Loop through the last column to find minimum energy value
        if(mini > seam_carving[i][w - 1]) {
            mini = seam_carving[i][w - 1];
            x = i; // Update row index of minimum energy
        }
    }

    x_path_horizontal[w - 1] = x; // Store the starting row index of the seam
    y_path_horizontal[w - 1] = y; // Store the starting column index of the seam

    // Mark seam pixels in red in the image
    image.at<Vec3b>(x, y) = Vec3b(255, 0, 0);

    // Backtrack leftward through the seam from the rightmost to the leftmost column
    for(int j = w - 2; j >= 0; j--) {
        int res_x = x;
        int res_y = y - 1; // Move to the previous column
        double res_sum = seam_carving[res_x][res_y]; // Initialize with directly left pixel's energy

        // Check top-left diagonal pixel
        if(x - 1 >= 0 && res_sum > seam_carving[x - 1][y - 1]) {
            res_sum = seam_carving[x - 1][y - 1];
            res_x = x - 1;
        }

        // Check bottom-left diagonal pixel
        if(x + 1 < h && res_sum > seam_carving[x + 1][y - 1]) {
            res_sum = seam_carving[x + 1][y - 1];
            res_x = x + 1;
        }

        x = res_x; // Update x-coordinate
        y = res_y; // Update y-coordinate
        x_path_horizontal[j] = x; // Store x-coordinate of seam
        y_path_horizontal[j] = y; // Store y-coordinate of seam

        // Mark seam pixels in red
        image.at<Vec3b>(x, y) = Vec3b(255, 0, 0);
    }

    // Create a resized image by removing the seam
    Mat res_image;
    cv::resize(image, res_image, Size(w, h - 1), INTER_LINEAR);

    // Shift pixels up to remove the seam
    for(int j = 0; j < w; j++) {
        int flag = 0; // Flag to track shift due to seam removal
        for(int i = 0; i < h; i++) {
            if(x_path_horizontal[j] == i && y_path_horizontal[j] == j) {
                flag = 1; // Set flag when seam pixel is reached
                continue;
            }
            // Copy pixels, shifting them up to close the gap
            res_image.at<Vec3b>(i - flag, j) = image.at<Vec3b>(i, j);
        }
    }

    imshow("Display Image", image); // Display intermediate image with seam marked in red
    cv::waitKey(1); // Wait briefly to display image

    return res_image; // Return the resized image with one seam removed
}

// Main function to handle image loading, resizing, and user interaction
int main(int argc, char **argv) {
    string image_path; // Variable to hold the path of the image
    cout << "Give path of image" << endl;
    cin >> image_path; // Get image path from the user

    cout << "Give size of image you want in (height,width) format" << endl;
    int res_height, res_width; // Variables to hold target height and width
    cin >> res_height >> res_width; // Get target height and width from the user

    Mat image = imread(image_path, IMREAD_COLOR); // Load the image in color
    if (!image.data) { // Check if the image was loaded successfully
        printf("No image data \n"); // Print error message if image loading failed
        return -1; // Exit with error code
    }

    namedWindow("Display Image", WINDOW_AUTOSIZE); // Create a window to display images
    imshow("Display Image", image); // Display the original image

    int height = image.rows; // Get original image height
    int width = image.cols; // Get original image width

    // Check if the requested size is within the original image size range
    if (res_height <= 0 || res_width <= 0 || res_height > height || res_width > width) {
        cout << "Please specify size in range" << endl; // Prompt the user if size is invalid
        return -1; // Exit with error code
    }

    int w = width; // Initialize current width
    int h = height; // Initialize current height

    // Reduce the width of the image by removing vertical seams
    while (w > res_width) {
        image = seam_carving_vertical(w, h, image); // Remove one vertical seam
        w--; // Decrement width by 1
    }
    imshow("Display Image", image); // Display the image after width resizing

    // Reduce the height of the image by removing horizontal seams
    while (h > res_height) {
        image = seam_carving_horizontal(w, h, image); // Remove one horizontal seam
        h--; // Decrement height by 1
    }
    imshow("Display Image", image); // Display the final resized image

    cv::waitKey(0); // Wait indefinitely for a key press before closing the window
    return 0; // Exit the program
}


