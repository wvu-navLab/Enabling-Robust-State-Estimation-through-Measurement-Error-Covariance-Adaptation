/**
 *  @file   clusterExample.cpp
 *  @author Ryan Watson & Jason Gross
 *  @brief  Factor graph for batch covariance estimation to enable robust GPS processing. This example processes both pseudorange and carrier-phase observations. For more inforomation on how carrier-phase is incorporated, please see [3].

 * REF::
 * [1] "Batch Measurement Error Covariance Estimation for Robust Localization", Watson, R., Taylor, C., Leishman, R., and Gross, J., ION GNSS+ 2018.

 * [2] Steinberg, Daniel. "An unsupervised approach to modelling visual data." (2013).

 * [3] Watson, Ryan M., and Jason N. Gross. "Evaluation of kinematic precise point positioning convergence with an incremental graph optimizer." Position, Location and Navigation Symposium (PLANS), 2018 IEEE/ION. IEEE, 2018.

 * how to run ::
 * ./clusterExample -h
 * ./clusterExample -i data.gtsam --writeENU --robustIter 1 --percentFaulty 0.3 --dir test

 **/
// GTSAM
#include <gtsam/base/Vector.h>
#include <gtsam/base/Matrix.h>
#include <gtsam/inference/Key.h>
#include <gtsam/geometry/Point3.h>
#include <gtsam/base/FastVector.h>
#include <gtsam/inference/Symbol.h>
#include <gtsam/nonlinear/Values.h>
#include <gtsam/slam/PriorFactor.h>
#include <gtsam/slam/BetweenFactor.h>
#include <gtsam/nonlinear/Marginals.h>
#include <gtsam/inference/FactorGraph.h>
#include <gtsam/gnssNavigation/GnssData.h>
#include <gtsam/gnssNavigation/GnssTools.h>
#include <gtsam/nonlinear/DoglegOptimizer.h>
#include <gtsam/nonlinear/NonlinearFactor.h>
#include <gtsam/gnssNavigation/FolderUtils.h>
#include <gtsam/gnssNavigation/GnssPostfit.h>
#include <gtsam/gnssNavigation/PhaseFactor.h>
#include <gtsam/gnssNavigation/gnssStateVec.h>
#include <gtsam/robustModels/PseudorangeMaxMix.h>
#include <gtsam/nonlinear/NonlinearFactorGraph.h>
#include <gtsam/nonlinear/GaussNewtonOptimizer.h>
#include <gtsam/robustModels/PseudorangeMaxMix.h>
#include <gtsam/gnssNavigation/PseudorangeFactor.h>
#include <gtsam/nonlinear/NonlinearOptimizer.h>
#include <gtsam/nonlinear/LevenbergMarquardtOptimizer.h>

// LibCluster
#include <libcluster/probutils.h>
#include <libcluster/libcluster.h>
#include <libcluster/distributions.h>

// BOOST
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/serialization/export.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

// STANDARD
#include <fstream>
#include <iostream>
#include <tuple>
#include <ios>

// Intel Threading Building Block
#ifdef GTSAM_USE_TBB
  #include <tbb/tbb.h>
  #undef max // TBB seems to include windows.h and we don't want these macros
  #undef min
#endif

using namespace std;
using namespace gtsam;
using namespace boost;
using namespace libcluster;
using namespace distributions;

namespace po = boost::program_options;

typedef noiseModel::Diagonal diagNoise;
namespace NM = gtsam::noiseModel;

Eigen::MatrixXd Tw_inv;
// Whiten data using ZCA
Eigen::MatrixXd whitenData(Eigen::MatrixXd& data)
{
        auto means = data.colwise().mean();
        auto data_zm = data.rowwise() - means;

        auto cov_zm = probutils::cov(data_zm);

        Eigen::JacobiSVD<Eigen::MatrixXd> svd( cov_zm, Eigen::ComputeFullU );
        const Eigen::MatrixXd U = svd.matrixU();
        const Eigen::ArrayXd S = svd.singularValues();
        const Eigen::VectorXd S_sqrt = S.inverse().sqrt();

        // ZCA (Zero-phase whitening) transformation
        auto Tw = U * S_sqrt.asDiagonal() * U.transpose();

        Eigen::MatrixXd tmp = Tw.inverse();
        Tw_inv = tmp;
        return data * Tw;
};


int main(int argc, char** argv) {

        bool noTrop, switchFactor, switchFactorWBetween, writeGraph, elWeight, using_fs=false;
        bool writeENU, writeECEF, writeBias, loose, tight, robust = false, first_ob = true;
        int currKey=-1, trop=1, startEpoch=0, satKeyPrev=-1, sc=1, nThreads, startKey;
        int num_gps_factors=0, factorCount=-1, lastStep, firstStep, robustIter;
        double rangeWeight, phaseScale, percentFaulty, biasWeight;
        string gnssFile, insFile, outputFile, residualTxtInit="initResidaul.txt";
        string residualTxtOut="finalResidual.txt",textExtension=".txt", strategy;
        string switchExtension = "Switch.txt", graphExtension=".dot", dir;
        vector<string> satIndexLiteral, timeIndex;
        vector<rnxData> data;
        vector<pair<int,int> > pseudorangeFactors, priorFactors, betweenFactors;
        typedef std::tuple<int, int, int>  tup;
        vector<tup> phaseFactors, ambFactors;
        Values initial_values, optimized_values;
        Eigen::MatrixXd residuals;

        cout.precision(10);
        // define std out print color
        const string red("\033[0;31m");
        const string green("\033[0;32m");
        const string lineBreak = "###########################\n";

        NonlinearFactorGraph graph;

        po::options_description desc("Available options");
        desc.add_options()
                ("help,h", "Print help message")
                ("gpsObs,i", po::value<string>(&gnssFile)->default_value(""),
                "Input GNSS data file")
                ("percentFaulty", po::value<double>(&percentFaulty)->default_value(0.0),
                "Percentage of observations to add faults. (scale [0,1]) )")
                ("outFile,o", po::value<string>(&outputFile)->default_value("initResults"),
                "Write graph and solution to the specified file.")
                ("firstStep,f", po::value<int>(&firstStep)->default_value(0),
                "First step to process from the dataset file")
                ("lastStep,l", po::value<int>(&lastStep)->default_value(-1),
                "Last step to process, or -1 to process until the end of the dataset")
                ("threads", po::value<int>(&nThreads)->default_value(-1),
                "Number of threads, or -1 to use all processors")
                ("noTrop", "Will turn residual troposphere estimation off. Troposphere will still be modeled.")
                ("robustIter",po::value<int>(&robustIter)->default_value(1),
                "Number of iterations before initial postfit data edit")
                ("dir", po::value<string>(&dir)->default_value(""),
                "Total path to store generated data")
                ("elWeight,el", "Elevation angle dependant measuremnt weighting")
                ("rangeWeight", po::value<double>(&rangeWeight)->default_value(2.5),
                "Assumed Pseudorange Noise")
                ("biasWeight", po::value<double>(&biasWeight)->default_value(3e6),
                "Assumed carrier phase bias uncertianty")
                ("phaseScale", po::value<double>(&phaseScale)->default_value(100),
                "phase noise = range noise * 1/phaseScale")
                ("writeGraph",
                "Write graph to text file. Do not write large graphs (i.e. Nodes>=100)")
                ("writeECEF", "write ecef solution to file")
                ("writeENU", "write enu solution to file")
                ("writeBias", "write carrier-phase ambiguity values to file")
        ;

        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
        po::notify(vm);

        writeGraph = (vm.count("writeGraph")>0);
        writeENU = (vm.count("writeENU")>0);
        writeECEF = (vm.count("writeECEF")>0);
        writeBias = (vm.count("writeBias")>0);
        noTrop = (vm.count("noTrop") > 0);

        if ( gnssFile.empty() ) {
                cout << red << "\n\n GNSS data must be specified\n"
                     << "\n\n" << green << desc << endl;
                exit(1);
        }

        if ( noTrop ) { trop = 0; }

        #ifdef GTSAM_USE_TBB
        std::auto_ptr<tbb::task_scheduler_init> init;
        if(nThreads > 0) {
                cout << "\n\n Using " << nThreads << " threads " << endl;
                init.reset(new tbb::task_scheduler_init(nThreads));
        }
        else
                cout << green << " \n\n Using threads for all processors" << endl;
        #else
        if(nThreads > 0) {
                cout << red <<" \n\n GTSAM is not compiled with TBB, so threading is"
                     << " disabled and the --threads option cannot be used."
                     << endl;
                exit(1);
        }
        #endif

        // set up directory to store all generated data
        if ( dir.empty() ) { dir = getTimestamp(); }
        makeDir( dir );
        chdir( dir.c_str() );

        // Specify the starting location of the platform in ECEF XYZ.
        // Nom. pos. for the phastball dataset.
        // Point3 nomXYZ(820699.8121, -4888612.5490, 4000524.7052);
        // greenhouse
        Point3 nomXYZ(856514.1467,-4843013.0689, 4047939.8237);
        // Nom. pos. for the dec12 dataset.
        // Point3 nomXYZ(856295.3346, -4843033.4111, 4048017.6649);
        Point3 nomNED(0.0, 0.0, 0.0);

        double output_time = 0.0;
        rangeWeight = pow(rangeWeight,2);
        double phaseWeight = pow(rangeWeight*(1/phaseScale), 2);

        phaseBias bias_state(Z_1x1);
        gnssStateVector phase_arc(Z_34x1);
        gnssStateVector bias_counter(Z_34x1);
        for (int i=1; i<34; i++) {bias_counter(i) = bias_counter(i-1) + 10000; }
        nonBiasStates initEst((gtsam::Vector(5) << 0,0,0,0,0).finished());

        using symbol_shorthand::X; // nonBiasStates ( dx, dy, dz, trop, cb )
        using symbol_shorthand::G; // bias states ( Phase Bias )

        noiseModel::Diagonal::shared_ptr nonBias_InitNoise = noiseModel::Diagonal::Variances((gtsam::Vector(5) << 5.0, 5.0, 5.0, 3e6, 1e-1).finished());

        noiseModel::Diagonal::shared_ptr nonBias_ProcessNoise = noiseModel::Diagonal::Variances((gtsam::Vector(5) << 0.5, 0.5, 0.5, 1e3, 1e-3).finished());

        noiseModel::Diagonal::shared_ptr initNoise = noiseModel::Diagonal::Variances((gtsam::Vector(1) << biasWeight).finished());

        // Read GNSS data
        try {data = readGNSS_SingleFreq(gnssFile); }
        catch(std::exception& e)
        {
                cout << red << "\n\n Cannot read GNSS data file " << endl;
                exit(1);
        }

        strategy = "GNSS Only using VDP in conjuction with max-mixtures";
        cout << green << "\n\n" << lineBreak << " GNSS Data File :: "
             << gnssFile << endl;
        cout << "\n Processing Strategy  :: "  << strategy << endl;
        cout << lineBreak << endl;

        if ( firstStep != 0 ) {
                using_fs = true;
                for( unsigned int i = 0; i < data.size(); i++ ) {
                        if ( firstStep == get<1>(data[i]) ) { break; }
                        ++startEpoch;
                }
        }
        if ( lastStep < 0 ) { lastStep = get<0>(data.back()); }

        // Construct Graph --- Only Pseudorange factors
        for(unsigned int i = startEpoch; i < data.size()-1; i++ ) {

                // Get the current epoch's observables
                double gnssTime = get<0>(data[i]);
                int currKey = get<1>(data[i]);
                if (first_ob || using_fs) {
                        startKey = currKey; first_ob=false; using_fs=false;
                        graph.add(PriorFactor<nonBiasStates>(X(currKey), initEst,  nonBias_InitNoise));
                        priorFactors.push_back(make_pair(i,++factorCount));
                        optimized_values.insert(X(currKey), initEst);
                }
                int nextKey = get<1>(data[i+1]);
                int svn = get<2>(data[i]);
                Point3 satXYZ = get<3>(data[i]);
                double rho = get<4>(data[i]);
                double range = get<5>(data[i]);
                double phase = get<6>(data[i]);
                double phase_break = get<7>(data[i]);

                if ( currKey == startKey || phase_arc[svn]!=phase_break)
                {
                        bias_state[0] = phase-range;
                        if (currKey > startKey) { bias_counter[svn] = bias_counter[svn] +1; }
                        optimized_values.insert(G(bias_counter[svn]), bias_state);
                        graph.add(boost::make_shared<PriorFactor<phaseBias> >(G(bias_counter[svn]), bias_state,  initNoise));
                        phase_arc[svn] = phase_break;
                        ambFactors.push_back(tup(i, ++factorCount, bias_counter[svn]));
                }

                // Generate pseudorange factor
                graph.add(boost::make_shared<PseudorangeFactor>(X(currKey), (range-rho), satXYZ, nomXYZ,  diagNoise::Variances( (gtsam::Vector(1) << elDepWeight(satXYZ, nomXYZ, rangeWeight)).finished())));

                pseudorangeFactors.push_back(make_pair(i,++factorCount));

                // Generate phase factor
                graph.add(boost::make_shared<PhaseFactor>(X(currKey), G(bias_counter[svn]), (phase-rho), satXYZ, nomXYZ, diagNoise::Variances( (gtsam::Vector(1) << elDepWeight(satXYZ, nomXYZ, phaseWeight)).finished() )));

                phaseFactors.push_back( tup(i, ++factorCount, bias_counter[svn]) );
                satIndexLiteral.push_back(to_string(currKey) + " " + to_string(svn));

                // Add prior and between factors
                if ( currKey != nextKey && nextKey != 0) {
                        if ( lastStep == nextKey ) { break; }
                        timeIndex.push_back(to_string(gnssTime));
                        double scale = (get<0>(data[i+1])-get<0>(data[i]))*10.0;
                        optimized_values.insert(X(nextKey),initEst);
                        nonBias_ProcessNoise = noiseModel::Diagonal::Variances((gtsam::Vector(5) << 0.5*scale, 0.5*scale, 0.5*scale, 1e3*scale, 1e-3*scale).finished());

                        graph.add(boost::make_shared<BetweenFactor<nonBiasStates> >(X(currKey), X(nextKey), initEst, nonBias_ProcessNoise));

                        betweenFactors.push_back(make_pair(i, ++factorCount));
                }
        }

        // Robust Optimizatoin Loop
        //
        // 1) Optimizer graph
        // 2) Cluster residuals --> find inlier/outlier distributions
        // 3) Update previous graph with new noise model.
        //
        //  Iterate over steps until convergence.
        bool keepGoing = true;
        bool firstIteration = true;
        int numIters = 0;

        if (robustIter == 0)
        {
                optimized_values = LevenbergMarquardtOptimizer(graph, optimized_values).optimize();
                keepGoing = false;
        }


        // Cluster
        StickBreak weights;
        vector<GaussWish> clusters;
        Eigen::MatrixXd qZ;

        while (keepGoing) {

                // Optimize graph
                LevenbergMarquardtOptimizer optimizer(graph, optimized_values);
                optimizer.iterate();
                optimizer.iterate();
                optimizer.iterate();
                optimized_values.update( optimizer.values() );

                // string ecefSol = "ecef" + to_string(numIters) + ".sol";
                // ofstream os(ecefSol);
                // writeEarthFrame( optimized_values, nomXYZ, ecefSol );

                ++numIters;
                if(numIters > robustIter) {
                        keepGoing = false;
                        break;
                }


                residuals.setZero(pseudorangeFactors.size(),2);
                for (int i = 0; i<pseudorangeFactors.size(); i++) {
                        residuals.block(i,0,1,2) << graph.at(pseudorangeFactors[i].second)->residual(optimized_values)[0], graph.at(get<1>(phaseFactors[i]))->residual(optimized_values)[0];

                        //cout << graph.at(pseudorangeFactors[i].second)->residual(optimized_values)[0] << " " << graph.at(get<1>(phaseFactors[i]))->residual(optimized_values)[0] << endl;
                }

                // variational dirichlet process.
                //Eigen::MatrixXd whitened_residuals = whitenData(residuals);
                learnVDP(residuals, qZ, weights, clusters);
                Tw_inv = Eigen::MatrixXd::Identity(2,2);
                //whitened_residuals.resize(0,0);

                Eigen::ArrayXd::Index hypInd, tmpInd;
                Eigen::MatrixXd::Index nullInd;
                double maxWeight = weights.Elogweight().exp().maxCoeff(&hypInd);

                int ind=0;
                double maxTrace = 0, currTrace;
                for (vector<GaussWish>::iterator k=clusters.begin(); k < clusters.end(); ++k) {
                        currTrace = (k->getcov()*Tw_inv).trace();
                        if (currTrace > maxTrace) {
                                maxTrace = currTrace;
                                nullInd = ind;
                        }
                        ind++;
                }

                cout << endl << "Cluster Weights:" << endl;
                cout << weights.Elogweight().exp().transpose() << endl;

                cout << endl << "Cluster means:" << endl;
                for (vector<GaussWish>::iterator k=clusters.begin(); k < clusters.end(); ++k)
                        cout << k->getmean() * Tw_inv << endl;

                cout << endl << "Cluster covariances:" << endl;
                for (vector<GaussWish>::iterator k=clusters.begin(); k < clusters.end(); ++k)
                        cout << k->getcov() * Tw_inv << endl << endl;

                // Update previous graph with mixture factors
                // NOTE:: This assumes that every observation contains both range & phase
                // NOTE:: Change this to only update factors which move into new cluster
                for (std::size_t i = 0; i<pseudorangeFactors.size(); i++) {

                        int dataInd = pseudorangeFactors[i].first;
                        int currKey = get<1>(data[dataInd]);
                        int biasKey = get<2>(phaseFactors[i]);
                        int svn = get<2>(data[dataInd]);
                        Point3 satXYZ = get<3>(data[dataInd]);
                        double rho = get<4>(data[dataInd]);
                        double range = get<5>(data[dataInd]);
                        double phase = get<6>(data[dataInd]);

                        Eigen::ArrayXd::Index tmpInd;
                        auto likelihoods = qZ.row(i).maxCoeff(&ind);
                        Eigen::MatrixXd cov = clusters[ind].getcov()*Tw_inv;
                        Eigen::VectorXd mean = clusters[ind].getmean()*Tw_inv;

                        noiseModel::Gaussian::shared_ptr rangeHypothesis = noiseModel::Gaussian::Covariance(
                                ( (Vector(1) << cov(0,0) ).finished() ).asDiagonal() );

                        noiseModel::Gaussian::shared_ptr phaseHypothesis = noiseModel::Gaussian::Covariance(
                                ( (Vector(1) << cov(1,1) ).finished() ).asDiagonal() );

                        // Update range factor
                        auto rangeFactor = boost::make_shared<PseudorangeFactor>(X(currKey), ((range-mean(0))-rho), satXYZ, nomXYZ, rangeHypothesis );

                        graph.replace(pseudorangeFactors[i].second, rangeFactor);

                        // Update phase factor
                        auto phaseFactor = boost::make_shared<PhaseFactor>(X(currKey), G(biasKey), ((phase-mean(1))-rho), satXYZ, nomXYZ, phaseHypothesis);

                        graph.replace(get<1>(phaseFactors[i]), phaseFactor);


                }

                // Update noise model on amb factors
//		for (std::size_t i = 0; i<ambFactors.size(); i++)
//		{

//			auto graph_ind = get<1>(ambFactors[i]);
                //	auto phase_key = get<2>(ambFactors[i]);

                //	auto ambState = optimized_values.at<phaseBias>(G(phase_key));

                //       auto ambFactor = boost::make_shared<PriorFactor<phaseBias>>(G(phase_key),ambState,initNoise);
                //	graph.replace(graph_ind, ambFactor);
//		}
                for (int i = 0; i<ambFactors.size(); i++) {
                        cout << graph.at(get<1>(ambFactors[i]))->residual(optimized_values)[0] << endl;
                }


        }

        try {
                // Render to PDF using "fdp pseudorange.dot -Tpdf > graph.phf"
                string enuSol = "enu.sol";
                string ecefSol = "ecef.sol";
                string optimizedGraph = "graph.dot";
                string resultString = "state.values";
                string biasString = "bias.values";
                writeStates( optimized_values, timeIndex, resultString );
                if (writeENU) { ofstream os(enuSol); writeNavFrame( optimized_values, nomXYZ, timeIndex, enuSol ); }
                if (writeGraph) { ofstream os(optimizedGraph); graph.saveGraph(os,optimized_values); }
                if (writeECEF) { ofstream os(ecefSol); writeEarthFrame( optimized_values, nomXYZ, timeIndex, ecefSol ); }
                if (writeBias) {ofstream os(biasString); writeAmbiguity(optimized_values, biasString, satIndexLiteral); }
        }
        catch(std::exception& e) { cout << e.what() << endl; exit(1); }
        return 0;
}
