#ifndef QNLP_GATEWRITER
#define QNLP_GATEWRITER

#include <iostream>
#include <fstream>
#include <unordered_set>

namespace QNLP{
    class GateWriter{
        private:
        std::ofstream latex_csv_out, matrix_val_out;
        std::unordered_set<std::string> gates;
        
        public:
        GateWriter() : GateWriter("qnlp_gate_calls.csv", "qnlp_matrices.log"){
        }
        GateWriter( std::string latexCSVFilename, std::string matrixFilename ){           
            //Check if file exists, add header upon creation if not
            std::ifstream file_exists(latexCSVFilename.c_str()) ;
            
            latex_csv_out.open ( latexCSVFilename, std::ofstream::out | std::ofstream::app );
            matrix_val_out.open( matrixFilename, std::ofstream::out | std::ofstream::app ); 
            
            if (!(bool) file_exists)
                latex_csv_out << "gate_label,gate_ctrl,gate_tgt,gate_value" << std::endl;
        }
        ~GateWriter(){
            latex_csv_out  << std::endl;
            matrix_val_out << std::endl;

            latex_csv_out.close();
            matrix_val_out.close();
        }

        /**
         * @brief Writes the given matrix to matrix_val_out. Assumes matrix type is printable
         * 
         * @param matrix 
         */
        void gateOut(std::string gateLabel, std::string matrixVals){
            if( !gates.count(gateLabel)){
                matrix_val_out << gateLabel << "," << matrixVals << std::endl;
                gates.insert(gateLabel);
            }
        }

        /**
         * @brief Writes the single qubit gate to disk. -1 used to denote lack of control line.
         * 
         * @param gateLabel std::string used to label gate name.
         * @param matrixVal The matrix representing the gate.
         * @param gate_idx The qubit index to apply gate.
         */
        void oneQubitGateCall(const std::string& gateLabel, const std::string& matrixVals, std::size_t gate_idx){
            twoQubitGateCall(gateLabel, matrixVals, -1, gate_idx);
        }

        /**
         * @brief Writes the controlled gate to disk.
         * 
         * @param gateLabel std::string used to label gate name.
         * @param matrixVal The matrix representing the gate.
         * @param gate_ctrl_idx The qubit index for the gate control line
         * @param gate_tgt_idx The qubit index for the gate target line
         */
        void twoQubitGateCall(const std::string gateLabel, const std::string matrixVals, std::size_t gate_ctrl_idx, std::size_t gate_tgt_idx){
            latex_csv_out << gateLabel << "," << gate_ctrl_idx << "," << gate_tgt_idx << "," << matrixVals << std::endl;
            gateOut(gateLabel, matrixVals);
        }

        /**
         * @brief Return the map of all applied gates so far
         * 
         * @return std::unordered_map<std::string, Mat2x2Type>& 
         */
        std::unordered_set<std::string>& getAppliedGates(){
            return gates;
        }
    };
}

#endif