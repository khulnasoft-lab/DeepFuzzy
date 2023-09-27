# DeepFuzzy

[![Slack Chat](http://slack.empirehacking.nyc/badge.svg)](https://slack.empirehacking.nyc/)

DeepFuzzy is a framework that provides C and C++ developers with a common interface to various symbolic execution and fuzzing engines. Users can write one test harness using a Google Test-like API, then execute it using multiple backends without having to learn the complexities of the underlying engines. It supports writing unit tests and API sequence tests, as well as automatic test generation. Read more about the goals and design of DeepFuzzy in our [paper](https://agroce.github.io/bar18.pdf).

The
[2018 IEEE Cybersecurity Development Conference](https://secdev.ieee.org/2018/home)
included a
[full tutorial](https://github.com/khulnasoft-lab/publications/tree/master/workshops/DeepFuzzy:%20Bringing%20vulnerability%20detection%20tools%20into%20the%20development%20lifecycle%20-%20SecDev%202018)
on effective use of DeepFuzzy.


Table of Contents
=================
  * [DeepFuzzy in a Nutshell](#deepfuzzy-in-a-nutshell)
  * [Articles describing DeepFuzzy](#articles-describing-deepfuzzy)
  * [Overview of Features](#overview-of-features)
  * [Build'n'run](#buildnrun)
     * [Supported Platforms](#supported-platforms)
     * [Dependencies](#dependencies)
     * [Building on Ubuntu 18.04 (Bionic)](#building-on-ubuntu-1804-bionic)
     * [Building on Windows 10](#building-on-windows-10)
     * [Installing](#installing)
     * [Installation testing](#installation-testing)
     * [Docker](#docker)
  * [Documentation](#documentation)
  * [Contributing](#contributing)
  * [Trophy case](#trophy-case)
  * [License](#license)


## DeepFuzzy in a Nutshell

If you want to jump right in, or are having trouble with building DeepFuzzy, you can just use a Docker that is pre-built and has compiled versions of several of the easiest examples of DeepFuzzy in use:

```shell
docker pull agroce/deepfuzzy_examples_aflpp
docker run -it agroce/deepfuzzy_examples_aflpp
```

Then within the DeepFuzzy docker container, go to an example:
```shell
cd ~/examples/fuzz_tcas
deepfuzzy-afl ./TCAS_AFL -o fuzz_afl --fuzzer_out --timeout 120
./TCAS_cov --input_test_files_dir fuzz_afl/the_fuzzer/queue/
llvm-cov-9 gcov TCAS_driver.cpp  -b
```

This runs the AFL++ fuzzer on the TCAS code (https://en.wikipedia.org/wiki/Traffic_collision_avoidance_system), a long-used example program in software testing.  After two minutes of fuzzing, we run a version of the test driver that collects code coverage, and see how much of the code AFL has managed to cover in two minutes.

NOTE 1: The above docker is built using AFL++ instead of AFL for "AFL" fuzzing.  You can use agroce/deepfuzzy_examples instead if for some reason you prefer "classic" AFL.

NOTE 2: You may need to modify `/proc/sys/kernel/core_pattern` on your host for AFL to run properly, e.g.:

```shell
echo core | sudo tee /proc/sys/kernel/core_pattern
```

Finally, we can look at the failing tests AFL produces:

```shell
./TCAS --input_test_files_dir fuzz_afl/the_fuzzer/crashes/ --min_log_level=0
```

Inspecting `TCAS_driver.cpp` and `Makefile` will give a good idea of how DeepFuzzy can be used.  The other examples, including the one in the original DeepFuzzy blog post, are similar in structure and usage.

## Articles describing DeepFuzzy

* [Fuzzing an API with DeepFuzzy (Part 1)](https://blog.khulnasoft.com/2019/01/22/fuzzing-an-api-with-deepfuzzy-part-1)
* [Fuzzing an API with DeepFuzzy (Part 2)](https://blog.khulnasoft.com/2019/01/23/fuzzing-an-api-with-deepfuzzy-part-2)
* [Fuzzing Unit Tests with DeepFuzzy and Eclipser](https://blog.khulnasoft.com/2019/05/31/fuzzing-unit-tests-with-deepfuzzy-and-eclipser)
* [DeepFuzzy Now Supports Ensemble Fuzzing](https://blog.khulnasoft.com/2019/09/03/deepfuzzy-now-supports-ensemble-fuzzing/)
* [Everything You Ever Wanted To Know About Test-Case Reduction, But Didn’t Know to Ask](https://blog.khulnasoft.com/2019/11/11/test-case-reduction/)

## Overview of Features

* Tests look like Google Test, but can use symbolic execution/fuzzing to generate data (parameterized unit testing)
  * Easier to learn than binary analysis tools/fuzzers, but provides similar functionality
* Already supports Manticore, Angr, libFuzzer, file-based fuzzing with
  e.g., AFL or Eclipser; more back-ends likely in future
  * Switch test generation tool without re-writing test harness
    * Work around show-stopper bugs
    * Find out which tool works best for your code under test
    * Different tools find different bugs/vulnerabilities
    * Fair way to benchmark/bakeoff tools
* Provides test replay for regression [plus effective automatic test case reduction to aid debugging](https://blog.khulnasoft.com/2019/11/11/test-case-reduction/)
* Supports API-sequence generation with extensions to Google Test interface
  * Concise readable way (OneOf) to say "run one of these blocks of code"
  * Same construct supports fixed value set non-determinism
  * E.g., writing a POSIX file system tester is pleasant, not painful as in pure Google Test idioms
* Provides high-level strategies for improving symbolic execution/fuzzing effectiveness
  * Pumping (novel to DeepFuzzy) to pick concrete values when symbolic execution is too expensive
  * Automatic decomposition of integer compares to guide coverage-driven fuzzers
  * Strong support for automated [swarm testing](https://agroce.github.io/issta12.pdf)

To put it another way, DeepFuzzy sits at the intersection of
*property-based testing*, *traditional unit testing*, *fuzzing*, and
*symbolic execution*.  It lets you perform property-based unit testing
using fuzzing or symbolic execution as a back end to generate data, and saves the
results so that what DeepFuzzy finds can easily be used in
deterministic settings such as regression testing or CI.

## Build'n'run

### Supported Platforms

DeepFuzzy currently targets Linux, Windows, with macOS support in progress
(the fuzzers work fine, but symbolic execution is not well-supported
yet, without a painful cross-compilation process). No current support for ARM64 architecture,
 this includes Apple Silicon processors, as there is no support for multilib compilers. 

### Dependencies

Build:

- CMake
- GCC and G++ with multilib support
- Python 3.6 (or newer)
- Setuptools

Runtime:

- Python 3.6 (or newer)
- Z3 (for the Manticore backend)

### Building on Ubuntu 18.04 (Bionic)

First make sure you install [Python 3.6 or greater](https://askubuntu.com/a/865569). Then use this command line to install additional requirements and compile DeepFuzzy:

**Building Deepfuzzy**
```shell
sudo apt update && sudo apt-get install build-essential gcc-multilib g++-multilib cmake python3-setuptools python3-dev libffi-dev z3
sudo apt-add-repository ppa:sri-csl/formal-methods
sudo apt-get update
sudo apt-get install yices2
git clone https://github.com/khulnasoft-lab/deepfuzzy deepfuzzy
mkdir deepfuzzy/build && cd deepfuzzy/build
cmake ../
make
sudo make install
```

### Changing file permissions
```shell
cd deepfuzzy
sudo chmod -R 755 .
sudo chown -R username:groupname .
```

### Installing Dependencies on Ubuntu if issues arise
**CMake:**
```shell
sudo apt install cmake
```

**GCC & G++ with multilib support:**
```shell
sudo apt-get install gcc-multilib
sudo apt-get install g++-multilib
```

**Installing the latest version of Python via CLI:**
```shell
sudo apt-get update && sudo apt upgrade
sudo apt install wget build-essential libncursesw5-dev libssl-dev \
libsqlite3-dev tk-dev libgdbm-dev libc6-dev libbz2-dev libffi-dev zlib1g-dev
sudo add-apt-repository ppa:deadsnakes/ppa
sudo apt install python3.11
```

**Setup Tools:**

Skip the first command below if pip is already installed.
```shell
sudo apt install python3-pip
pip3 install setuptools
```

**Z3:**
```shell
sudo apt install z3
```

### Building on Windows 10
If you want to compile DeepFuzzy on Windows make sure to install MinGW with MSYS2 by following the [installation instructions](https://www.msys2.org/#installation). After the installation is finished, select an environment and launch that version of the environment from the Windows programs menu(if in doubt, choose MINGW64 or UCRT64). Then, use the command below to install all of your environment's dependencies and compile DeepFuzzy:
```shell
pacman -Syyu
pacman -S mingw-w64-x86_64-python3 mingw-w64-x86_64-python-setuptools mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-libffi mingw-w64-x86_64-make
pacman -S make git

git clone https://github.com/khulnasoft-lab/deepfuzzy deepfuzzy
mkdir deepfuzzy/build && cd deepfuzzy/build
cmake -G "Unix Makefiles" ../
make
make install
```

NOTE: If you decide to use UCRT64, keep in mind to change `x86_64` to `ucrt-x86_64` in the second pacman command, i.e. `mingw-w64-x86_64-python3` gets replaced with `mingw-w64-ucrt-x86_64-python3`.

### Installing

Assuming the DeepFuzzy build resides in `$DEEPFUZZY`, run the following commands to install the DeepFuzzy python package:

```shell
virtualenv venv
. venv/bin/activate
python $DEEPFUZZY/build/setup.py install
```

The `virtualenv`-enabled `$PATH` should now include two executables: `deepfuzzy` and `deepfuzzy-angr`. These are _executors_, which are used to run DeepFuzzy test binaries with specific backends (automatically installed as Python dependencies). The `deepfuzzy` or `deepfuzzy-figurative` executor uses the Manticore backend while `deepfuzzy-angr` uses angr. They share a common interface where you may specify a number of workers and an output directory for saving backend-generated test cases.

If you try using Manticore, and it doesn't work, but you definitely have the latest Manticore installed, check the `.travis.yml` file.  If that grabs a Manticore other than the master version, you can try using the version of Manticore we use in our CI tests.  Sometimes Manticore makes a breaking change, and we are behind for a short time.


### Installation testing

You can check your build using the test binaries that were (by default) built and emitted to `deepfuzzy/build/examples`. For example, to use angr to symbolically execute the `IntegerOverflow` test harness with 4 workers, saving generated test cases in a directory called `out`, you would invoke:

```shell
deepfuzzy-angr --num_workers 4 --output_test_dir out $DEEPFUZZY/build/examples/IntegerOverflow
```

 The resulting `out` directory should look something like:

 ```
 out
└── IntegerOverflow.cpp
    ├── SignedInteger_AdditionOverflow
    │   ├── a512f8ffb2c1bb775a9779ec60b699cb.fail
    │   └── f1d3ff8443297732862df21dc4e57262.pass
    └── SignedInteger_MultiplicationOverflow
        ├── 6a1a90442b4d898cb3fac2800fef5baf.fail
        └── f1d3ff8443297732862df21dc4e57262.pass
```

To run these tests, you can just use the native executable, e.g.:

```shell
$DEEPFUZZY/build/examples/IntegerOverflow --input_test_dir out
```

to run all the generated tests, or

```shell
$DEEPFUZZY/build/examples/IntegerOverflow --input_test_files_dir out/IntegerOverflow.cpp/SignedInteger_AdditionOverflow --input_which_test SignedInteger_AdditionOverflow
```

to run the tests in one directory (in this case, you want to specify
which test to run, also).  You can also run a single test, e.g.:

```shell
$DEEPFUZZY/build/examples/IntegerOverflow --input_test_file out/IntegerOverflow.cpp/SignedInteger_AdditionOverflow/a512f8ffb2c1bb775a9779ec60b699cb.fail--input_which_test SignedInteger_AdditionOverflow
```

In the absence of an `--input_which_test` argument, DeepFuzzy defaults
to the first-defined test.  Run the native executable with the `--help`
argument to see all DeepFuzzy options.


### Docker

You can also try out Deepfuzzy with Docker, which is the easiest way
to get all the fuzzers and tools up and running on any system.

The build may take about 40 minutes, because some fuzzers require us
building huge projects like QEMU or LLVM.

**Ensure that docker is installed:**

- Check out the docker website [here](https://docs.docker.com/engine/install/) for installation instructions.

**Check if docker is installed correctly:**
```bash
docker run hello-world
```

**Run these commands to install deepfuzzy via docker:**
```bash
git clone https://github.com/khulnasoft-lab/deepfuzzy deepfuzzy
$ cd deepfuzzy
$ docker build -t deepfuzzy-base -f docker/base/Dockerfile docker/base
$ docker build -t deepfuzzy --build-arg make_j=6 -f ./docker/Dockerfile .
$ docker run -it deepfuzzy bash
user@a17bc44fd259:~/deepfuzzy$ cd build/examples
user@a17bc44fd259:~/deepfuzzy/build/examples$ deepfuzzy-angr ./Runlen
user@a17bc44fd259:~/deepfuzzy/build/examples$ mkdir tmp && deepfuzzy-eclipser ./Runlen -o tmp --timeout 30
user@a17bc44fd259:~/deepfuzzy/build/examples$ cd ../../build_libfuzzer/examples
user@a17bc44fd259:~/deepfuzzy/build_libfuzzer/examples$ ./Runlen_LF -max_total_time=30
user@a17bc44fd259:~/deepfuzzy/build_libfuzzer/examples$ cd ../../build_afl/examples
user@a17bc44fd259:~/deepfuzzy/build_afl/examples$ mkdir foo && echo x > foo/x && mkdir afl_Runlen2
user@a17bc44fd259:~/deepfuzzy/build_afl/examples$ $AFL_HOME/afl-fuzz -i foo -o afl_Runlen -- ./Runlen_AFL --input_test_file @@ --no_fork --abort_on_fail
user@a17bc44fd259:~/deepfuzzy/build_afl/examples$ deepfuzzy-afl -o afl_Runlen2 ./Runlen_AFL --fuzzer_out
```

**How to use docker without sudo:**
```bash
sudo groupadd docker
sudo gpasswd -a $USER docker
newgrp docker
```

**If this error occurs:**
```bash
ERROR: Cannot connect to the Docker daemon at unix:///var/run/docker.sock. Is the docker daemon running?
```
**Run this command:**
```bash
systemctl start docker
```

## Documentation

Check out [docs](/docs) folder:

* [Basic usage](/docs/basic_usage.md)
* [Writing a test harness](/docs/test_harness.md)
* [Fuzzing](/docs/fuzzing.md)
* [Swarm testing](/docs/swarm_testing.md)

## External Tools

DeepFuzzy can be used to test R packages written using the popular Rcpp package.  The [Rcppdeepfuzzy tool](https://github.com/akhikolla/RcppDeepFuzzy) is described in a [paper presented at the 2021 IEEE International Symposium on Software Reliability Engineering](https://agroce.github.io/issre21.pdf).

## Contributing

All accepted PRs are awarded bounties by KhulnaSoft DevOps. Join the #deepfuzzy channel on the [Empire Hacking Slack](https://slack.empirehacking.nyc/) to discuss ongoing development and claim bounties. Check the [good first issue](https://github.com/khulnasoft-lab/deepfuzzy/issues?q=is%3Aissue+is%3Aopen+label%3A%22good+first+issue%22) label for suggested contributions.

## Trophy case

We have not yet applied DeepFuzzy to many targets, but it was responsible for finding the following confirmed bugs (serious faults are in bold):

- https://github.com/Blosc/c-blosc2/issues/93
- https://github.com/Blosc/c-blosc2/issues/94
- **https://github.com/Blosc/c-blosc2/issues/95** (bug causing compression engine to return incorrect uncompressed data) **FIXED**
- **https://github.com/FooBarWidget/boyer-moore-horspool/issues/4** (implementation of Turbo version of Boyer-Moore string search can fail to find present string)

## License

DeepFuzzy is released under [The Apache License 2.0](LICENSE).
