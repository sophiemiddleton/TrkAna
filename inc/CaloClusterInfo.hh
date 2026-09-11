//
// CaloClusterInfo: calorimeter clusters with indices of hits
//
#ifndef CaloClusterInfo_HH
#define CaloClusterInfo_HH

#include <vector>
#include "EventNtuple/inc/RootVectors.hh"

namespace mu2e
{
  struct CaloClusterInfo {

    int               diskID_;              // Calo disk number
    float             time_;                // Cluster time
    float             timeErr_;             // Cluster time error
    float             energyDep_;           // Cluster energy
    float             energyDepErr_;        // Cluster energy error
    XYZVectorF        cog_;                 // Cluster centroid
    std::vector<int>  hits_;                // vector of branch indices of hits in the cluster
    unsigned          size_;                // Number of hits
    bool              isSplit_;             // Is this cluster split
    float             secondMoment;           // Second moment of cluster distribution
    float             e1;                     // Energy in 1x1 square (MeV)
    float             e2;                     // Energy in 2x2 square (MeV)
    float             e9;                     // Energy in 3x3 square (MeV)
    float             e25;                    // Energy in 5x5 square (MeV)


    CaloClusterInfo() : diskID_(0), time_(0.0), timeErr_(0.0), energyDep_(0.0), energyDepErr_(0.0), cog_(), hits_(), size_(0), isSplit_(0), secondMoment(-1), e1(-1), e2(-1), e9(-1), e25(-1) {}
    void reset() { *this = CaloClusterInfo(); }
  };
}
#endif
