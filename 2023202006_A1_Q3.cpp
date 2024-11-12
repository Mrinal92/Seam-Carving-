#include <iostream>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;


double calculate_energy_matrix(int i,int j,int w,int h,Mat image)
{
   double horizontal;
        double vertical;

          int prev_x = (i-1 + h)%h;
          int next_x = (i+1)%h;

          int r1 = image.at<cv::Vec3b>(prev_x,j)[0]-'\0';
          int g1 = image.at<cv::Vec3b>(prev_x,j)[1]-'\0';
          int b1 = image.at<cv::Vec3b>(prev_x,j)[2]-'\0';

          int r2 = image.at<cv::Vec3b>(next_x,j)[0]-'\0';
          int g2 = image.at<cv::Vec3b>(next_x,j)[1]-'\0';
          int b2 = image.at<cv::Vec3b>(next_x,j)[2]-'\0';

          int diff_r = abs(r1-r2);
          int diff_g = abs(g1-g2);
          int diff_b = abs(b1-b2);

          vertical = (double) diff_r*diff_r + (double) diff_g*diff_g + (double) diff_b*diff_b;  
          

        int prev_y = (j-1 + w)%w;
        int next_y = (j+1)%w;

         r1 = image.at<cv::Vec3b>(i,prev_y)[0]-'\0';
         g1 = image.at<cv::Vec3b>(i,prev_y)[1]-'\0';
         b1 = image.at<cv::Vec3b>(i,prev_y)[2] -'\0';

         r2 = image.at<cv::Vec3b>(i,next_y)[0]-'\0';
         g2 = image.at<cv::Vec3b>(i,next_y)[1]-'\0';
         b2 = image.at<cv::Vec3b>(i,next_y)[2]-'\0';

         diff_r = abs(r1-r2);
         diff_g = abs(g1-g2);
         diff_b = abs(b1-b2);

          horizontal = (double) diff_r*diff_r + (double) diff_g*diff_g + (double) diff_b*diff_b; 

       double res_energy = sqrt(horizontal+vertical);

       return res_energy;

}


Mat seam_carving_vertical(int w,int h,Mat image)
{
     
      int* x_path_vertical = new int [h];
      int* y_path_vertical = new int [h];


    double** seam_carving = new double* [h];

      for (int i = 0; i < h; i++) {
 
        // Declare a memory block
        // of size n
        seam_carving[i] = new double [w];
    }


    for(int i=0;i<w;i++)
    {
      seam_carving[0][i] = calculate_energy_matrix(0,i,w,h,image);
    }

    for(int i=1;i<h;i++)
    {
      for(int j=0;j<w;j++)
      {
      
        
        double sum = seam_carving[i-1][j];

        if(j-1>=0)
          {
            if(sum>seam_carving[i-1][j-1])
            {
                 sum = seam_carving[i-1][j-1];
                
            }
          }

        if(j+1<w)
          {
            if(sum>seam_carving[i-1][j+1])
            {
               sum = seam_carving[i-1][j+1];
                
            }
          }

       
        seam_carving[i][j] = sum + calculate_energy_matrix(i,j,w,h,image);


      }
    }

  double mini = seam_carving[h-1][0];
  int x = h-1;
  int y = 0;

  for(int i=1;i<w;i++)
    {
      if(mini>seam_carving[h-1][i])
      {
        mini = seam_carving[h-1][i];
        x = h-1;
        y = i;
       }
    }

  x_path_vertical[h-1] = x;
  y_path_vertical[h-1] = y;
   
  image.at<cv::Vec3b>(x,y)[0]=255+'\0';
  image.at<cv::Vec3b>(x,y)[1]=0+'\0';
  image.at<cv::Vec3b>(x,y)[2]= 0+'\0';
   
  for(int j=h-2;j>=0;j--)
  {
      int res_x = x-1;
      int res_y = y;

      double res_sum = seam_carving[res_x][res_y];

      if(y-1>=0)
      {
        if(res_sum>seam_carving[x-1][y-1])
        {
          res_sum = seam_carving[x-1][y-1];
          res_x = x-1;
          res_y = y-1;
        }
      }

      if(y+1<w)
      {
         if(res_sum>seam_carving[x-1][y+1])
         {
          res_sum = seam_carving[x-1][y+1];
          res_x = x-1;
          res_y = y+1;
         }
      }

       x = res_x;
       y = res_y;

    x_path_vertical[j] = x;
    y_path_vertical[j] = y;
    

    image.at<cv::Vec3b>(x,y)[0]=255+'\0';
    image.at<cv::Vec3b>(x,y)[1]=0+'\0';
    image.at<cv::Vec3b>(x,y)[2]= 0+'\0';
  }


  Mat res_image;
 cv::resize(image, res_image, Size(w-1, h), INTER_LINEAR);

 
  for(int i=0;i<h;i++)
  {
    int flag=0;
    for(int j=0;j<w;j++)
    {
        if(x_path_vertical[i]==i && y_path_vertical[i]==j)
        {
        flag=1;    
        continue;
        }
       
    res_image.at<cv::Vec3b>(i,j-flag)[0]=image.at<cv::Vec3b>(i,j)[0];
    res_image.at<cv::Vec3b>(i,j-flag)[1]=image.at<cv::Vec3b>(i,j)[1];
    res_image.at<cv::Vec3b>(i,j-flag)[2]=image.at<cv::Vec3b>(i,j)[2];

    }
  }
     imshow("Display Image", image);
      cv::waitKey(1);

     return res_image; 
}

Mat seam_carving_horizontal(int w,int h,Mat image)
{
 
  // int x_path_horizontal[w];
  // int y_path_horizontal[w];

  // double seam_carving[h][w];

  int* x_path_horizontal = new int [w];
  int* y_path_horizontal = new int [w];


  double** seam_carving = new double* [h];

    for (int i = 0; i < h; i++) {
 
        seam_carving[i] = new double [w];
    }
 
  for(int i=0;i<h;i++)
   {
    seam_carving[i][0] = calculate_energy_matrix(i,0,w,h,image);
   }

  
   for(int j=1;j<w;j++)
   {

    for(int i=0;i<h;i++)
    {
      
        
        double sum = seam_carving[i][j-1];

        if(i-1>=0)
        {
            if(sum>seam_carving[i-1][j-1])
            {
                 sum = seam_carving[i-1][j-1];
                
            }
        }

        if(i+1<h)
        {
            if(sum>seam_carving[i+1][j-1])
            {
               sum = seam_carving[i+1][j-1];
                
            }
        }

       
        seam_carving[i][j] = sum + calculate_energy_matrix(i,j,w,h,image);


    }

   }

   double mini = seam_carving[0][w-1];
   int x = 0;
   int y = w-1;
   for(int i=1;i<h;i++)
    {
      if(mini>seam_carving[i][w-1])
      {
        mini = seam_carving[i][w-1];
        x = i;
        y = w-1;
      }
    }

   x_path_horizontal[w-1] = x;
   y_path_horizontal[w-1] = y;
   
    image.at<cv::Vec3b>(x,y)[0]=255+'\0';
    image.at<cv::Vec3b>(x,y)[1]=0+'\0';
    image.at<cv::Vec3b>(x,y)[2]= 0+'\0';


    
   for(int j=w-2;j>=0;j--)
   {
       int res_x = x;
       int res_y = y-1;

       double res_sum = seam_carving[res_x][res_y];

       if(x-1>=0)
       {
        if(res_sum>seam_carving[x-1][y-1])
        {
          res_sum = seam_carving[x-1][y-1];
          res_x = x-1;
          res_y = y-1;
        }
       }

       if(x+1<h)
       {
         if(res_sum>seam_carving[x+1][y-1])
         {
          res_sum = seam_carving[x+1][y-1];
          res_x = x+1;
          res_y = y-1;
         }
       }

       x = res_x;
       y = res_y;

   x_path_horizontal[j] = x;
   y_path_horizontal[j] = y;
    

    image.at<cv::Vec3b>(x,y)[0]=255+'\0';
    image.at<cv::Vec3b>(x,y)[1]=0+'\0';
    image.at<cv::Vec3b>(x,y)[2]= 0+'\0';
   }


   Mat res_image;
 cv::resize(image, res_image, Size(w, h-1), INTER_LINEAR);

 
  for(int j=0;j<w;j++)
  {
    int flag=0;
    for(int i=0;i<h;i++)
    {
        if(x_path_horizontal[j]==i && y_path_horizontal[j]==j)
        {
        flag=1;    
        continue;
        }
       
    res_image.at<cv::Vec3b>(i-flag,j)[0]=image.at<cv::Vec3b>(i,j)[0];
    res_image.at<cv::Vec3b>(i-flag,j)[1]=image.at<cv::Vec3b>(i,j)[1];
    res_image.at<cv::Vec3b>(i-flag,j)[2]=image.at<cv::Vec3b>(i,j)[2];

    }
  }
     imshow("Display Image", image);
      cv::waitKey(1);

  return res_image; 

  //return image;
}

int main(int argc, char** argv )
{


string image_path;
cout<<"Give path of image"<<endl;
cin>>image_path;

cout<<"Give size of image you want in (height,width) format"<<endl;

int res_height;
int res_width;

cin>>res_height;
cin>>res_width;

 Mat image;
 image = imread( image_path, IMREAD_COLOR );
 if ( !image.data )
 {
 printf("No image data \n");
 return -1;
 }

//Mat k = image;
 namedWindow("Display Image", WINDOW_AUTOSIZE );
imshow("Display Image", image);

int height = image.rows;
int width = image.cols;

if(res_height<=0 || res_width<=0 || res_height>height || res_width>width)
{
  cout<<"Please specify size in range"<<endl;
  return -1;

}
//  cout<<image.rows<<endl;
//  cout<<image.cols<<endl;


//cout<<image.at<cv::Vec3b>(0,0)[0]-'\0';



int w = width;
int h = height;



while(w>res_width)
{
   image=seam_carving_vertical(w,h,image);
   w--;
}

imshow("Display Image", image);
   
while(h>res_height)
{
  image=seam_carving_horizontal(w,h,image); 
  h--;
}

imshow("Display Image", image);
//cout<<image.at<uchar>(0, 0)[0];

 cv::waitKey(0);
 return 0;
}
