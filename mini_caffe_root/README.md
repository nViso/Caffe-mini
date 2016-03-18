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