/**
 * Copyright 2021 Huawei Technologies Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef MICRO_CODER_SESSION_CODER_H_
#define MICRO_CODER_SESSION_CODER_H_

#include <map>
#include <string>
#include <vector>
#include <memory>
#include "schema/inner/model_generated.h"
#include "coder/coder_graph.h"
#include "coder/coder_context.h"
#include "coder/coder_config.h"
#include "coder/allocator/allocator.h"
#include "coder/opcoders/op_coder.h"

namespace mindspore::lite::micro {
class CoderSession {
 public:
  CoderSession();

  ~CoderSession();

  int Init(const std::string &model_path);

  int Build();

  int Run();

  int GenerateCode();

 private:
  int InitNodesInputsAndOutputs();
  int InitTensorsRef();
  int ConvertTensors();
  int CreateOpCoders();
  int InitGraphInOutTensors();
  int CompileGraph();
  int InferShape();
  void EndCode();

  std::unique_ptr<CoderGraph> coder_graph_{nullptr};
  std::unique_ptr<CoderContext> coder_context_{nullptr};
  MemoryAllocator *allocator_{nullptr};
  std::vector<std::unique_ptr<OperatorCoder>> op_coders_;
};

std::shared_ptr<CoderSession> CreateCoderSession();

}  // namespace mindspore::lite::micro
#endif  // MICRO_CODER_SESSION_CODER_H_