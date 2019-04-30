mkdir -p build

g++ test_gnss_bce.cpp -std=c++11 -lboost_system -lboost_program_options -ltbb -I/localhome/ryan_watson/include  -L/localhome/ryan_watson/lib -lcluster -lgpstk -Wno-deprecated-declarations -lgtsam -I/localhome/ryan_watson/git/ion_gnss_2018/3rdparty/RobustGNSS/gtsam/gtsam/3rdparty/Eigen -fopenmp -o build/test_gnss_bce


g++ test_gnss_maxmix.cpp -std=c++11 -lboost_system -lboost_program_options -ltbb -I/localhome/ryan_watson/include  -L/localhome/ryan_watson/lib -lgpstk -Wno-deprecated-declarations -lgtsam -I/localhome/ryan_watson/git/ion_gnss_2018/3rdparty/RobustGNSS/gtsam/gtsam/3rdparty/Eigen -o build/test_gnss_maxmix


g++ test_gnss_dcs.cpp -std=c++11 -lboost_system -lboost_program_options -ltbb -I/localhome/ryan_watson/include  -L/localhome/ryan_watson/lib -lgpstk -Wno-deprecated-declarations -lgtsam -I/localhome/ryan_watson/git/ion_gnss_2018/3rdparty/RobustGNSS/gtsam/gtsam/3rdparty/Eigen -o build/test_gnss_dcs


g++ rnx_2_gtsam.cpp -std=c++11 -lboost_system  -lboost_program_options -I/localhome/ryan_watson/include -L/localhome/ryan_watson/lib -lgpstk -Wno-deprecated-declarations -o build/rnx_2_gtsam
