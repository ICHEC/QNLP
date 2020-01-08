#include "oracle.hpp"

#include "Simulator.hpp"
#include "IntelSimulator.cpp"
#include "catch2/catch.hpp"

#include <bitset>

using namespace QNLP;

//For simplicity, enabling complex double only
typedef ComplexDP Type;
template class QNLP::Oracle<IntelSimulator>;

TEST_CASE("3 qubit Oracle standalone class","[oracle]"){
    std::size_t num_qubits = 3;
    IntelSimulator sim(num_qubits);
    auto& reg = sim.getQubitRegister();
    Oracle<decltype(sim)> oracle;

    SECTION("2^3 bit patterns (8)"){
        // Testing patterns 000 001 010 011, etc.
        const std::vector<std::size_t> bit_patterns {0, 1, 2, 3, 4, 5, 6, 7};

        //Declare which indices are control lines
        std::vector<std::size_t> ctrl_indices;
        for(std::size_t i = 0; i < num_qubits-1; ++i){
            ctrl_indices.push_back(i);
        }

        // Loop over given bit-patterns and show effect on resulting state
        for(auto &i : bit_patterns){
            DYNAMIC_SECTION("bitStringNCU with pattern " << i){
                sim.initRegister();
                //Create initial superposition
                for(std::size_t j = 0; j < num_qubits; ++j){
                    sim.applyGateH(j);
                }

                // Mark state: convert matching state pattern to |11...1>
                // apply nCZ, and undo conversion; negates the matched pattern phase
                oracle.bitStringNCU(sim, i, ctrl_indices, num_qubits-1, sim.getGateZ(), "Z");

                CAPTURE( reg[i], i );
                REQUIRE( reg[i].real() < 0.);
            }

            DYNAMIC_SECTION("bitStringPhaseOracle with pattern " << i){
                sim.initRegister();
                //Create initial superposition
                for(std::size_t j = 0; j < num_qubits; ++j){
                    sim.applyGateH(j);
                }

                // Mark state: convert matching state pattern to |11...1>
                // apply nCZ, and undo conversion; negates the matched pattern phase
                oracle.bitStringPhaseOracle(sim, i, ctrl_indices, num_qubits-1);

                CAPTURE( reg[i], i );
                REQUIRE( reg[i].real() < 0.);
            }
        }
    }

    SECTION("4 bit patterns"){
        // Testing patterns 000 001 010 011, etc.
        const std::vector<std::size_t> bit_patterns {0, 5, 6, 7};

        //Declare which indices are control lines
        std::vector<std::size_t> ctrl_indices;
        for(std::size_t i = 0; i < num_qubits-1; ++i){
            ctrl_indices.push_back(i);
        }

        // Loop over given bit-patterns and show effect on resulting state
        for(auto &i : bit_patterns){
            DYNAMIC_SECTION("bitStringNCU with pattern " << i){
                sim.initRegister();
                //Create initial superposition
                for(std::size_t j = 0; j < num_qubits; ++j){
                    sim.applyGateH(j);
                }

                // Mark state: convert matching state pattern to |11...1>
                // apply nCZ, and undo conversion; negates the matched pattern phase
                oracle.bitStringNCU(sim, i, ctrl_indices, num_qubits-1, sim.getGateZ(), "Z");

                CAPTURE( reg[i], i );
                REQUIRE( reg[i].real() < 0.);
            }

            DYNAMIC_SECTION("bitStringPhaseOracle with pattern " << i){
                sim.initRegister();
                //Create initial superposition
                for(std::size_t j = 0; j < num_qubits; ++j){
                    sim.applyGateH(j);
                }

                // Mark state: convert matching state pattern to |11...1>
                // apply nCZ, and undo conversion; negates the matched pattern phase
                oracle.bitStringPhaseOracle(sim, i, ctrl_indices, num_qubits-1);

                CAPTURE( reg[i], i );
                REQUIRE( reg[i].real() < 0.);
            }
        }
    }
}

TEST_CASE("8 qubit Oracle standalone class","[oracle]"){
    std::size_t num_qubits = 8;
    IntelSimulator sim(num_qubits);
    auto& reg = sim.getQubitRegister();
    Oracle<decltype(sim)> oracle;

    SECTION("2^8 bit patterns (256)"){
        // Testing patterns 000 001 010 011, etc.
        std::vector<std::size_t> bit_patterns;
        for (std::size_t s = 0; s < (std::size_t) (0b1 << num_qubits); s++){
            bit_patterns.push_back(s);
        }

        //Declare which indices are control lines
        std::vector<std::size_t> ctrl_indices;
        for(std::size_t i = 0; i < num_qubits-1; ++i){
            ctrl_indices.push_back(i);
        }

        // Loop over given bit-patterns and show effect on resulting state
        for(auto &i : bit_patterns){
            DYNAMIC_SECTION("bitStringNCU with pattern " << i){
                sim.initRegister();
                //Create initial superposition
                for(std::size_t j = 0; j < num_qubits; ++j){
                    sim.applyGateH(j);
                }

                // Mark state: convert matching state pattern to |11...1>
                // apply nCZ, and undo conversion; negates the matched pattern phase
                oracle.bitStringNCU(sim, i, ctrl_indices, num_qubits-1, sim.getGateZ(), "Z");

                CAPTURE( reg[i], i );
                REQUIRE( reg[i].real() < 0.);
            }

            DYNAMIC_SECTION("bitStringPhaseOracle with pattern " << i){
                sim.initRegister();
                //Create initial superposition
                for(std::size_t j = 0; j < num_qubits; ++j){
                    sim.applyGateH(j);
                }

                // Mark state: convert matching state pattern to |11...1>
                // apply nCZ, and undo conversion; negates the matched pattern phase
                oracle.bitStringPhaseOracle(sim, i, ctrl_indices, num_qubits-1);

                CAPTURE( reg[i], i );
                REQUIRE( reg[i].real() < 0.);
            }
        }
    }

    SECTION("4 bit patterns"){
        // Testing patterns 000 001 010 011, etc.
        const std::vector<std::size_t> bit_patterns {0, 5, 6, 7};

        //Declare which indices are control lines
        std::vector<std::size_t> ctrl_indices;
        for(std::size_t i = 0; i < num_qubits-1; ++i){
            ctrl_indices.push_back(i);
        }

        // Loop over given bit-patterns and show effect on resulting state
        for(auto &i : bit_patterns){
            DYNAMIC_SECTION("bitStringNCU with pattern " << i){
                sim.initRegister();
                //Create initial superposition
                for(std::size_t j = 0; j < num_qubits; ++j){
                    sim.applyGateH(j);
                }

                // Mark state: convert matching state pattern to |11...1>
                // apply nCZ, and undo conversion; negates the matched pattern phase
                oracle.bitStringNCU(sim, i, ctrl_indices, num_qubits-1, sim.getGateZ(), "Z");

                CAPTURE( reg[i], i );
                REQUIRE( reg[i].real() < 0.);
            }

            DYNAMIC_SECTION("bitStringPhaseOracle with pattern " << i){
                sim.initRegister();
                //Create initial superposition
                for(std::size_t j = 0; j < num_qubits; ++j){
                    sim.applyGateH(j);
                }

                // Mark state: convert matching state pattern to |11...1>
                // apply nCZ, and undo conversion; negates the matched pattern phase
                oracle.bitStringPhaseOracle(sim, i, ctrl_indices, num_qubits-1);

                CAPTURE( reg[i], i );
                REQUIRE( reg[i].real() < 0.);
            }
        }
    }
}

TEST_CASE("3 qubit Oracle simulator method","[oracle]"){
    std::size_t num_qubits = 3;
    IntelSimulator sim(num_qubits);
    auto& reg = sim.getQubitRegister();

    SECTION("2^3 bit patterns (8)"){
        // Testing patterns 000 001 010 011, etc.
        const std::vector<std::size_t> bit_patterns {0, 1, 2, 3, 4, 5, 6, 7};

        //Declare which indices are control lines
        std::vector<std::size_t> ctrl_indices;
        for(std::size_t i = 0; i < num_qubits-1; ++i){
            ctrl_indices.push_back(i);
        }

        // Loop over given bit-patterns and show effect on resulting state
        for(auto &i : bit_patterns){
            DYNAMIC_SECTION("applyOracleU with pattern " << i){
                sim.initRegister();
                //Create initial superposition
                for(std::size_t j = 0; j < num_qubits; ++j){
                    sim.applyGateH(j);
                }

                // Mark state: convert matching state pattern to |11...1>
                // apply nCZ, and undo conversion; negates the matched pattern phase
                //oracle.bitStringNCU(sim, i, ctrl_indices, num_qubits-1, sim.getGateZ());
                sim.applyOracleU(i, ctrl_indices, num_qubits-1, sim.getGateZ(), "Z");

                CAPTURE( reg[i], i );
                REQUIRE( reg[i].real() < 0.);
            }

            DYNAMIC_SECTION("applyOraclePhase with pattern " << i){
                sim.initRegister();
                //Create initial superposition
                for(std::size_t j = 0; j < num_qubits; ++j){
                    sim.applyGateH(j);
                }

                // Mark state: convert matching state pattern to |11...1>
                // apply nCZ, and undo conversion; negates the matched pattern phase
                sim.applyOraclePhase(i, ctrl_indices, num_qubits-1);

                CAPTURE( reg[i], i );
                REQUIRE( reg[i].real() < 0.);
            }
        }
    }

    SECTION("4 bit patterns"){
        // Testing patterns 000 001 010 011, etc.
        const std::vector<std::size_t> bit_patterns {0, 5, 6, 7};

        //Declare which indices are control lines
        std::vector<std::size_t> ctrl_indices;
        for(std::size_t i = 0; i < num_qubits-1; ++i){
            ctrl_indices.push_back(i);
        }

        // Loop over given bit-patterns and show effect on resulting state
        for(auto &i : bit_patterns){
            DYNAMIC_SECTION("applyOracleU with pattern " << i){
                sim.initRegister();
                //Create initial superposition
                for(std::size_t j = 0; j < num_qubits; ++j){
                    sim.applyGateH(j);
                }

                // Mark state: convert matching state pattern to |11...1>
                // apply nCZ, and undo conversion; negates the matched pattern phase
                sim.applyOracleU(i, ctrl_indices, num_qubits-1, sim.getGateZ(), "Z");

                CAPTURE( reg[i], i );
                REQUIRE( reg[i].real() < 0.);
            }

            DYNAMIC_SECTION("applyOraclePhase with pattern " << i){
                sim.initRegister();
                //Create initial superposition
                for(std::size_t j = 0; j < num_qubits; ++j){
                    sim.applyGateH(j);
                }

                // Mark state: convert matching state pattern to |11...1>
                // apply nCZ, and undo conversion; negates the matched pattern phase
                sim.applyOraclePhase(i, ctrl_indices, num_qubits-1);

                CAPTURE( reg[i], i );
                REQUIRE( reg[i].real() < 0.);
            }
        }
    }
}

TEST_CASE("8 qubit Oracle simulator method","[oracle]"){
    std::size_t num_qubits = 8;
    IntelSimulator sim(num_qubits);
    auto& reg = sim.getQubitRegister();

    SECTION("2^8 bit patterns (256)"){
        // Testing patterns 000 001 010 011, etc.
        std::vector<std::size_t> bit_patterns;
        for (std::size_t s = 0; s < (std::size_t) (0b1 << num_qubits); s++){
            bit_patterns.push_back(s);
        }

        //Declare which indices are control lines
        std::vector<std::size_t> ctrl_indices;
        for(std::size_t i = 0; i < num_qubits-1; ++i){
            ctrl_indices.push_back(i);
        }

        // Loop over given bit-patterns and show effect on resulting state
        for(auto &i : bit_patterns){
            DYNAMIC_SECTION("applyOracleU with pattern " << i){
                sim.initRegister();
                //Create initial superposition
                for(std::size_t j = 0; j < num_qubits; ++j){
                    sim.applyGateH(j);
                }

                // Mark state: convert matching state pattern to |11...1>
                // apply nCZ, and undo conversion; negates the matched pattern phase
                sim.applyOracleU(i, ctrl_indices, num_qubits-1, sim.getGateZ(), "Z");

                CAPTURE( reg[i], i );
                REQUIRE( reg[i].real() < 0.);
            }

            DYNAMIC_SECTION("applyOraclePhase with pattern " << i){
                sim.initRegister();
                //Create initial superposition
                for(std::size_t j = 0; j < num_qubits; ++j){
                    sim.applyGateH(j);
                }

                // Mark state: convert matching state pattern to |11...1>
                // apply nCZ, and undo conversion; negates the matched pattern phase
                sim.applyOraclePhase(i, ctrl_indices, num_qubits-1);

                CAPTURE( reg[i], i );
                REQUIRE( reg[i].real() < 0.);
            }
        }
    }

    SECTION("4 bit patterns"){
        // Testing patterns 000 001 010 011, etc.
        const std::vector<std::size_t> bit_patterns {0, 5, 6, 7};

        //Declare which indices are control lines
        std::vector<std::size_t> ctrl_indices;
        for(std::size_t i = 0; i < num_qubits-1; ++i){
            ctrl_indices.push_back(i);
        }

        // Loop over given bit-patterns and show effect on resulting state
        for(auto &i : bit_patterns){
            DYNAMIC_SECTION("applyOracleU with pattern " << i){
                sim.initRegister();
                //Create initial superposition
                for(std::size_t j = 0; j < num_qubits; ++j){
                    sim.applyGateH(j);
                }

                // Mark state: convert matching state pattern to |11...1>
                // apply nCZ, and undo conversion; negates the matched pattern phase
                sim.applyOracleU(i, ctrl_indices, num_qubits-1, sim.getGateZ(), "Z");

                CAPTURE( reg[i], i );
                REQUIRE( reg[i].real() < 0.);
            }

            DYNAMIC_SECTION("applyOraclePhase with pattern " << i){
                sim.initRegister();
                //Create initial superposition
                for(std::size_t j = 0; j < num_qubits; ++j){
                    sim.applyGateH(j);
                }

                // Mark state: convert matching state pattern to |11...1>
                // apply nCZ, and undo conversion; negates the matched pattern phase
                sim.applyOraclePhase(i, ctrl_indices, num_qubits-1);

                CAPTURE( reg[i], i );
                REQUIRE( reg[i].real() < 0.);
            }
        }
    }
}
