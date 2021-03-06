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


namespace downgrade_proto {

	// copy the blob dim information from v2 blob to v0 blob
	bool CopyBlobDim(caffe::BlobProto* v0_blob, new_caffe::BlobProto v2_blob)
	{
		bool success = true;

		// add dim
		if (v2_blob.has_num())
		{
			v0_blob->set_num(v2_blob.num());
		}
		else
		{
			v0_blob->set_num(1);
		}

		if (v2_blob.has_channels())
		{
			v0_blob->set_channels(v2_blob.channels());
		}
		else
		{
			v0_blob->set_channels(1);
		}

		if (v2_blob.has_height())
		{
			v0_blob->set_height(v2_blob.height());
		}
		else
		{
			v0_blob->set_height(1);
		}

		if (v2_blob.has_width())
		{
			v0_blob->set_width(v2_blob.width());
		}
		else
		{
			v0_blob->set_width(1);
		}


		// add shape
		if (v2_blob.has_shape())
		{
			int shape_num = v2_blob.shape().dim_size();
			if (shape_num == 1)
			{
				v0_blob->set_width(v2_blob.shape().dim(0));
			}
			else if (shape_num == 2)
			{
				v0_blob->set_height(v2_blob.shape().dim(0));
				v0_blob->set_width(v2_blob.shape().dim(1));
			}
			else if (shape_num == 3)
			{
				v0_blob->set_channels(v2_blob.shape().dim(0));
				v0_blob->set_height(v2_blob.shape().dim(1));
				v0_blob->set_width(v2_blob.shape().dim(2));
			}
			else if (shape_num == 4)
			{
				v0_blob->set_num(v2_blob.shape().dim(0));
				v0_blob->set_channels(v2_blob.shape().dim(1));
				v0_blob->set_height(v2_blob.shape().dim(2));
				v0_blob->set_width(v2_blob.shape().dim(3));
			}

		}

		if (!(v0_blob->has_num() && v0_blob->has_channels() && v0_blob->has_height() && v0_blob->has_width())) success = false;
		return success;
	}


	// copy the input dim information in NetParameter from v2 to v0
	bool CopyNetInputDim(caffe::NetParameter* v0_net, new_caffe::NetParameter v2_net)
	{
		bool success = true;

		// add dim
		// if the dim information are stored in input_dim     	
		for (int i = 0; i < v2_net.input_dim_size(); ++i)
		{
			v0_net->add_input_dim(v2_net.input_dim(i));
		}


		//  if the dim information are stored in input_shape
		if (v0_net->input_dim_size() == 0)
		{
			// for each input shape, we add 4 numbers
			for (int i = 0; i < v2_net.input_shape_size(); ++i)
			{
				new_caffe::BlobShape blob_shape = v2_net.input_shape(i);
				if (blob_shape.dim_size() > 4 || blob_shape.dim_size() < 1)
				{
					success = false;
				}
				else
				{
					for (int j = 0; j < 4 - blob_shape.dim_size(); ++j)
					{
						v0_net->add_input_dim(1);
					}

					for (int j = 0; j < blob_shape.dim_size(); ++j)
					{
						v0_net->add_input_dim(blob_shape.dim(j));
					}
				}

			}
		}


		return success;


	}

	// copy single blob from v2 blob to v0 blob
	bool CopySingleBlob(caffe::BlobProto* v0_blob, new_caffe::BlobProto v2_blob)
	{
		bool success = true;

		// add data
		for (int k = 0; k < v2_blob.data_size(); ++k)
		{
			v0_blob->add_data(v2_blob.data(k));
		}


		// add diff
		for (int k = 0; k < v2_blob.diff_size(); ++k)
		{
			v0_blob->add_diff(v2_blob.diff(k));
		}

		// add dim
		success &= CopyBlobDim(v0_blob, v2_blob);
		return success;

	}


	// copy the blobs from v2 layer to v0 layer
	bool CopyBlobs(caffe::LayerParameter* v0_layer, new_caffe::LayerParameter v2_layer)
	{
		bool success = true;
		// add blobs
		for (int j = 0; j < v2_layer.blobs_size(); ++j)
		{
			new_caffe::BlobProto v2_blob = v2_layer.blobs(j);
			BlobProto* v0_blob = v0_layer->add_blobs();
			//Copy each blob
			success &= CopySingleBlob(v0_blob, v2_blob);
		}

		return success;
	}

	//copy the weight filler in convolutional layer from v2 layer to v0 layer
	void CopyConvWeightFiller(caffe::LayerParameter* v0_layer, new_caffe::LayerParameter v2_layer)
	{
		if (v2_layer.convolution_param().weight_filler().has_type())
			v0_layer->mutable_weight_filler()->set_type(v2_layer.convolution_param().weight_filler().type());

		if (v2_layer.convolution_param().weight_filler().has_value())
			v0_layer->mutable_weight_filler()->set_value(v2_layer.convolution_param().weight_filler().value());

		if (v2_layer.convolution_param().weight_filler().has_min())
			v0_layer->mutable_weight_filler()->set_min(v2_layer.convolution_param().weight_filler().min());

		if (v2_layer.convolution_param().weight_filler().has_max())
			v0_layer->mutable_weight_filler()->set_max(v2_layer.convolution_param().weight_filler().max());

		if (v2_layer.convolution_param().weight_filler().has_mean())
			v0_layer->mutable_weight_filler()->set_mean(v2_layer.convolution_param().weight_filler().mean());

		if (v2_layer.convolution_param().weight_filler().has_std())
			v0_layer->mutable_weight_filler()->set_std(v2_layer.convolution_param().weight_filler().std());

	}

	//copy the bias filler in convolutional layer from v2 layer to v0 layer
	void CopyConvBiasFiller(caffe::LayerParameter* v0_layer, new_caffe::LayerParameter v2_layer)
	{
		if (v2_layer.convolution_param().bias_filler().has_type())
			v0_layer->mutable_bias_filler()->set_type(v2_layer.convolution_param().bias_filler().type());

		if (v2_layer.convolution_param().bias_filler().has_value())
			v0_layer->mutable_bias_filler()->set_value(v2_layer.convolution_param().bias_filler().value());

		if (v2_layer.convolution_param().bias_filler().has_min())
			v0_layer->mutable_bias_filler()->set_min(v2_layer.convolution_param().bias_filler().min());

		if (v2_layer.convolution_param().bias_filler().has_max())
			v0_layer->mutable_bias_filler()->set_max(v2_layer.convolution_param().bias_filler().max());

		if (v2_layer.convolution_param().bias_filler().has_mean())
			v0_layer->mutable_bias_filler()->set_mean(v2_layer.convolution_param().bias_filler().mean());

		if (v2_layer.convolution_param().bias_filler().has_std())
			v0_layer->mutable_bias_filler()->set_std(v2_layer.convolution_param().bias_filler().std());

	}


	// copy the layer parameter from v2 to v0 layer, notice that v0 caffe doesn't support share_mode and parameter blob name
	void CopyParam(caffe::LayerParameter* v0_layer, new_caffe::LayerParameter v2_layer)
	{
		for (int j = 0; j < v2_layer.param_size(); ++j)
		{
			if (v2_layer.param(j).has_lr_mult())
			{
				float mult_para = v2_layer.param(j).lr_mult();
				v0_layer->add_blobs_lr(mult_para);
			}

			if (v2_layer.param(j).has_decay_mult())
			{
				float decay_para = v2_layer.param(j).decay_mult();
				v0_layer->add_weight_decay(decay_para);
			}

		}

	}

	// copy the inner product layer weight filler, from v2 to v0
	void CopyIPWeightFiller(caffe::LayerParameter* v0_layer, new_caffe::LayerParameter v2_layer)
	{
		if (v2_layer.inner_product_param().weight_filler().has_type())
			v0_layer->mutable_weight_filler()->set_type(v2_layer.inner_product_param().weight_filler().type());

		if (v2_layer.inner_product_param().weight_filler().has_value())
			v0_layer->mutable_weight_filler()->set_value(v2_layer.inner_product_param().weight_filler().value());

		if (v2_layer.inner_product_param().weight_filler().has_min())
			v0_layer->mutable_weight_filler()->set_min(v2_layer.inner_product_param().weight_filler().min());

		if (v2_layer.inner_product_param().weight_filler().has_max())
			v0_layer->mutable_weight_filler()->set_max(v2_layer.inner_product_param().weight_filler().max());

		if (v2_layer.inner_product_param().weight_filler().has_mean())
			v0_layer->mutable_weight_filler()->set_mean(v2_layer.inner_product_param().weight_filler().mean());

		if (v2_layer.inner_product_param().weight_filler().has_std())
			v0_layer->mutable_weight_filler()->set_std(v2_layer.inner_product_param().weight_filler().std());

	}


	// copy the inner product layer bias filler, from v2 to v0
	void CopyIPBiasFiller(caffe::LayerParameter* v0_layer, new_caffe::LayerParameter v2_layer)
	{
		if (v2_layer.inner_product_param().bias_filler().has_type())
			v0_layer->mutable_bias_filler()->set_type(v2_layer.inner_product_param().bias_filler().type());

		if (v2_layer.inner_product_param().bias_filler().has_value())
			v0_layer->mutable_bias_filler()->set_value(v2_layer.inner_product_param().bias_filler().value());

		if (v2_layer.inner_product_param().bias_filler().has_min())
			v0_layer->mutable_bias_filler()->set_min(v2_layer.inner_product_param().bias_filler().min());

		if (v2_layer.inner_product_param().bias_filler().has_max())
			v0_layer->mutable_bias_filler()->set_max(v2_layer.inner_product_param().bias_filler().max());

		if (v2_layer.inner_product_param().bias_filler().has_mean())
			v0_layer->mutable_bias_filler()->set_mean(v2_layer.inner_product_param().bias_filler().mean());

		if (v2_layer.inner_product_param().bias_filler().has_std())
			v0_layer->mutable_bias_filler()->set_std(v2_layer.inner_product_param().bias_filler().std());

	}

	// set the data layer
	void SetDataLayer(caffe::LayerParameter* v0_layer, new_caffe::LayerParameter layer)
	{
		v0_layer->set_type("data");
		v0_layer->set_source(layer.data_param().source());
		v0_layer->set_batchsize(layer.data_param().batch_size());
		v0_layer->set_scale(layer.transform_param().scale());
	}

	// set the convolutional layer
	bool SetConvLayer(caffe::LayerParameter* v0_layer, new_caffe::LayerParameter layer)
	{
		bool success = true;
		v0_layer->set_type("conv");

		// copy the blob information from v2 net to v0 net if it exists
		success &= CopyBlobs(v0_layer, layer);

		v0_layer->set_num_output(layer.convolution_param().num_output());

		// check the number of kernel
		if (layer.convolution_param().kernel_size_size() > 1)
		{
			cout << " the convolutional layer has more than 1 kernel while v0 model only support single kernel" << endl;
			success = false;
		}

		// check the number of stride
		if (layer.convolution_param().stride_size() > 1)
		{
			cout << " the convolutional layer has more than 1 stride while v0 model only support single stride" << endl;
			success = false;
		}

		v0_layer->set_kernelsize(layer.convolution_param().kernel_size(0));
		v0_layer->set_stride(layer.convolution_param().stride(0));
		v0_layer->set_pad(layer.convolution_param().pad(0));

		// add the weight filler if it exist
		CopyConvWeightFiller(v0_layer, layer);

		// add the bias filler if it exists
		CopyConvBiasFiller(v0_layer, layer);

		// add param if it exists
		CopyParam(v0_layer, layer);

		return success;
	}

	// set the pooling layer
	bool SetPoolLayer(caffe::LayerParameter* v0_layer, new_caffe::LayerParameter layer)
	{
		bool success = true;
		v0_layer->set_type("pool");

		//set kernel and stride
		v0_layer->set_kernelsize(layer.pooling_param().kernel_size());
		v0_layer->set_stride(layer.pooling_param().stride());

		//  set pooling param
		new_caffe::PoolingParameter_PoolMethod new_pool = layer.pooling_param().pool();
		if (new_pool == new_caffe::PoolingParameter_PoolMethod::PoolingParameter_PoolMethod_MAX)
			v0_layer->set_pool(caffe::LayerParameter_PoolMethod_MAX);
		else if (new_pool == new_caffe::PoolingParameter_PoolMethod::PoolingParameter_PoolMethod_AVE)
			v0_layer->set_pool(caffe::LayerParameter_PoolMethod_AVE);
		else if (new_pool == new_caffe::PoolingParameter_PoolMethod::PoolingParameter_PoolMethod_STOCHASTIC)
			v0_layer->set_pool(caffe::LayerParameter_PoolMethod_STOCHASTIC);
		else {
			cout << "mismatch PoolingParameter_PoolMethod" << endl;
			success = false;
		}

		return success;
	}

	// set the inner product layer
	bool SetIPLayer(caffe::LayerParameter* v0_layer, new_caffe::LayerParameter layer)
	{
		bool success = true;
		v0_layer->set_type("innerproduct");

		// set the params
		CopyParam(v0_layer, layer);

		// add the weight filler and bias filler
		CopyIPWeightFiller(v0_layer, layer);
		CopyIPBiasFiller(v0_layer, layer);

		//set the number of output
		v0_layer->set_num_output(layer.inner_product_param().num_output());

		// add blobs if they exist
		success &= CopyBlobs(v0_layer, layer);
		return success;

	}

	// set the dropout layer
	bool SetDropoutLayer(caffe::LayerParameter* v0_layer, new_caffe::LayerParameter layer)
	{
		bool success = true;
		v0_layer->set_type("dropout");
		// add dropout ratio
		if (layer.has_dropout_param())
			v0_layer->set_dropout_ratio(layer.dropout_param().dropout_ratio());

		// add blobs if they exist
		success &= CopyBlobs(v0_layer, layer);
		return success;

	}


	// downgrade the net prarameter from v2 to v0
	bool DownGradeV2ToV0(caffe::NetParameter* v0_net, new_caffe::NetParameter v2_net)
	{
		bool success = true;
		v0_net->Clear();
		v0_net->set_name(v2_net.name());
		// set the force backward if it exists
		if (v2_net.has_force_backward())
			v0_net->set_force_backward(v2_net.force_backward());

		// copy input shape, add the dim information
		for (int i = 0; i < v2_net.input_shape_size(); ++i)
		{
			new_caffe::BlobShape blob_shape = v2_net.input_shape(i);
			for (int j = 0; j < blob_shape.dim_size(); ++j)
			{
				int32 dim = blob_shape.dim(j);
				v0_net->add_input_dim(dim);
			}
		}

		// copy input dim if it exists in v2 network
		for (int i = 0; i < v2_net.input_dim_size(); ++i)
		{
			v0_net->add_input_dim(v2_net.input_dim(i));
		}



		// add the input information if it exists
		for (int i = 0; i < v2_net.input_size(); ++i)
		{
			v0_net->add_input(v2_net.input(i));
		}


		// add the layer information
		for (int i = 0; i < v2_net.layer_size(); ++i)
		{
			// choose the ith layer of v2 model
			new_caffe::LayerParameter layer = v2_net.layer(i);

			// add the layerconnection
			LayerConnection* layer_conn = v0_net->add_layers();

			// add bottom and top
			layer_conn->mutable_bottom()->CopyFrom(layer.bottom());
			layer_conn->mutable_top()->CopyFrom(layer.top());

			// set the name of this layer
			LayerParameter v0_layer;
			v0_layer.set_name(layer.name());

			// for the data layer
			if (layer.type().compare("Data") == 0)
			{
				SetDataLayer(&v0_layer, layer);
			}

			// for the convolution layer
			else if (layer.type().compare("Convolution") == 0)
			{
				success &= SetConvLayer(&v0_layer, layer);

			}
			// for the pooling layer
			else if (layer.type().compare("Pooling") == 0)
			{
				success &= SetPoolLayer(&v0_layer, layer);
			}

			// for the innerproduct layer
			else if (layer.type().compare("InnerProduct") == 0)
			{
				success &= SetIPLayer(&v0_layer, layer);

			}
			else if (layer.type().compare("ReLU") == 0)
			{
				v0_layer.set_type("relu");
			}
			else if (layer.type().compare("Softmax") == 0)
			{
				v0_layer.set_type("softmax");
			}

			else if (layer.type().compare("SoftmaxWithLoss") == 0)
			{
				v0_layer.set_type("softmax_loss");
			}
			else if (layer.type().compare("Dropout") == 0)
			{
				success &= SetDropoutLayer(&v0_layer, layer);
			}
			else
			{
				// for other layers, we just copy the type name, this could cause problem with layers containing other parameters
				v0_layer.set_type(layer.type());
			}

			layer_conn->mutable_layer()->CopyFrom(v0_layer);

		}

		return success;
	}
}
