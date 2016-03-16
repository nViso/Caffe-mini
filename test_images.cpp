#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <string>
#include<iostream>
#include<stdlib.h>
#include<infstr.h>
#include <fstream>
#include "new_caffe/new_caffe.pb.h"
#include <google/protobuf/text_format.h>

#include "caffe/caffe.hpp"

#include "downgrade_proto.hpp"

using namespace caffe;  // NOLINT(build/namespaces)
using namespace std;
using namespace google::protobuf;
using namespace downgrade_proto;

template <typename Dtype>
static void save_blob(const string& fn, Blob<Dtype> *b) {
	LOG(INFO) << "Saving " << fn;
	FILE *f = fopen(fn.c_str(), "wb");
	CHECK(f != NULL);
	for (int i = 0; i < b->count(); i++)
	{
		fprintf(f, "%f ", b->cpu_data()[i]);
	}
	fclose(f);
}


int main(int argc, char** argv)
{
	if (argc < 5) {
		LOG(ERROR) << "test_net net_proto pretrained_net_proto iterations inputbin output_dir"
			<< " [CPU/GPU]";
		return 0;
	}

	Caffe::set_phase(Caffe::TEST);

	if (argc == 7 && strcmp(argv[6], "GPU") == 0) {
		LOG(ERROR) << "Using GPU";
		Caffe::set_mode(Caffe::GPU);
	}
	else {
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
	if (outer_unknown > 0) cout << "there exist unknown fields in the outer message" << endl;
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

	int total_iter = atoi(argv[3]);
	LOG(ERROR) << "Running " << total_iter << " Iterations.";

	//save layer, the output directory
	char output_dir_landmark[1024];
	char output_dir_Yaw[1024];
	char output_dir_Pitch[1024];
	char output_dir_Roll[1024];

	// the layer index of the output layer
	int output_layer_idx_landmark = -1;
	int output_layer_idx_Yaw = -1;
	int output_layer_idx_Pitch = -1;
	int output_layer_idx_Roll = -1;

	// find the index of output layer, there are 4 outputs, landmark, yaw, pitch and roll
	for (int i = 0; i < caffe_test_net.layer_names().size(); i++)
	{
		if (caffe_test_net.layer_names()[i] == "ip0_3") { // here the output layer is the inner product layer
			output_layer_idx_landmark = i;
		}

		if (caffe_test_net.layer_names()[i] == "ip1_3") { // here the output layer is the inner product layer
			output_layer_idx_Yaw = i;
		}

		if (caffe_test_net.layer_names()[i] == "ip2_3") { // here the output layer is the inner product layer
			output_layer_idx_Pitch = i;
		}

		if (caffe_test_net.layer_names()[i] == "ip3_3") { // here the output layer is the inner product layer
			output_layer_idx_Roll = i;
		}
	}


	CHECK_NE(output_layer_idx_landmark, -1);
	CHECK_NE(output_layer_idx_Yaw, -1);
	CHECK_NE(output_layer_idx_Pitch, -1);
	CHECK_NE(output_layer_idx_Roll, -1);

	LOG(INFO) << "Output layer: " << output_layer_idx_landmark;

	// the output blob and data blob(input data)
	Blob<float>* output_landmark = caffe_test_net.top_vecs()[output_layer_idx_landmark][0],
		*output_Yaw = caffe_test_net.top_vecs()[output_layer_idx_Yaw][0],
		*output_Pitch = caffe_test_net.top_vecs()[output_layer_idx_Pitch][0],
		*output_Roll = caffe_test_net.top_vecs()[output_layer_idx_Roll][0],
		*data_blob = caffe_test_net.input_blobs()[0];


	cout << "start loading the image" << endl;
	//load the input image

	Datum datum;
	ReadGrayImageToDatum(argv[4], 1, 40, 40, &datum);

	//get the blobproto
	BlobProto blob_proto;
	blob_proto.set_num(1);
	blob_proto.set_channels(datum.channels());
	blob_proto.set_height(datum.height());
	blob_proto.set_width(datum.width());
	const int data_size = datum.channels() * datum.height() * datum.width();
	int size_in_datum = std::max<int>(datum.data().size(),
		datum.float_data_size());
	for (int i = 0; i < size_in_datum; ++i) {
		blob_proto.add_data(0.);
	}

	const string& data = datum.data();
	// normalize the data by 255
	if (data.size() != 0) {
		for (int i = 0; i < size_in_datum; ++i) {
			blob_proto.set_data(i, blob_proto.data(i) + (uint8_t)data[i]/255.);
		}
	}

	//set data into blob
	data_blob->FromProto(blob_proto);

	cout << "success load the image" << endl;

	for (int i = 0; i < total_iter; ++i)
	{


		const vector<Blob<float>*>& result =
			caffe_test_net.Forward(caffe_test_net.input_blobs());

		// save the result to the result directory, the number is the index of iteration
	
		    sprintf(output_dir_landmark, "%s/%s_Landmarks", argv[5], argv[4]);

			sprintf(output_dir_Yaw, "%s/%s_Yaw", argv[5], argv[4]);
	
			sprintf(output_dir_Pitch, "%s/%s_Pitch", argv[5], argv[4]);
	
			sprintf(output_dir_Roll, "%s/%s_Roll", argv[5], argv[4]);
	

		save_blob(output_dir_landmark, output_landmark);
		save_blob(output_dir_Yaw, output_Yaw);
		save_blob(output_dir_Pitch, output_Pitch);
		save_blob(output_dir_Roll, output_Roll);
	}
	cout << "output is saved in " << argv[5] << " directory" << endl;
	return 0;
}