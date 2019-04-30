/*
   Simple test script to eval. the accuracy of the var. inf. clustering algos.

   Author: Ryan
 */
#include <libcluster/libcluster.h>
#include <libcluster/distributions.h>
#include <libcluster/probutils.h>

// #include <omp.h>

// BOOST
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/serialization/export.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

// STD
#include <string>
#include <random>
#include <chrono>
#include <random>
#include <iomanip>
#include <fstream>
#include <unistd.h>
#include <iostream>
#include <algorithm>

// Eigen
#include <Eigen/Core>
#include <Eigen/SVD>

using namespace std;
using namespace Eigen;
using namespace boost;
using namespace libcluster;
using namespace distributions;

namespace po = boost::program_options;

#define MAXBUFSIZE  ((int) 1e6)
Eigen::MatrixXd Tw_inv;

// txt file to eigen matrix
MatrixXd readMatrix(const char *filename)
{
        int cols = 0, rows = 0;
        double buff[MAXBUFSIZE];

        // Read numbers from file into buffer.
        ifstream infile;
        infile.open(filename);
        while (!infile.eof())
        {
                string line;
                getline(infile, line);

                int temp_cols = 0;
                stringstream stream(line);
                while(!stream.eof())
                        stream >> buff[cols*rows+temp_cols++];

                if (temp_cols == 0)
                        continue;

                if (cols == 0)
                        cols = temp_cols;

                rows++;
        }

        infile.close();

        rows--;

        // Populate matrix with numbers.
        MatrixXd result(rows,cols);
        for (int i = 0; i < rows; i++)
                for (int j = 0; j < cols; j++)
                        result(i,j) = buff[ cols*i+j ];

        return result;
};


// Whiten data using ZCA
MatrixXd whitenData(MatrixXd& data)
{
        auto means = data.colwise().mean();
        auto data_zm = data.rowwise() - means;

        auto cov_zm = probutils::cov(data_zm);

        JacobiSVD<MatrixXd> svd( cov_zm, ComputeFullU );
        const Eigen::MatrixXd U = svd.matrixU();
        const Eigen::ArrayXd S = svd.singularValues();
        const Eigen::VectorXd S_sqrt = S.inverse().sqrt();

        // ZCA (Zero-phase whitening) transformation
        auto Tw = U * S_sqrt.asDiagonal() * U.transpose();

        Eigen::MatrixXd tmp = Tw.inverse();
        Tw_inv = tmp;
        cout << data * Tw << endl;
        return data * Tw;
};


int main(int argc, char** argv)
{

        bool help;
        string data_file;

        po::options_description desc("Available options");
        desc.add_options()
                ("help,h", "Print help message")
                ("data_file,d", po::value<string>(&data_file)->default_value(""),
                "Name of data file")
        ;

        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
        po::notify(vm);

        help = (vm.count("help")>0);
        if (help)
                exit(1);

        // read data
        MatrixXd data = readMatrix(data_file.c_str());

        // Set up the inputs for the BGMM
        StickBreak weights;
        // Dirichlet weights;
        vector<GaussWish> clusters;
        MatrixXd qZ;

        // perform clustering
        // REF:: 'Accelerated variational Dirichlet process mixtures'.
        auto start = std::chrono::steady_clock::now();
        MatrixXd whitened_data = whitenData(data);
        learnVDP(whitened_data, qZ, weights, clusters);
        // learnVDP(data, qZ, weights, clusters);
        auto end = std::chrono::steady_clock::now();
        // extract run time in µs
        auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();

        cout << Tw_inv << endl;

        cout << endl << "Elapsed Time = " << elapsedTime / 10e6 << endl;
        cout << endl << "Cluster Weights:" << endl;
        cout << weights.Elogweight().exp().transpose() << endl;

        cout << endl << "Cluster means:" << endl;
        for (vector<GaussWish>::iterator k=clusters.begin(); k < clusters.end(); ++k)
                cout << k->getmean() * Tw_inv << endl;

        cout << endl << "Cluster covariances:" << endl;
        for (vector<GaussWish>::iterator k=clusters.begin(); k < clusters.end(); ++k)
                cout << k->getcov() * Tw_inv << endl << endl;

        return 0;
}
