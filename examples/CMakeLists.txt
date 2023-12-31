# Copyright (c) 2019 KhulnaSoft DevOps, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

set(_not_for_fuzzing_files TakeOver Squares Klee)
file(GLOB files "*.cpp")

foreach(file ${files})
   get_filename_component(file_no_ext ${file} NAME_WE)  # should be NAME_WLE if newer cmake

   # skip some files that will be added manually later on
   list (FIND _not_for_fuzzing_files ${file_no_ext} _index)
   if (${_index} GREATER -1)
      continue()
   endif()

   add_executable(${file_no_ext} ${file})
   target_link_libraries(${file_no_ext} deepfuzzy)

   if (DEEPFUZZY_LIBFUZZER)
      add_executable("${file_no_ext}_LF" "${file}")
      target_link_libraries("${file_no_ext}_LF" deepfuzzy_LF)
      target_link_libraries("${file_no_ext}_LF" "-fsanitize=fuzzer,undefined")
      set_target_properties("${file_no_ext}_LF" PROPERTIES COMPILE_DEFINITIONS "LIBFUZZER")
   endif()

   if (DEEPFUZZY_AFL)
      add_executable("${file_no_ext}_AFL" "${file}")
      target_link_libraries("${file_no_ext}_AFL" deepfuzzy_AFL)
   endif()

   if (DEEPFUZZY_HONGGFUZZ)
      add_executable("${file_no_ext}_HFUZZ" "${file}")
      target_link_libraries("${file_no_ext}_HFUZZ" deepfuzzy_HFUZZ)
   endif()

   if (DEEPFUZZY_ANGORA)
      set(ANGORA_TAINT_RULE_LIST "./deepfuzzy_abilist.txt")

      if(DEFINED ENV{USE_TRACK})
         add_executable("${file_no_ext}_angora_taint" "${file}")
         target_link_libraries("${file_no_ext}_angora_taint" deepfuzzy_taint)
      else()
         add_executable("${file_no_ext}_angora_fast" "${file}")
         target_link_libraries("${file_no_ext}_angora_fast" deepfuzzy_fast)
      endif()
   endif()
endforeach()

if (NOT (DEEPFUZZY_LIBFUZZER OR DEEPFUZZY_AFL OR DEEPFUZZY_HONGGFUZZ OR DEEPFUZZY_ANGORA))
   if (NOT APPLE)
     add_executable(Squares Squares.c)
     target_link_libraries(Squares deepfuzzy)
     set_target_properties(Squares PROPERTIES COMPILE_DEFINITIONS "DEEPFUZZY_TEST")
   endif()

   add_executable(TakeOver TakeOver.cpp)
   target_link_libraries(TakeOver deepfuzzy)

   add_executable(Klee Klee.c)
   target_link_libraries(Klee deepfuzzy)
endif()