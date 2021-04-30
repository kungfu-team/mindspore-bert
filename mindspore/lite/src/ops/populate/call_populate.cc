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
#include "src/ops/populate/populate_register.h"
using mindspore::schema::PrimitiveType_Call;

namespace mindspore {
namespace lite {
OpParameter *PopulateCallParameter(const void *prim) {
  OpParameter *call_parameter = reinterpret_cast<OpParameter *>(malloc(sizeof(OpParameter)));
  if (call_parameter == nullptr) {
    MS_LOG(ERROR) << "malloc CallParameter failed.";
    return nullptr;
  }
  memset(call_parameter, 0, sizeof(OpParameter));
  auto primitive = static_cast<const schema::Primitive *>(prim);
  call_parameter->type_ = primitive->value_type();
  return reinterpret_cast<OpParameter *>(call_parameter);
}
REG_POPULATE(PrimitiveType_Call, PopulateCallParameter, SCHEMA_CUR)
}  // namespace lite
}  // namespace mindspore