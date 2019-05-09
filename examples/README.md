# Description


## Estimation Examples
- test_gnss_maxmix.cpp --> GNSS based state estimation with the Max Mixtures (MM) approach, as discussed within [1].
- test_gnss_dcs --> GNSS based state estimation with the Dynamic Covariance Scaling (DCS) approach, as discussed within [2].
- test_gnss_bce --> GNSS based state estimation with the Batch Covariance Estimation (BCE) approach, as discussed within the pdf in the main directory
  - This can also be used for L2 optimization if the *--robustIter* flag is set to 0.





<br/>
<br/>
<br/>

[1] Olson, Edwin, and Pratik Agarwal. "Inference on networks of mixtures for robust robot mapping." The International Journal of Robotics Research 32.7 (2013): 826-840.
<br/>
[2] Agarwal, Pratik, et al. "Robust map optimization using dynamic covariance scaling." 2013 IEEE International Conference on Robotics and Automation. Ieee, 2013.
