// Copyright 2013 Yangqing Jia
//
// This is a simple script that allows one to quickly test a network whose
// structure is specified by text format protocol buffers, and whose parameter
// are loaded from a pre-trained network.
// Usage:
//    test_net(the .prototxt file) weight(the .cafemodel file) iterations(the number of iterations) input-image output-folder [CPU/GPU]

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <string>
#include<iostream>
#include<stdlib.h>
#include<infstr.h>
#include <fstream>
#include <google/protobuf/text_format.h>


#include "caffe/caffe.hpp"

using namespace caffe;  // NOLINT(build/namespaces)
using namespace std;
using namespace google::protobuf;

template <typename Dtype>
static void save_blob(const string& fn, Blob<Dtype> *b){
	LOG(INFO) << "Saving " << fn;
	FILE *f = fopen(fn.c_str(), "wb");
	CHECK(f != NULL);
	for (int i = 0; i < b->count(); i++)
	{
		fprintf(f,"%f " , b->cpu_data()[i]);
	}
	fclose(f);
}


// add the code to read mnist data

int ReverseInt(int i)
{
	unsigned char ch1, ch2, ch3, ch4;
	ch1 = i & 255;
	ch2 = (i >> 8) & 255;
	ch3 = (i >> 16) & 255;
	ch4 = (i >> 24) & 255;
	return((int)ch1 << 24) + ((int)ch2 << 16) + ((int)ch3 << 8) + ch4;
}
void ReadMNIST(string filename, int NumberOfImages, int DataOfAnImage, vector<vector<double>> &arr)
{
	arr.resize(NumberOfImages, vector<double>(DataOfAnImage));
	ifstream file(filename, ios::binary);
	if (file.is_open())
	{
		int magic_number = 0;
		int number_of_images = 0;
		int n_rows = 0;
		int n_cols = 0;
		file.read((char*)&magic_number, sizeof(magic_number));
		magic_number = ReverseInt(magic_number);
		file.read((char*)&number_of_images, sizeof(number_of_images));
		number_of_images = ReverseInt(number_of_images);
		file.read((char*)&n_rows, sizeof(n_rows));
		n_rows = ReverseInt(n_rows);
		file.read((char*)&n_cols, sizeof(n_cols));
		n_cols = ReverseInt(n_cols);
		for (int i = 0; i<number_of_images; ++i)
		{
			for (int r = 0; r<n_rows; ++r)
			{
				for (int c = 0; c<n_cols; ++c)
				{
					unsigned char temp = 0;
					file.read((char*)&temp, sizeof(temp));
					arr[i][(n_rows*r) + c] = (double)temp;
				}
			}
		}
	}
}




int main(int argc, char** argv) {
  if (argc < 5) {
    LOG(ERROR) << "test_net net_proto pretrained_net_proto iterations inputbin output_dir"
        << " [CPU/GPU]";
    return 0;
  }

  Caffe::set_phase(Caffe::TEST);

  if (argc == 7 && strcmp(argv[6], "GPU") == 0) {
    LOG(ERROR) << "Using GPU";
    Caffe::set_mode(Caffe::GPU);
  } else {
    LOG(ERROR) << "Using CPU";
    Caffe::set_mode(Caffe::CPU);
  }

  // load the .prototxt file
  NetParameter test_net_param;
  ReadProtoFromTextFile(argv[1], &test_net_param);
  Net<float> caffe_test_net(test_net_param);
  
  //load the weights
  NetParameter trained_net_param;
  ReadProtoFromBinaryFile(argv[2], &trained_net_param);
  
  //check if the caffemodel confirm with caffe.proto (check if there exists unknown fields)
  // change this part if you want to update the caffe.proto file
  cout << "start checking caffe model" << endl;
  const Reflection* ref = trained_net_param.GetReflection();
  int outer_unknown = ref->GetUnknownFields(trained_net_param).field_count();
  if (outer_unknown > 0) cout << "there exist unknown fields in the outer message"<<endl;
  else
  {
	  LayerConnection second_layer = trained_net_param.layers(1);
	  const Reflection* second_layer_ref = second_layer.GetReflection();
	  int inner_unknown = second_layer_ref->GetUnknownFields(second_layer).field_count();
	  if (inner_unknown > 0) cout << "there exist unknown fields in the inner message" << endl;
	  else
	  {
		  cout << "no unknown fields, the caffe model is the same format as caffe.proto" << endl;
	  }
  }

  // convert weights to test model
  caffe_test_net.CopyTrainedLayersFrom(trained_net_param);

#if 0
  SolverState state;
  std::string state_file = std::string(argv[2]) + ".solverstate";
  ReadProtoFromBinaryFile(state_file, &state);
#endif
  // the number of iterations
  int total_iter = atoi(argv[3]);
  LOG(ERROR) << "Running " << total_iter << " Iterations.";

  // in order to get the test accuracy, you still need input the label information
  double test_accuracy = 0;

  //save layer, the output directory
  char output_dir[1024];
  
  // the layer index of the output layer
  int output_layer_idx = -1;


  for(int i=0;i<caffe_test_net.layer_names().size();i++)
	  if(caffe_test_net.layer_names()[i] == "prob"){ // here the output layer is the probability layer
		  output_layer_idx = i;
		  break;
	  }

  CHECK_NE(output_layer_idx, -1);

  LOG(INFO) << "Output layer: " << output_layer_idx;
  



  // the output blob and data blob(input data)
  Blob<float>* output = caffe_test_net.top_vecs()[output_layer_idx][0],
	  *data_blob = caffe_test_net.input_blobs()[0];
    
  LOG(INFO) << "OUTPUT BLOB dim: " << output->num() << ' '
	  << output->channels() << ' '
	  << output->width() << ' '
	  << output->height();

  // read the image, store it in ar
  vector<vector<double>> ar;

  const int ih = data_blob->height(), iw = data_blob->width(), ic = data_blob->channels();
  size_t len = ih * iw * ic;
  CHECK_EQ(len, 784);

  cout << "the number of images that pass to the network: " << data_blob->num() << endl;
  // here load all the input image, there are 10000 images( >= the number of images that pass to the network) and the length of each image is 784
  cout << "start loading all the images" << endl;
  ReadMNIST(argv[4], 10000, 784, ar);
  cout << "finish loading all the images" << endl;

  for (int i = 0; i < total_iter; ++i) {
	  float *d = data_blob->mutable_cpu_data();
	  for(int j = 0; j < data_blob->num(); j++){
		  for(int k=0;k<len;k++){
			  d[k] = ar[j][k]/256;
		}
		  d += len;
	  }

    // forward the network
    const vector<Blob<float>*>& result =
        caffe_test_net.Forward(caffe_test_net.input_blobs());

	// save the result to the result directory, the number is the index of iteration
    sprintf(output_dir, "%s/result_%05d", argv[5], i);
    save_blob(output_dir, output);

  }

  cout << "output is saved in "<<argv[5] <<" directory"<< endl;
  return 0;
}
