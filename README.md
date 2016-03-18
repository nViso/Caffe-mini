# Caffe-mini
Low dependents caffe for testing phase

## Dependence

As a mini Caffe, this one could directly test a trained model. We remove most dependence but still keep 2 libraries, in order to make it could load prototxt and input image.

The 2 dependence is Protobuf and Opencv, for the protobuf, it already exist in the project, you can directly use it. For the Opencv, you need to install it by yourself and add it to the project.

The Opencv version I use is 2.4.11, and I cmake it to support vc 14 for x86 structure.  Here is the guidance show how to cmake it 

http://amin-ahmadi.com/2015/12/04/how-to-build-opencv-from-source-for-vc14/

You need first download the opencv from official webside, and then cmake it according to this guidance. After doing this, you need to first add it to your envirment variables and then set it in your visual studio. Here is the guidance show how to set environment variables and add it to visual studio.

http://opencv-srf.blogspot.ch/2013/05/installing-configuring-opencv-with-vs.html

Note: instead of vc11 you need vc14 for visual studio 2015, modify this in all the pathes. As the bin folder contains both debug and release, you should add 2 paths to the PATH environment variable, they are %OPENCV_DIR%\x86\vc14\bin\Debug and %OPENCV_DIR%\x86\vc14\bin\Release, rather than the %OPENCV_DIR%\x86\vc11\bin in the above guidance. What's more, in the visual studio, in debug mode, add $(OPENCV_DIR)\x86\vc14\lib\Debug and list the *.libs in the Debug folder. In release mode, add $(OPENCV_DIR)\x86\vc14\lib\Release and list the *.libs in the Release folder(the linker general and input part). You can add a new filder called "opencv2" and copy all the files in opencv\include\opencv2 to this folder.

Now you have already set all the dependence, if visual studio can not open head files in opencv2, close visual studio and open it again, remember to save the changes before close it.

## Compile it with Visual Studio 2015
First clone this project from the original git:

    git clone https://github.com/ufoym/Caffe-mini.git

Then create an empty project with Visual Studio 2015. In the Solution Explorer, right click Source Files -> Add Existing Item, then choose the caffe.cpp from the project you forked from git.

Before you can run it, you must do these things:

    Right click on your project's entry in solution explorer,
	
    Select Properties,
	
	Choose C/C++, then click the General,
	
	In the "Additional Include Directories", add the whole project you forked from git, it is \path\to\Caffe-mini
	
	Still in C/C++, Select Preprocessor,
	
    add these flags to "Preprocessor Definitions":
	
        USE_EIGEN
		
        _CRT_SECURE_NO_DEPRECATE
		
        _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS=1
        
    one flag per line.
	
After this, choose Source Files in Solution Explorer, right click and add New Filter, the Filter name should be 'caffe', and then copy all the files in the folder 'caffe' from the git project, to this New Filter

Create another New Filter called 'google', again, copy all the files in the original 'google' folder to this new Filter

Now, you can build your own solution

## Version Problem

This Caffe Mini is based on the Caffe version 0.99 from https://github.com/BVLC/caffe, so the caffe.proto file is different from the latest one. In order to run this Caffe, make sure your .prototxt file and weights file (the caffe model) is the same format as the version 0.99. As the model changes since Caffe 0.999, V0 Caffe can not test the model trained with Caffe newer than version 0.99.

In order to solve this problem, I write a code called downgrade_proto.cpp, which could transfer simple v2 model to v0 model (like lenet), both prototxt and the binary weights. As the Caffe is complicated, I only consider several widely used layers and fields, it may fail to transfer some complex networks if it contains the layers I don't add in the code. You can add these layers by yourself, it is quited simple. So far, it supports most layers but it could miss some prarameters which don't exist in the v0 caffe.proto (for the layers don't exist in v0 caffe model, I just copy the type and bottom and top information, as they are not the padding layers, so in most case it doesn't affect the performance), if you face some problems, please check this.

## How to downgrade V2 model to V0

Right click on your project's entry in solution explorer,
	
    Select Properties,
	
	Choose C/C++, then click the General,
	
	In the "Additional Include Directories", add the "new_caffe" folder you forked from git, it is \path\to\new_caffe
	
	run the downgrade_example.cpp code , input 4 parameters:
	
	v2_prototxt v2_caffemodel v0_output_prototxt v0_output_caffemodel
	
	for example:
	
	lenet.prototxt lenet_iter_10000.caffemodel v0Proto.prototxt v0Weight.caffemodel

 

## About the Test Example
The test example is based on the Lenet network and mnist data set, in order to test this model, just input:

 *.prototxt weight_file(which is the .caffemodel file in the up-to-date Caffe) number_of_iterations input_images output_directory CPU/GPU(only support CPU)
 
 for example:
 
 lenet.prototxt lenet_iter_10000 1 t10k-images.idx3-ubyte result  CPU
 
 Make sure to create the result folder before test it
 
## About the Complex Test Example
The test example is based on a complicated face landmark model, you can find the input files in the folder "complex_example". In this folder, there are both v2 and v0 models, you can also get the v0 model from v2 with codes in downgrade_proto.cpp. In order to test the model, directly run test_complex.cpp, with input:

 *.prototxt weight_file(which is the .caffemodel file in the up-to-date Caffe) number_of_iterations input_images output_directory CPU/GPU(only support CPU)
 
 for example:
 
 v0_proto.prototxt v0_weights.caffemodel 1 image_data.binaryproto result  CPU
 
  Make sure to create the result folder before test it
 
 
 The example also shows how to read v2 binaryproto file to our model.
 
## Details About the Landmarks Detection Model
 
 
 
 
 
## About the Grayscale Image Example(test_images.cpp)
This test example is based on a complicated face landmark model, you can find the input files in the folder "grayscale_example". In this folder, there are both v2 and v0 models, you can also get the v0 model from v2 with codes in downgrade_proto.cpp. In order to test the model, directly run test_complex.cpp, with input:

 *.prototxt weight_file(which is the .caffemodel file in the up-to-date Caffe) number_of_iterations input_images output_directory CPU/GPU(only support CPU)
 
 for example:
 
 v0proto.prototxt v0weights.caffemodel 1 0002d5c7-cc03-4475-93bc-7c3c85650119.png result CPU
 
  Make sure to create the result folder before test it
 
 
 The example shows how to read grayscale image to our test model.
 
## Future Work

Update the codes, make it more clear.
	

#User Instructor of This Mini Caffe

In this readme file, we give the description of the model, examples on how to use it, the output it returns and details about the input images.

##Model Description

This model computes the facial landmarks as well as the yaw, roll and pitch head angles. The landmark values are scaled between -10 and 10 where -10 corresponds to a pixel value of 0 and 10 corresponds to the maximum pixel value, in this case 40.

- the 1st layer is convolution layer "conv0", for which the filter size is 5 and stride is 1. There are 96 outputs in this layer.
- the 2nd layer is relu layer called "relu0" and the 3rd layer is a pooling layer named "pool0". In this max pooling layer, the filter size is 2 and stride is 2.
- the 4th layer is convolution layer "conv1", for which the filter size is 3 and stride is 1. There are 256 outputs.
-  the 5th layer is relu layer called "relu1" and the 6th layer is a pooling layer named "pool1". In this max pooling layer, the filter size is 2 and stride is 2.
-  the 7th layer is convolution layer "conv2", for which the filter size is 3 and stride is 1. There are 384 outputs in this layer.
-  the 8th layer is relu layer called "relu2" and the 9th layer is a pooling layer named "pool2". In this max pooling layer, the filter size is 2 and stride is 2.
-  the 10th layer is convolution layer "conv3", for which the filter size is 3 and stride is 1. There are 384 outputs in this layer.
-  the 11th layer is relu layer called "relu3" and the 12th layer is a pooling layer named "pool3". In this max pooling layer, the filter size is 2 and stride is 2.
-  the 13th layer is inner product layer "ip0_1" with 256 outputs, the 14th layer is the relu layer "relu0_1". The 15th layer is dropout layer called "drop0_1" with dropout ratio 0.5.
-   the 16th layer is inner product layer "ip0_2" with 256 outputs, the 17th layer is the relu layer "relu0_2". The 18th layer is dropout layer called "drop0_2" with dropout ratio 0.2.
-   the 19th layer is inner product layer "ip0_3" with 64 outputs, which are the landmarks. 
-   the 20th layer is inner product layer "ip1_1" with 256 outputs, the 21st layer is the relu layer "relu1_1". The 22nd layer is dropout layer called "drop1_1" with dropout ratio 0.5.
-   the 23rd layer is inner product layer "ip1_2" with 256 outputs, the 24th layer is the relu layer "relu1_2". The 25th layer is dropout layer called "drop1_2" with dropout ratio 0.5. 
-   the 26th layer is inner product layer "ip1_3" with 1 output, which is the yaw angle.
-    the 27th layer is inner product layer "ip2_1" with 256 outputs, the 28th layer is the relu layer "relu2_1". The 29th layer is dropout layer called "drop2_1" with dropout ratio 0.5.
-    the 30th layer is inner product layer "ip2_2" with 256 outputs, the 31st layer is the relu layer "relu2_2". The 32nd layer is dropout layer called "drop2_2" with dropout ratio 0.5.
-    the 33rd layer is inner product layer "ip2_3" with 1 output, which is the pitch angle.
-    the 34th layer is inner product layer "ip3_1" with 256 outputs, the 35th layer is the relu layer "relu3_1". The 36th layer is dropout layer called "drop3_1" with dropout ratio 0.5.
-    the 37th layer is inner product layer "ip3_2" with 256 outputs, the 38th layer is the relu layer "relu3_2". The 39th layer is dropout layer called "drop3_2" with dropout ratio 0.5.
-    the 40th layer is inner product layer "ip3_3" with 1 output, which is the roll angle.

All these outputs are computed by the corresponding inner product layers. Among all the outputs, the landmark is computed by layer "ip0_3", and it returns 64 float numbers which are the X,Y coordinates of 32 landmarks. The yaw is computed by layer "ip1_3", while pitch is computed by
"ip2_3" and the roll is "ip3_3". The architecture of this network is shown in models/v0proto.prototxt and the weights of this network are in models/v0weights.caffemodel.

The details of how to get these outputs are in src/test_images.cpp.

##How to Use 

First add the folder which contains the file "caffe-mini.exe" to your PATH environment variable. 

Then run the following in the command line:
 
caffe-mini.exe para1 para2 para3 para4

- para1: path to the file "v0proto.prototxt", like path/to/v0proto.prototxt

- para2: path to the file "v0weights.caffemodel", like path/to/v0weights.caffemodel

- para3: path to the input image, like path/to/image.png

- para4: path to the folder which store the result, like path/to/result

example input: 

in command line, if we are in mini_caffe_root folder:

caffe-mini.exe models/v0proto.prototxt models/v0weights.caffemodel test/new_images/000d5ac6-6e40-4570-8c8b-1a7fd16c7bcc.png test/output

Remember to make sure all the paths exist before you run the file, including the folder which you store all the outputs.

## Output 

The output contains information about the landmark and head poses(yaw, roll and pitch) and it is stored in json file format.

## Image Details

The input image should be 40*40, grayscale human face picture. You can find the example pictures in test/new_images.

## Test Example
In the folder test/new_images, you can find the sample input images. You can also predict the landmarks and head poses to the folder test\output. The folder test/true_outputs contains the true values which you can compare with your own prediction. 

We also offer one script to check if your prediction is correct, it is in test/true_outputs, you have to cd to this folder in order to execute it. After you cd to this folder just type this in your command line:

python test_results.py path/to/your/output/folder