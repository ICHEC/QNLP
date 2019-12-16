/**
 * @file Gates.hpp
 * @author Lee J. O'Riordan (lee.oriordan@ichec.ie)
 * @brief Gates wrapper/storage class
 * @version 0.1
 * @date 2019-11-15
 */

#ifndef QNLP_GATECACHE
#define QNLP_GATECACHE

#include <unordered_map>
#include <unordered_set>

#include <complex>
#include <cassert>
#include <utility>
#include <vector>
#include <iostream>
#include <functional>

#include <cmath>
#include <limits>
#include "mat_ops.hpp"

namespace QNLP{

    /**
     * @brief Comparison of floating point values within the given machine epsilon
     * 
     * @tparam fpType Floating point type (float, double)
     * @param theta0 First parameter to compare
     * @param theta1 Second parameter to compare
     * @return true Abs value of the difference of params is less than given eps
     * @return false Abs value of the difference of params is greater than given eps
     */
    template <class fpType>
    inline bool fpComp(fpType theta0, fpType theta1) {
        return std::fabs(theta0 - theta1) < std::numeric_limits<fpType>::epsilon();
    }

    /**
     * @brief Meta container object for gate caching. 
     * 
     */
    struct GateMetaData {
        std::string labelGate; //String label for gate; X,Y,Z,H,I, or others

        std::size_t sqrt_depth = 0; //Depth of sqrt calculations, for use with NCU
        bool adjoint = false; //Is this an adjoint
        double theta = 0.0; //Gate parameter, if any

        GateMetaData(   std::string labelGate = "", 
                        std::size_t sqrt_depth = 0, 
                        bool adjoint = false,
                        double theta = 0.0) :   labelGate(labelGate), 
                                                sqrt_depth(sqrt_depth), 
                                                adjoint(adjoint), 
                                                theta(theta)
        { }

        bool operator==(const GateMetaData &other) const { 
            return (    !labelGate.compare(other.labelGate) &&
                        fpComp<double>(theta, other.theta) &&
                        sqrt_depth == other.sqrt_depth ); /* labels and depth should be sufficient
                    && theta == other.third);*/
        }

        friend std::ostream& operator<<(std::ostream& os, const GateMetaData& gmd){
            os << gmd.labelGate << ',' << gmd.sqrt_depth << ',' << gmd.adjoint << "," << gmd.theta;
            return os;
        }
    };

    /**
     * @brief Hashing function for GateMetaData objects, to allow storage in unordered_map. Taken from default docs example
     * 
     */
    struct GateMetaDataHasher{
        std::size_t operator()(const GateMetaData& gmd) const {
            return (
                  (std::hash<std::string>{}(gmd.labelGate)
                ^ (std::hash<double>{}(gmd.theta) << 1)) >> 1)
                ^ (std::hash<std::size_t>{}(gmd.sqrt_depth) << 1);
        }
    };

    /**
     * @brief Class to cache intermediate matrix values used within other parts of the computation. 
     * Heavily depended upon by NCU to store sqrt matrix values following Barenco et al. (1995) decomposition.
     * 
     * @tparam SimulatorType The simulator type with SimulatorGeneral as base class
     */
    template <class SimulatorType>
    class GateCache {
        private:
        std::size_t cache_depth;

        public:
        GateCache() : cache_depth(0) { };

        GateCache(SimulatorType& qSim) : GateCache() {
            //initCache(qSim, 16);
            //cache_depth = 16;
        }

        GateCache(SimulatorType& qSim, std::size_t default_depth) : cache_depth(default_depth) {
            initCache(qSim, cache_depth);
        }

        ~GateCache(){ clearCache(); }

        //Take the 2x2 matrix type from the template SimulatorType
        using GateType = decltype(std::declval<SimulatorType>().getGateX());

        //Maintain a map for each gate label (X,Y,Z, etc.), and use vectors to store sqrt (indexed by 1/2^(i), and pairing matrix and adjoint)
        std::unordered_map<std::string, std::vector< std::pair<GateType, GateType> > > gateCacheMap;
        
        void clearCache(){
            gateCacheMap.clear();
            cache_depth = 0;
        }

        /**
         * @brief Initialise the gate cache with PauliX,Y,Z and H up to a given sqrt depth
         * 
         * @param sim The simulator object
         * @param sqrt_depth The depth to which calculate sqrt matrices and their respective adjoints
         */
        void initCache(SimulatorType& sim, std::size_t sqrt_depth){
            // If we do not have a sufficient circuit depth, clear and rebuild up to given depth.
            // Could ideally use existing values and rebuild from lowest depth, but consider this
            // after we have a working implementation.

            if(cache_depth < sqrt_depth ){
                gateCacheMap.clear();
                cache_depth = 0;
            }

            if (gateCacheMap.empty()){

                gateCacheMap["X"] = std::vector< std::pair<GateType, GateType> > { std::make_pair( sim.getGateX(), adjointMatrix( sim.getGateX() ) ) };
                gateCacheMap["Y"] = std::vector< std::pair<GateType, GateType> > { std::make_pair( sim.getGateY(), adjointMatrix( sim.getGateY() ) ) };
                gateCacheMap["Z"] = std::vector< std::pair<GateType, GateType> > { std::make_pair( sim.getGateZ(), adjointMatrix( sim.getGateZ() ) ) };
                gateCacheMap["H"] = std::vector< std::pair<GateType, GateType> > { std::make_pair( sim.getGateH(), adjointMatrix( sim.getGateH() ) ) };

                for( std::size_t depth = 1; depth <= sqrt_depth; depth++ ){
                    for( auto& kv : gateCacheMap ){
                        kv.second.reserve(sqrt_depth + 1);
                        auto m = matrixSqrt<GateType>(kv.second[depth-1].first);
                        kv.second.emplace(kv.second.begin() + depth, std::make_pair( m, adjointMatrix( m ) ) );
                    }
                }
                cache_depth = sqrt_depth;
            }
        }

        /**
         * @brief Adds new gate to the cache up to a given sqrt depth
         * 
         * @param gateLabel Label of gate to index into map
         * @param gate Gate matrix
         * @param max_depth Depth of calculations for sqrt and associate adjoints
         */
        void addToCache(SimulatorType& sim, const std::string gateLabel, const GateType& gate, std::size_t max_depth){
            if(max_depth < cache_depth && gateCacheMap.find(gateLabel) != gateCacheMap.end() ){
                return;
            }
            else if(max_depth > cache_depth){
                initCache(sim, max_depth);
            }

            std::vector< std::pair<GateType, GateType> > v;

            v.reserve(max_depth + 1);
            v.push_back(std::make_pair( gate, adjointMatrix( gate ) ) );
            for( std::size_t depth = 1; depth <= max_depth; depth++ ){
                auto m = matrixSqrt<GateType>( v[depth-1].first );
                v.emplace(v.begin() + depth, std::make_pair( m, adjointMatrix( m ) ) );
            }
            gateCacheMap.emplace(std::make_pair(gateLabel, v) );
        }
    };
};
#endif
