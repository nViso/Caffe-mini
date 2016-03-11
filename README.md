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
 
## About the Complex Test Example
The test example is based on a complicated face landmark model, you can find the input files in the folder "complex_example". In this folder, there are both v2 and v0 models, you can also get the v0 model from v2 with codes in downgrade_proto.cpp. In order to test the model, directly run test_complex.cpp, with input:

 *.prototxt weight_file(which is the .caffemodel file in the up-to-date Caffe) number_of_iterations input_images output_directory CPU/GPU(only support CPU)
 
 for example:
 
 v0_proto.prototxt v0_weights.caffemodel 1 image_data.binaryproto result  CPU
 
 
 The example also shows how to read v2 binaryproto file to our model.
 
## Future Work

Update the codes, make it more clear.
	

