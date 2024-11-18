# SEAM CARVING
Image Resizing using Seam Carving Algorithm

## Problem Statement: 
Apply seam carving content aware image-resizing algorithm on a given image. Take the height and width (in pixels) of the output image as inputs from the user.

### What is Seam Carving?
- Seam-carving is a content-aware image resizing technique where the image is reduced in size by one pixel of height (or width) at a time.
- A vertical seam in an image is a path of pixels connected from the top to the bottom with one pixel in each row.
- A horizontal seam is a path of pixels connected from the left to the right with one pixel in each column.
- Steps:
    - Energy Calculation: Each pixel has some RGB values.
    Calculate energy for each pixel. For ex.- You can use
    dual-gradient energy function but you are free to use any
    energy function of your choice. Also, you can refer to this
    [link](https://www.cs.princeton.edu/courses/archive/fall17/cos226/assignments/seam/index.html) for details.
    - Seam Identification: Identify the lowest energy seam.
    - Seam Removal: Remove the lowest energy seam.
### Program Flow:
  - Extract individual pixel’s RGB values from the sample image
    ./data/input/sample.jpeg and write them into ./data/input/rgb_in.txt
  - Load the RGB values from ./data/input/rgb_in.txt in a 3D matrix.
  - Apply seam carving algorithm.
  - Write the individual pixel’s RGB values for resized image into the
    ./data/output/rgb_out.txt
  - Generate sample image output ./data/output/sample_out.jpeg
    using the ./data/output/rgb_out.txt.

### Output Example

- #### Sample Image1
  Original Image
  
   ![sample4-1](https://github.com/user-attachments/assets/27bc4e59-c079-4954-9bfb-1bd5817ab466)

  
  Resized Image
  
   ![output4](https://github.com/user-attachments/assets/7cb26fe0-dec2-47ad-856b-966767a55bf9)

  
- #### Sample Image2
  Original Image
  
   ![sample6](https://github.com/user-attachments/assets/7f7e690b-95f8-4f56-be68-82505f1ff585)

  Resized Image
  
  ![output6](https://github.com/user-attachments/assets/9fb20fd2-c435-4736-9a0a-36c6535e635b)



