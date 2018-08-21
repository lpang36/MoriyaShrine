# Project Overview 
The purpose of our project is to provide intelligent mood lighting for a small area, based on environmental cues. The lighting color can be generated in three ways: based on averaging image color, background color behind a laser pointer, and background color behind a person’s face. In addition, the project logs its results with warnings and errors, and improves its own face detection model iteratively.  

This project is about several themes: intelligent image processing for recognizing certain elements, processing data using the limited capabilities of an embedded system on a chip (SoC), interfacing analog output with an embedded SoC, and interacting with compiled programs through system scripts. 

Our original plan was to have it aim a laser pointer at humans and geese, but the objective could not be fulfilled as we had issues with the motors, and we lacked access to the necessary C++ libraries on the Omega2. Our current project does not have motors and does not include the machine learning aspects of the original plan; however, it retains the webcam and much of the software for image processing, object detection, and logging purposes. 

# Software Design 
## Software Overview 
The entire project is run through the MountainOfFaith.sh shell script. This script reads in input from the webcam, and saves it in image.jpg. It then calls Main.cpp, which contains the main logic of the project, with two arguments: the mode of operation, and the name of the image file. As outlined above, there are three modes of operation: face detection, laser pointer detection, and averaging across the entire image. The Main.cpp file will write output to the output.txt file, which is then read by the shell script. The values contained in output.txt will be numbers from 0-100, which indicate PWM values to be written to the RGB LED. A higher PWM indicates the corresponding LED will be brighter.  

The main logic for this project is contained in Main.cpp. Based on user input, it will be passed two command line arguments: the first will be an integer from 0-2, and the second will be the name of the image file, in JPEG format. The first argument indicates the mode of operation. 0 is for face detection, 1 is for laser pointer detection, and 2 is for averaging across the entire image. 

The main method will first read the JPEG image with stb_image, an open source library for reading image files. It will then convert the data in the image to an object of Image class. The Image class is defined in Image.h, and its methods are implemented in Main.cpp. Depending on the mode of operation, it will then attempt to recognize a target area, which may be a face, a laser pointer, or the entire image. If it is unsuccessful in doing so, it will output a warning to the log file and default to processing the entire image. Then, it will compute the average color over and around the target area, and write this to the output.txt file, which is used to write output to the RGB LED. 

## State Machine Diagram 

![](images/statemachine.jpg)

## Function Call Trees 
The diagrams below are the function call trees for each of the three modes of operation. System functions, such as file writing and reading, and functions used multiple times, such as getCurrentTime and constructors, are omitted or shown only once for clarity. 
### Face Detection Call Tree 

![](images/facedetection.jpg)

### Laser Pointer Detection Call Tree 

![](images/laserdetection.jpg)

### Entire Image Call Tree 

![](images/imagetree.jpg)

## Classes and Methods 
### Image Processing 
The image processing software component of this project has two components: a) face detection and b) detection of a laser pointer. The face detection component is based on the work of Darrell et al. (1998), who outlined a relatively simple face detection algorithm. The algorithm was adjusted to adapt to the constraints of this project. A rudimentary form of machine learning was also implemented so as to improve the face detection algorithm automatically. 

The foundation of the image processing being performed is the Image class, defined in Image.h and implemented in Main.cpp. The members and functions of this class are described as follows: 

### Image Class Members 
* int width, height, depth: The dimensions of the image. Depth is the number of color channels - usually 3 for red, green, and blue in that order, or 1 for grayscale images.
* std::vector< std::vector< std::vector<int> > > mat: 3-dimensional vector containing the numerical values of the image. Each element is an integer between 0 and 255 inclusive. Vectors were chosen rather than arrays due to ease of use.
* std::vector< std::vector<bool> > valid: 2-dimensional vector that acts as a filter for the image, of size width x height. Based on various operations, each pixel can be set to a boolean value based on this vector.

### Image Class Methods 
* Image(): Default constructor.
* Image(std::vector< std::vector< std::vector<int> > > img): Initializes mat to the parameter img. Also initializes valid to all true by default.
* Image(std::vector< std::vector< std::vector<int> > > img, std::vector< std::vector<bool> > val): Initializes mat from the parameter img, and valid from the parameter val.
* ~Image(): Default destructor.
* void colorFilter(int r, int g, int b, double tolerance): If the color of a pixel is not sufficiently close (defined by the tolerance parameter) to the value defined by (r,g,b), its filter is set to false. The metric for closeness is defined logarithmically, based on the method outlined by Darrell et al. (1998). This is used to find colors in the image similar to skin tone, for face detection. 
* void erode(int r): Erosion is an image morphological operation. For each pixel, if not all pixels within an area of r x r are true, that pixel is set to false. This is used to eliminate noise for face detection.
* void dilate(int r): Dilation is an image morphological operation. For every true pixel, set all pixels within an area of r x r to true. This is used to eliminate noise for face detection.
* std::vector<int> largestConnComp(Image& img): Finds the bounding box for the largest connected component of true pixels. Returns a vector in the form [smallest x value, smallest y value, width, height]. A connected component is one in which every true value is connected to at least one other true value. First, this function finds the size of the largest connected component and keeps track of one pixel in that component. Then, the bounds on that connected component are found. Both of these steps are accomplished by the flood-fill algorithm. The flood-fill algorithm starts at some true pixel, and adds all true neighbors of that pixel to a stack. Then it pops an element from the stack, and repeats this process until the stack is empty. This way, all true pixels connected to the starting pixel are entered in the stack at some point. Finally, all the data within the bounds of the largest connected component is written to the img parameter, passed by reference. This is used to find the areas in an image where a face or laser pointer is most likely to be.
* void subtractColor(int r, int g, int b): Subtracts an RGB value from all pixels. This is used to produce an image of the facial area.
* std::vector<int> averageColor(std::vector<int> bounds, bool val): Average color of image within some bounds. Returns vector in format [average R, average G, average B]. If parameter val is true, the average color is taken across all pixels where the filter is also true; otherwise, the average is only across pixels where the filter is false. This is useful in the case of finding the background color behind a face or laser pointer, as it is undesirable to include the actual color of the face or laser pointer. This is used to write an average color value to the RGB LED.
* void flatten(): Converts to grayscale by averaging R, G, B values. This is used to produce an image of the facial area.
* void threshold(int thresh): thresholds a grayscale image. All pixels with a value less than thresh have their filters set to false. This is used to produce an image of the facial area, and to detect bright spots (which could be a laser pointer).
* void scaleDown(int w, int h): Scales an image down to size w x h. Uses a basic averaging algorithm. This is used to produce an image of the facial area, to compare to a standard image.
* int hammingDist(Image img): Computes sum of absolute differences between each pixel in two images. This is used to determine if an area in the image contains a face.
* std::vector<int> detectFace(Image& standard, int r, int g, int b, int& loss, std::vector<int>& params, const double IMG_LEARNING_RATE, std::ofstream* logfile, std::string time): Overall function which detects faces. More explanation below.

### Face Detection Algorithm 
First, the image is filtered based on similarity to skin color. Then, an opening morphological operation is performed (this consists of erosion followed by dilation). The purpose of this is to eliminate noise, such as very small connected components or irregular edges. The largest connected component is then found. A face, if one exists in the image, is likely to be located here. The image is then cropped to only where this connected component is located. The average face color is subtracted from the image, and it is thresholded and converted to grayscale. This produces an image of the face. It is then scaled down to 16x16, to be compared to a standard facial image, contained in the file std.txt. If the hamming distance between the two images is sufficiently small, the face is considered valid.  

### Laser Pointer Detection Algorithm 
This algorithm is considerably simpler than the face detection algorithm. The image is converted to grayscale and then thresholded. Typically, as our webcam produces rather dark images, the laser will be the only object sufficiently bright. However, if other bright objects are in the image, some checks are implemented to see if the detected area is really a laser. If the area is small enough and the most common color is red (we are using a red laser), it is considered a laser. Otherwise, we repeat this process with a different area, until a laser is found or all areas are processed. 

### Other Methods 
string getCurrentTime(): Defined in Main.cpp but not part of the Image class. This function returns the current timestamp as a string. It is used for logging purposes. 

## System-Dependent Components 
System-dependent code is that which functions specifically on the Omega2. The code contained in Main.cpp is generally not system-dependent. The primary interaction between the Omega2 and the software is through the shell file. The majority of shell file commands can be executed on any generic UNIX-based environment (provided that the ash or bash shell is present). However, the writing of PWM values to the RGB LED is system-dependent, making use of the Omega2’s GPIO pins and the fast-gpio package built in. 

## System-Independent Components 
Most of the software is system-independent. The image processing methods and algorithms in the Image class can operate similarly on any platform. The main logic of the software, contained in Main.cpp, is also independent of the Omega2. Receiving input from the webcam and storing it to memory through the shell file is mostly system-independent, only with the note that the fswebcam package must be installed (and of course, it must be run on a UNIX system with a shell that supersedes ash). Finally, the logging infrastructure, which will be further explained below, is system-independent as well. 

## Logging Infrastructure 
In total, the software component of this project interacts with four different files: Image.jpg, output.txt, logfile.txt, and std.txt. Image.jpg is the image read in from the webcam, output.txt contains the RGB values written to the LED, logfile.txt contains a standard log, and std.txt contains the values of a standard facial image, which is used in the facial detection algorithm. 

### Logging 
Log statements written to logfile.txt may take on one of three levels of significance. Normal statements concerning the day-to-day operation of the project include outputting the location and dimensions of any faces or laser pointers detected, and the RGB values written to the LED. It also logs important function calls, such as to different image processing algorithms in the Image class, for debugging purposes.  
The second level includes warning statements, which are prefixed by "Warning: ". These will be logged if there is no face or laser pointer detected in the image.  

Finally, there are error statements, prefixed by "Error: ". These will be logged in the case of insufficient command line arguments, or failure to open a file. In every case, the log statement is prefixed by a timestamp, returned by the getCurrentTime() function in Main.cpp. 

## Standard Face Image 
The standard face image is contained in std.txt. While it is used in facial detection, it also serves another purpose. Although the Omega2 is not powerful enough to support true machine learning, a very rudimentary form of machine learning could still be implemented where the algorithm can continuously self-improve as it makes more face detections.  

If a face is detected in the image, the average color of the face is found. A weighted average of these values and the original "standard" face color is done, which becomes the new face color to be used in filtering the next image. A similar operation is done on the image of the face. The previous image is updated with a weighted average of the new values. At the end, the updated image and color are written to the std.txt file, so these values can be reused when the program executes again. Ideally, if the user of the product scans many images of their face, the values in std.txt will resemble the user’s own face more and more closely. This allows the face detection algorithm to continuously improve itself, without the need for external input – a basic form of unsupervised machine learning.  

## Shell Script 
The interfacing between the hardware and the software on the Omega2 was done through a UNIX shell script written for the ash shell (natively run on the Omega2).  

The script opens with ASCII art (inspired by the Omega2’s “What will you invent” banner). Interfacing with the webcam (a Logitech C170) was done through the package fswebcam 
(installed through opkg, natively built for openwrt on the MIPS-24K architecture). The fswebcam program was run with the flags -p YUYV which sets the colour encoding system in the file to the YUYV format, (the only one suitable for the C170), --no-banner, which disables the bottom banner with date/time that is written by default by fswebcam, and -r 300x300, which sets the resolution to fit the size of a 300x300px image (since the aspect ratio was not 1:1, the image ended up 352x288). Finally, the parameter was set to image.jpg, specifying the output file name. 

The next steps in the script are to clear the logfile from the previous run (as logfiles build up quickly due to the amount of data stored within them) and call the Main C++ program with the parameters passed in.  

The script sets up the output file to be read, then reads it line by line and writes the value to the output pins accordingly. Output pins 0-2 were set up to the LED pins for R,G,B respectively, and written using the fast-gpio package available. It sets them to output pwm (software pwm provided through digital outputs with sequences of delays in between) at 500Hz, with a on-time percentage specified by the output file. No error checking was done for the output file, as the values must have been set by the C++ program to fit the required format. Finally, the logfile is opened and displayed for demo purposes. 

## Cross-Compilation Considerations 
Cross-compilation was done utilizing the LEDE buildchain for the MIPS-24K architecture. The toolchain was set up on a local Arch Linux system referencing notes created by the TA’s. The toolchain was modified to allow compilation of C++ programs and work with fast-gpio. xCompile.sh and the makefile can be found in the appendix with minor modifications made from the original example files; notable changes include changing the compiler to g++ instead of gcc and editing target names.  

## Additional Aspects 
A summary of the source files of the project is listed below. 

### C/C++ Files 
* Main.cpp: main logic of the program, and implements methods in the Image class.
* Image.h: defines the Image class.
* stb_image.h: open-source public-domain library used to read image files.

### Shell Files: 
* MountainOfFaith.sh: runs the program, interacts with hardware and calls C/C++ files.
* xCompile.sh: cross-compilation script for the program.

### Other Files: 
* std.txt: contains a standard face image.
* makefile: makefile for cross-compilation.

# Hardware Design 
The hardware consists of four basic components: the Omega2 SoC, the Logitech C170 webcam, the common cathode RGB LED, and the toilet paper roll.  

The C170 webcam is an inexpensive VGA-quality webcam that captures images in the YUYV colour space. This webcam was chosen due to its plug-and-play capabilities (using fswebcam, as specified above in the shell script description), the fact that high resolution pictures were not necessary, and its price. It was connected to the Omega2 simply using its inbuilt USB interface.  

The common cathode RGB LED was wired to the Omega2 using a simple breadboard. It receives R, G, B values from three separate pins, read as analog inputs. The higher the voltage that was passed to the pin, the brighter the respective colour would be. The common cathode pin (as per its name) grounds the LED, and was connected to the Omega2’s grounding pin. 

The toilet paper roll was a fairly last-minute design decision that was deemed necessary due to the inherent “separation” of colour characteristic of RGB LED’s. In order to provide a suitable colour output that was representative of the scene, the three colours had to be diffused slightly and combined into one. Furthermore, the toilet paper roll prevents any damage to the eyes from accidentally looking straight into the LED. 

The Omega2 SoC was where all the commands were run from. GPIO pins 0, 1, and 2 were wired to RGB LED pins for R, G, and B, and the ground pin was connected to ground the LED. The C170 webcam was attached through USB. Control for the Omega2 was done through ssh from a laptop connected to the Omega2’s local network. 