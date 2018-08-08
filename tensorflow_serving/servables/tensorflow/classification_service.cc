/* Copyright 2017 Google Inc. All Rights Reserved.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "tensorflow_serving/servables/tensorflow/classification_service.h"
#include <sys/time.h>
#include <thread>

#include <memory>

#include "tensorflow/contrib/session_bundle/session_bundle.h"
#include "tensorflow/contrib/session_bundle/signature.h"
#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/core/platform/tracing.h"
#include "tensorflow_serving/apis/classifier.h"
#include "tensorflow_serving/core/servable_handle.h"
#include "tensorflow_serving/servables/tensorflow/classifier.h"
#include "tensorflow_serving/servables/tensorflow/util.h"

long getCurrentTime2()  
{  
   struct timeval tv;  
   gettimeofday(&tv,NULL);  
   return tv.tv_sec * 1000 + tv.tv_usec / 1000;  
} 

namespace tensorflow {
namespace serving {

Status TensorflowClassificationServiceImpl::Classify(
    const RunOptions& run_options, ServerCore* core,
    const ClassificationRequest& request, ClassificationResponse* response) {

  long tmpcal_ptr = getCurrentTime2();
  TRACELITERAL("TensorflowClassificationServiceImpl::Classify");
  // Verify Request Metadata and create a ServableRequest
  if (!request.has_model_spec()) {
    return tensorflow::Status(tensorflow::error::INVALID_ARGUMENT,
                              "Missing ModelSpec");
  }

  ServableHandle<SavedModelBundle> saved_model_bundle;
  TF_RETURN_IF_ERROR(
      core->GetServableHandle(request.model_spec(), &saved_model_bundle));
  

  Status s =  RunClassify(run_options, saved_model_bundle->meta_graph_def,
                     saved_model_bundle.id().version,
                     saved_model_bundle->session.get(), request, response);
    long timeDiff = getCurrentTime2() - tmpcal_ptr;
    LOG(INFO) << "INTO classifier_interface time cost: "<<timeDiff<<" "<<std::this_thread::get_id();
    return s;
}

}  // namespace serving
}  // namespace tensorflow