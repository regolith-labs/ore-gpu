

cmake:
```
sudo apt install cmake
```

LLVM:
```
wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
sudo ./llvm.sh 14
sudo apt install libllvm-14-ocaml-dev libllvm14 llvm-14 llvm-14-dev llvm-14-doc llvm-14-examples llvm-14-runtime
```

CUDA:
```
wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2004/x86_64/cuda-ubuntu2004.pin
sudo mv cuda-ubuntu2004.pin /etc/apt/preferences.d/cuda-repository-pin-600
sudo apt-key adv --fetch-keys https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2004/x86_64/3bf863cc.pub
sudo add-apt-repository "deb https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2004/x86_64/ /"
sudo apt-get update
sudo apt-get install cuda-12.2
```

Flex:
```
sudo apt-get install flex
```

Bison:
```
sudo apt-get install bison libbison-dev
```

zlib
```
sudo apt-get install zlib1g-dev libgtest-dev
```

Build:
```
cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON ..
```