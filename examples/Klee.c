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

#include <deepfuzzy/Klee.h>

DEEPFUZZY_NOINLINE int get_sign(int x) {
  if (x == 0) {
    printf("zero\n");
    return 0;
  }

  if (x < 0) {
    printf("negative\n");
    return -1;
  } else {
    printf("positive\n");
    return 1;
  }
}

int main(int argc, char *argv[]) {
  int a;
  klee_make_symbolic(&a, sizeof(a), "a");

  return get_sign(a);
}
