# Enabling Robust State Estimation through Measurement Error Covariance Adaptation

## How to Install


### 1) Requirements/Recommendations

#### Required
* Boost -->  ```` sudo apt-get install libboost-all-dev ````
* CMake -->  ```` sudo apt-get install cmake ````
* OpenMP --> ```` sudo apt install libomp-dev ````


### 2) Clone repository to local machine  
````bash

git clone https://github.com/wvu-navLab/Enabling-Robust-State-Estimation-through-Measurement-Error-Covariance-Adaptation.git

````

### 3) Build

````bash

cd Enabling-Robust-State-Estimation-through-Measurement-Error-Covariance-Adaptation
./build.sh

````

### 4) Test
./examples/build/test_gnss_bce -i /total/path/to/data/gtsam/drive_1_lq.gtsam --robustIter 100 --writeENU --writeECEF --dir test_1
