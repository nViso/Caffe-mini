# Caffe-mini
Zero dependents caffe for testing phase

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

This Caffe Mini is based on the Caffe version 0.99 from https://github.com/BVLC/caffe, so the caffe.proto file is different from the latest one. In order to run this Caffe, make sure your .prototxt file and weights file (the caffe model) is the same format as the version 0.99. As the model changes since Caffe 0.999, our model can not test the model trained with Caffe newer than version 0.99.

## About the Test Example
The test example is based on the Lenet network and mnist data set, in order to test this model, just input:

 *.prototx weight_file(which is the .caffemodel file in the up-to-date Caffe) number_of_iterations input_images output_directory CPU/GPU(only support CPU)
 
 for example:
 
 lenet.prototxt lenet_iter_10000 1 t10k-images.idx3-ubyte result  CPU
 
 ##Future Work
 Make this Caffe Mini support trained model from the up-to-date Caffe
	

