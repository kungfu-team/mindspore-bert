/**
 * Copyright 2019-2021 Huawei Technologies Co., Ltd
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
#include "nnacl/infer/mfcc_infer.h"

namespace mindspore {
namespace lite {
namespace {
OpParameter *PopulateMfccParameter(const void *prim) {
  MfccParameter *arg_param = reinterpret_cast<MfccParameter *>(malloc(sizeof(MfccParameter)));
  if (arg_param == nullptr) {
    MS_LOG(ERROR) << "malloc MfccParameter failed.";
    return nullptr;
  }
  memset(arg_param, 0, sizeof(MfccParameter));
  auto *primitive = static_cast<const schema::Primitive *>(prim);
  arg_param->op_parameter_.type_ = primitive->value_type();
  auto param = primitive->value_as_Mfcc();
  arg_param->dct_coeff_num_ = param->dct_coeff_num();
  return reinterpret_cast<OpParameter *>(arg_param);
}
}  // namespace

Registry g_mfccParameterRegistry(schema::PrimitiveType_Mfcc, PopulateMfccParameter, SCHEMA_CUR);
}  // namespace lite
}  // namespace mindspore
