/*
 * Copyright (c) 2019 KhulnaSoft DevOps, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <deepfuzzy/DeepFuzzy.hpp>

using namespace deepfuzzy;


/* simple check performed only to see if certain chars match */
DEEPFUZZY_INLINE int check_pass(const char * pass, size_t len) {

  // length should be 11
  if (len != 11)
	return 1;

  // third char should be equal to `0`
  uint8_t ch0 = (uint8_t) pass[2];
  if (ch0 != 48)
    return 1;

  // ninth char should be equal to `o`
  uint8_t ch1 = (uint8_t) pass[8];
  if (ch1 != 111)
	return 1;

  return 0;
}


/* This test doesn't rely on any input methods from DeepFuzzy, so we prepend Boring*
   to signify that it is a concrete test that does not rely on any fuzzing/symex */
TEST(CharTest, BoringVerifyCheck) {
  const char *in_pass = "sh0uld_work";
  ASSERT(check_pass(in_pass, strlen(in_pass)) == 0)
	<< "password check failed, which SHOULDN'T happen here.";
}


/* This test clearly doesn't work, so we prepend `Disabled*` to the name such that
   during a run, it doesn't get called unless explicitly specified with `--run_disabled` */
TEST(CharTest, DisabledVerifyCheck) {
  const char *in_pass = "DOESNT_WORK_AT_ALL";
  ASSERT(check_pass(in_pass, strlen(in_pass)) == 0)
	<< "password check failed, as it SHOULD be failing.";
}


/* Regular test that executes during every run */
TEST(CharTest, VerifyCheck) {
  char *in_pass = DeepFuzzy_CStr_C(11, 0);
  ASSERT(check_pass(in_pass, 11) == 0)
	<< "password check failed.";
}
