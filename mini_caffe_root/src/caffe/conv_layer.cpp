// Copyright 2013 Yangqing Jia

#include <vector>

#include "layer.hpp"
#include "vision_layers.hpp"
#include "im2col.hpp"
#include "filler.hpp"
#include "math_functions.hpp"

namespace caffe {

template <typename Dtype>
void ConvolutionLayer<Dtype>::SetUp(const vector<Blob<Dtype>*>& bottom,
      vector<Blob<Dtype>*>* top) {
  CHECK_EQ(bottom.size(), 1) << "Conv Layer takes a single blob as input.";
  CHECK_EQ(top->size(), 1) << "Conv Layer takes a single blob as output.";
  KSIZE_ = this->layer_param_.kernelsize();
  STRIDE_ = this->layer_param_.stride();
  GROUP_ = this->layer_param_.group();
  PAD_ = this->layer_param_.pad();
  NUM_ = bottom[0]->num();
  CHANNELS_ = bottom[0]->channels();
  HEIGHT_ = bottom[0]->height();
  WIDTH_ = bottom[0]->width();
  NTILE_WIDTH_ = this->layer_param_.ntile_width();
  NTILE_HEIGHT_ = this->layer_param_.ntile_height();
  NUM_OUTPUT_ = this->layer_param_.num_output();
  CHECK_GT(NUM_OUTPUT_, 0);
  CHECK_EQ(CHANNELS_ % GROUP_, 0);
  // The im2col result buffer would only hold one image at a time to avoid
  // overly large memory usage.
  int height_out = (HEIGHT_ + 2 * PAD_ - KSIZE_) / STRIDE_ + 1;
  int width_out = (WIDTH_ + 2 * PAD_ - KSIZE_) / STRIDE_ + 1;

  CHECK(height_out % NTILE_HEIGHT_ == 0);
  CHECK(width_out % NTILE_WIDTH_ == 0);
  TILE_WIDTH_ = width_out / NTILE_WIDTH_;
  TILE_HEIGHT_ = height_out / NTILE_HEIGHT_;

  col_buffer_.Reshape(1, CHANNELS_ * KSIZE_ * KSIZE_, TILE_HEIGHT_, TILE_WIDTH_);
  out_buffer_.Reshape(1, NUM_OUTPUT_, TILE_HEIGHT_, TILE_WIDTH_);
  // Set the parameters
  CHECK_EQ(NUM_OUTPUT_ % GROUP_, 0)
      << "Number of output should be multiples of group.";
  biasterm_ = this->layer_param_.biasterm();
  // Figure out the dimensions for individual gemms.
  M_ = NUM_OUTPUT_ / GROUP_;
  K_ = CHANNELS_ * KSIZE_ * KSIZE_ / GROUP_;
  N_ = TILE_WIDTH_ * TILE_HEIGHT_;
  (*top)[0]->Reshape(bottom[0]->num(), NUM_OUTPUT_, height_out, width_out);
  int ntiles = NTILE_WIDTH_ * NTILE_HEIGHT_;
  // Check if we need to set up the weights
  if (this->blobs_.size() > 0) {
    LOG(INFO) << "Skipping parameter initialization";
  } else {
    if (biasterm_) {
      this->blobs_.resize(2*ntiles);
    } else {
      this->blobs_.resize(1*ntiles);
    }
    // Intialize the weight
    for(int i = 0; i < ntiles; i++) {
	    this->blobs_[i].reset(
		new Blob<Dtype>(NUM_OUTPUT_, CHANNELS_ / GROUP_, KSIZE_, KSIZE_));
	    // fill the weights
	    shared_ptr<Filler<Dtype> > weight_filler(
		GetFiller<Dtype>(this->layer_param_.weight_filler()));
	    weight_filler->Fill(this->blobs_[i].get());
	    // If necessary, intiialize and fill the bias term
	    if (biasterm_) {
	      this->blobs_[ntiles+i].reset(new Blob<Dtype>(1, 1, 1, NUM_OUTPUT_));
	      shared_ptr<Filler<Dtype> > bias_filler(
		  GetFiller<Dtype>(this->layer_param_.bias_filler()));
	      bias_filler->Fill(this->blobs_[ntiles+i].get());
	    }
    }
  }
  // Set up the bias filler
  if (biasterm_) {
    bias_multiplier_.reset(new SyncedMemory(N_ * sizeof(Dtype)));
    Dtype* bias_multiplier_data =
        reinterpret_cast<Dtype*>(bias_multiplier_->mutable_cpu_data());
    for (int i = 0; i < N_; ++i) {
        bias_multiplier_data[i] = 1.;
    }
  }
}


template <typename Dtype>
void ConvolutionLayer<Dtype>::Forward_cpu(const vector<Blob<Dtype>*>& bottom,
      vector<Blob<Dtype>*>* top) {
  const Dtype* bottom_data = bottom[0]->cpu_data();
  Dtype* top_data = (*top)[0]->mutable_cpu_data();
  Dtype* col_data = col_buffer_.mutable_cpu_data();
  if(NTILE_WIDTH_ * NTILE_HEIGHT_ <= 1){
	  const Dtype* weight = this->blobs_[0]->cpu_data();
	  int weight_offset = M_ * K_;
	  int col_offset = K_ * N_;
	  int top_offset = M_ * N_;
	  for (int n = 0; n < NUM_; ++n) {
		  // First, im2col
		  im2col_cpu(bottom_data + bottom[0]->offset(n), CHANNELS_, HEIGHT_,
				  WIDTH_, KSIZE_, PAD_, STRIDE_, col_data);
		  // Second, innerproduct with groups
		  for (int g = 0; g < GROUP_; ++g) {
			  caffe_cpu_gemm<Dtype>(CblasNoTrans, CblasNoTrans, M_, N_, K_,
					  (Dtype)1., weight + weight_offset * g, col_data + col_offset * g,
					  (Dtype)0., top_data + (*top)[0]->offset(n) + top_offset * g);
		  }
		  // third, add bias
		  if (biasterm_) {
			  caffe_cpu_gemm<Dtype>(CblasNoTrans, CblasNoTrans, NUM_OUTPUT_,
					  N_, 1, (Dtype)1., this->blobs_[1]->cpu_data(),
					  reinterpret_cast<const Dtype*>(bias_multiplier_->cpu_data()),
					  (Dtype)1., top_data + (*top)[0]->offset(n));
		  }
	  }
  }else{
	  //NOT_IMPLEMENTED;
	  CHECK_EQ(STRIDE_, 1);
	  CHECK_EQ(PAD_, 0);
	  CHECK_EQ(GROUP_, 1);
	  CHECK_EQ(col_buffer_.height(), TILE_HEIGHT_);
	  Dtype *out_buffer = out_buffer_.mutable_cpu_data();
	  for (int n = 0; n < NUM_; ++n) {
		  for(int ny = 0; ny < NTILE_HEIGHT_; ny++){
			  for(int nx = 0; nx < NTILE_WIDTH_; nx++){
				  int idx = ny * NTILE_WIDTH_ + nx;
				  const Dtype* weight = this->blobs_[idx]->cpu_data();
				  const Dtype * img = bottom_data + bottom[0]->offset(n, 0,
						  TILE_HEIGHT_ * ny, TILE_WIDTH_ * nx);
				  im2col_tile_cpu(img,   CHANNELS_, HEIGHT_,
						  WIDTH_, KSIZE_, col_data,
						  TILE_HEIGHT_, TILE_WIDTH_);
				  //dump(&col_buffer_);
				  caffe_cpu_gemm<Dtype>(CblasNoTrans, CblasNoTrans, M_, N_, K_,
						  (Dtype)1., weight, col_data, (Dtype)0., out_buffer);
				  if (biasterm_) {
					  const Dtype *bias_ptr = this->blobs_[idx + NTILE_WIDTH_ *
						  NTILE_HEIGHT_]->cpu_data();
					  caffe_cpu_gemm<Dtype>(CblasNoTrans, CblasNoTrans, NUM_OUTPUT_,
							  N_, 1, (Dtype)1., bias_ptr,
							  reinterpret_cast<const Dtype*>(bias_multiplier_->cpu_data()),
							  (Dtype)1., out_buffer);
				  }
				  //dump(&out_buffer_);
				  /* copy back */

				  int height_out = HEIGHT_ - KSIZE_ + 1;
				  int width_out = WIDTH_ - KSIZE_ + 1;
				  copy_stride_cpu(out_buffer, NUM_OUTPUT_, TILE_HEIGHT_, TILE_WIDTH_,
						  top_data + (*top)[0]->offset(n, 0, TILE_HEIGHT_*ny,
							  TILE_WIDTH_*nx), height_out, width_out);

			  }
		  }
	  }/* n */

  }
}

template <typename Dtype>
Dtype ConvolutionLayer<Dtype>::Backward_cpu(const vector<Blob<Dtype>*>& top,
		const bool propagate_down, vector<Blob<Dtype>*>* bottom) {
	const Dtype* top_diff = top[0]->cpu_diff();
	const Dtype* weight = this->blobs_[0]->cpu_data();
	Dtype* weight_diff = this->blobs_[0]->mutable_cpu_diff();
	const Dtype* bottom_data = (*bottom)[0]->cpu_data();
	Dtype* bottom_diff = (*bottom)[0]->mutable_cpu_diff();
	Dtype* col_data = col_buffer_.mutable_cpu_data();
	Dtype* col_diff = col_buffer_.mutable_cpu_diff();
	// bias gradient if necessary
	Dtype* bias_diff = NULL;

	int ntiles = NTILE_WIDTH_ * NTILE_HEIGHT_;
	if(ntiles <= 1){
		if (biasterm_) {
			bias_diff = this->blobs_[1]->mutable_cpu_diff();
			memset(bias_diff, 0, sizeof(Dtype) * this->blobs_[1]->count());
			for (int n = 0; n < NUM_; ++n) {
				caffe_cpu_gemv<Dtype>(CblasNoTrans, NUM_OUTPUT_, N_,
						1., top_diff + top[0]->offset(n),
						reinterpret_cast<const Dtype*>(bias_multiplier_->cpu_data()), 1.,
						bias_diff);
			}
		}

		int weight_offset = M_ * K_;
		int col_offset = K_ * N_;
		int top_offset = M_ * N_;
		memset(weight_diff, 0, sizeof(Dtype) * this->blobs_[0]->count());
		for (int n = 0; n < NUM_; ++n) {
			// since we saved memory in the forward pass by not storing all col data,
			// we will need to recompute them.
			im2col_cpu(bottom_data + (*bottom)[0]->offset(n), CHANNELS_, HEIGHT_,
					WIDTH_, KSIZE_, PAD_, STRIDE_, col_data);
			// gradient w.r.t. weight. Note that we will accumulate diffs.
			for (int g = 0; g < GROUP_; ++g) {
				caffe_cpu_gemm<Dtype>(CblasNoTrans, CblasTrans, M_, K_, N_,
						(Dtype)1., top_diff + top[0]->offset(n) + top_offset * g,
						col_data + col_offset * g, (Dtype)1.,
						weight_diff + weight_offset * g);
			}
			// gradient w.r.t. bottom data, if necessary
			if (propagate_down) {
				for (int g = 0; g < GROUP_; ++g) {
					caffe_cpu_gemm<Dtype>(CblasTrans, CblasNoTrans, K_, N_, M_,
							(Dtype)1., weight + weight_offset * g,
							top_diff + top[0]->offset(n) + top_offset * g,
							(Dtype)0., col_diff + col_offset * g);
				}
				// col2im back to the data
				col2im_cpu(col_diff, CHANNELS_, HEIGHT_, WIDTH_, KSIZE_, PAD_, STRIDE_,
						bottom_diff + (*bottom)[0]->offset(n));
			}
		}
	}else{
		NOT_IMPLEMENTED;
	}
	return Dtype(0.);
}

INSTANTIATE_CLASS(ConvolutionLayer);

}  // namespace caffe
