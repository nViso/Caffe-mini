#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <string>
#include<iostream>
#include<stdlib.h>
#include<infstr.h>
#include <fstream>

// the header contains v2 version caffe model information
#include "new_caffe/new_caffe.pb.h"


// the header contains v0 version caffe model information
#include "caffe/caffe.hpp"

// the header contains downgrade information
#include "downgrade_proto.hpp"
using namespace caffe;  // NOLINT(build/namespaces)
using namespace std;
using namespace downgrade_proto;



// 4 input: v2_prototxt v2_caffemodel v0_output_prototxt v0_output_caffemodel
int main(int argc, char** argv)
{
	// load the caffe model file(the weights)
	new_caffe::NetParameter v2_text_net;
	caffe::NetParameter v0_text_net;

	new_caffe::NetParameter v2_binary_net;
	caffe::NetParameter v0_binary_net;

	// load the v2 prototxt and caffemodel
	ReadProtoFromTextFile(argv[1], &v2_text_net);
	ReadProtoFromBinaryFile(argv[2], &v2_binary_net);

	// downgrade the prototxt and caffemodel
	if (DownGradeV2ToV0(&v0_text_net, v2_text_net))
	{ 
		cout << "successfully downgrade the prototxt" << endl;

		// output the corresponding v0 prototxt 
		WriteProtoToTextFile(v0_text_net, argv[3]);
	}
	else {
		cout << "fail to downgrade the prototxt" << endl;
	}
	if (DownGradeV2ToV0(&v0_binary_net, v2_binary_net))
	{
		cout << "successfully downgrade the caffemodel" << endl;

		//output the corresponding v0 caffemodel
		WriteProtoToBinaryFile(v0_binary_net, argv[4]);
	}
	else {
		cout << "fail to downgrade the caffemodel" << endl;
	}


}