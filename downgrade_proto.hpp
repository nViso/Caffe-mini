#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <string>
#include<iostream>
#include<stdlib.h>
#include<infstr.h>
#include <fstream>
#include "new_caffe/new_caffe.pb.h"



#include "caffe/caffe.hpp"

using namespace caffe;  // NOLINT(build/namespaces)
using namespace std;
using namespace google::protobuf; 

namespace downgrade_proto{
	// copy single blob from v2 blob to v0 blob
	bool CopySingleBlob(caffe::BlobProto* v0_blob, new_caffe::BlobProto v2_blob);


		// downgrade the net prarameter from v2 to v0
	bool DownGradeV2ToV0(caffe::NetParameter* v0_net, new_caffe::NetParameter v2_net);



}
