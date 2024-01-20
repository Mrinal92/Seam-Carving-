#include<iostream>
#include<cmath>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

double CalcEmat(Mat& , int, int, int, int);
int miin( int , int , int );

int main()
{   
    Mat img = imread("Sample1.jpg");
    

    int h=img.rows;
    int w=img.cols;
    
    Mat threeD(h,w, CV_8UC3);

    for(int i=0; i<h; i++)
    {
        for(int j=0; j<w; j++)
        {
            Vec3b p= img.at<Vec3b>(i,j);
            unsigned char R=p[2];
            unsigned char G=p[1];
            unsigned char B=p[0];
            threeD.at<Vec3b>(i,j)=Vec3b(B,G,R);
        }
    }

   

    Mat Emat(h,w,CV_64F);
     
    for(int i=0; i<h; i++)
    {
        for(int j=0; j<w; j++)
        {
            Emat.at<double>(i,j)= CalcEmat(threeD,i,j,h,w); 
        
        }
       
    }

    double** SeamCarv = new double*[h];
    for(int i=0;i<h;i++)
    {
        SeamCarv[i]=new double[w];
    }

     for(int i=0;i<w;i++)
    {
        SeamCarv[0][i] = Emat.at<double>(0,i);
    }

     for(int i=1; i<h; i++)
    {
        for(int j=0; j<w; j++)
        {  
            int Left=j-1;
            if(j<=0)
            Left=0;
            int Right=j+1;
            if(j>=w-1)
            Right=w-1;
            
            SeamCarv[i][j]= Emat.at<double>(i,j) + miin( SeamCarv[i-1][Left], SeamCarv[i-1][j], SeamCarv[i-1][Right]);
        }
        
    } 
    

    
    int RemoveW;
    cout<<"Enter the Horizontal Reduction (Number of columns to decrease)"<<endl;
    cin>>RemoveW;

    int RemoveH;
   // cout<<"Enter the vertical Reduction (Number of rows to decrease)"<<endl;
   // cin>>RemoveH;


    while(RemoveW--)
    {
    
    
    for(int i=0; i<h; i++)
    {
        for(int j=0; j<w; j++)
        {
            Emat.at<double>(i,j)= CalcEmat(threeD,i,j,h,w); 
            
        }
       
    }
    
         for(int i=1; i<h; i++)
    {
        for(int j=0; j<w; j++)
        {  
            int Left=j-1;
            if(j<=0)
            Left=0;
            int Right=j+1;
            if(j>=w-1)
            Right=w-1;
            
            SeamCarv[i][j]= Emat.at<double>(i,j) + miin( SeamCarv[i-1][Left], SeamCarv[i-1][j], SeamCarv[i-1][Right]);
        }
        
    }

    int Min=0;
    

    for(int i=0;i<w;i++)
    {
        if(SeamCarv[h-1][i] < SeamCarv[h-1][Min])
        Min=i;
    }
    

    // backtracking

    int SeamArr[h];

    SeamArr[h-1]=Min;
    int temp=0;
    int posval;

    
    //SeamCarv[h-1][Min] = 255;
    for(int i=h-2;i>-1;i--)
    {
       temp=SeamArr[i+1];
       posval=temp;
   
       
       int left=posval-1;
       int right=posval+1;
       int top=posval;
       
       if(posval<=0)
       left=w-1;

       if(posval>=w-1)
       right=0;

       int topleft=SeamCarv[i][ left];
       int topcenter=SeamCarv[i][ top];
       int topright=SeamCarv[i][ right];
        
    

       if(topleft==miin(SeamCarv[i][left], SeamCarv[i][ top], SeamCarv[i][right]))
       SeamArr[i]= posval-1;

       else if(topcenter==miin(SeamCarv[i][ left], SeamCarv[i][top], SeamCarv[i][ right]))
       SeamArr[i]= posval;

       else if(topright==miin(SeamCarv[i][ left], SeamCarv[i][top], SeamCarv[i][right]))
       SeamArr[i]= posval+1;
       

       else;
      

    }


       for(int i=0; i<h; i++)
    {
        for(int j=SeamArr[i]; j<w-1; j++)
        { 
           img.at<Vec3b>(i,j)=img.at<Vec3b>(i,j+1);
        }
        
    } 
    
    img=img(Rect(0,0,w-1,h));


    w--;
    }

 //--------------------------------------------------------------------------vertical
/* while(RemoveH--)
    {
    
    
    for(int i=0; i<h; i++)
    {
        for(int j=0; j<w; j++)
        {
            Emat.at<double>(i,j)= CalcEmat(threeD,i,j,h,w); 
            
        }
       
    }

  for(int i=0;i<h;i++)
    {
        SeamCarv[i][0] = Emat.at<double>(i,0);
    }

    
         for(int j=1; j<w; j++)
    {
        for(int i=0; i<h; i++)
        {  
            int upar = j-1;
            int leftside = i-1;
            if(i-1<0)
            leftside = 0;
            int rightside = i+1;
            if(i+1>=h)
            rightside = h-1;
            
            SeamCarv[i][j]= Emat.at<double>(i,j) + miin( SeamCarv[leftside][upar], SeamCarv[rightside][upar], SeamCarv[i][upar]);
        }
        
    }

    int Min=0;
    

    for(int i=0;i<h;i++)
    {
        if(SeamCarv[i][h-1] < SeamCarv[Min][h-1])
        Min=i;
    }
    

    // backtracking

    int SeamArr[w];

    SeamArr[w-1]=Min;
    int temp=0;
    int posval;

    
    //SeamCarv[h-1][Min] = 255;
    for(int i=w-2;i>-1;i--)
    {
       temp=SeamArr[i+1];
       posval=temp;
   
       
       int left=posval-1;
       int right=posval+1;
       int top=posval;
       
       if(posval<=0)
       left=w-1;

       if(posval>=w-1)
       right=0;

       int topleft=SeamCarv[i][ left];
       int topcenter=SeamCarv[i][ top];
       int topright=SeamCarv[i][ right];
        
    

       if(topleft==miin(SeamCarv[i][left], SeamCarv[i][ top], SeamCarv[i][right]))
       SeamArr[i]= posval-1;

       else if(topcenter==miin(SeamCarv[i][ left], SeamCarv[i][top], SeamCarv[i][ right]))
       SeamArr[i]= posval;

       else if(topright==miin(SeamCarv[i][ left], SeamCarv[i][top], SeamCarv[i][right]))
       SeamArr[i]= posval+1;
       

       else;
      

    }


       for(int i=0; i<h; i++)
    {
        for(int j=SeamArr[i]; j<w-1; j++)
        { 
           img.at<Vec3b>(i,j)=img.at<Vec3b>(i,j+1);
        }
        
    } 
    
    img=img(Rect(0,0,w,h-1));


    h--;
    }*/


    
    string resized="Resized_sample.jpg";
    cv::imwrite(resized, img);


    
    
    
    return 0;
}

double CalcEmat(Mat& threeD, int i, int j, int h, int w)
{
   
   int l,r,u,d;
   l=j-1;
   r=j+1;
   u=i-1;
   d=i+1;

   if(l<0)
   l=w-1;
   if(r>=w)
   r=0;
   if(u<0)
   u=h-1;
   if(d>=h)
   d=0;
   
 

  Vec3b up = threeD.at<Vec3b>(u,j);
Vec3b down = threeD.at<Vec3b>(d,j);
Vec3b left = threeD.at<Vec3b>(i,l);
 Vec3b right = threeD.at<Vec3b>(i,r);
 
 
  double x= pow(abs(right[2]-left[2]),2)+ pow(abs(right[1]-left[1]),2) + pow(abs(right[0]-left[0]),2);
  double y= pow(abs(up[2]-down[2]),2)+ pow(abs(up[1]-down[1]),2) + pow(abs(up[0]-down[0]),2);
 
  return sqrt(y+x);
}

int miin( int a, int b, int c)
{  

  int temp=0;
  temp=min(a,b);

  if(c<temp)
  return c;

  else
  return temp;
}
