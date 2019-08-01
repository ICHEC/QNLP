"""
The CircuitPrinter class creates a quantum circuit .tex file for viewing the nCU decomposition output.
Assumes the availability of the quantikz LaTeX package when compiling.
"""

class CircuitPrinter:
    def __init__(self, num_qubits):
        self.num_qubits = num_qubits
        self.ccts = []

    def ncu_cct_depth(self, num_ctrl):
        """
        Calculate the required depth for the circuit using the implemented nCU decomposition
        """
        if num_ctrl > 2:
            return 2 + 3*self.ncu_cct_depth(num_ctrl-1)
        else:
            return 5

    def ctrl_line_column(self, gate_name, ctrl, tgt, column_length):
        """
        Creates a columnar slice of the circuit with the given gate name, control and target lines.
        The number of qubits specifies the length of the column.
        """

        column = ["\qw & "]*column_length
        column[tgt] = r"\gate{{{}}} & ".format(gate_name)
        column[ctrl] = r"\ctrl{{{}}} & ".format(tgt - ctrl)
        return column

    def single_qubit_line_column(self, gate_name, index, column_length):
        """
        Creates a columnar slice of the circuit with the given gate name, on index
        The number of qubits specifies the length of the column.
        """

        column = ["\qw & "]*column_length
        column[index] = r"\gate{{{}}} & ".format(gate_name)
        return column

    def slice_line_column(self, slice_name, column_length):
        """
        Creates a columnar slice of the circuit with a dashed line denoting the marked section
        """

        column = ["\qw & "]*column_length
        if "\\" in slice_name:
            slice_name = "${}$".format(slice_name)
        column[0] = r"\qw\slice{{{}}} & ".format(slice_name)
        return column


    def load_data_csv(self, csv_file):
        """
        Loads the data from a CSV file. Assumes the following layout:
        gate_name, control_qubit_number, target_qubit_number, gate_matrix_values
        """
        import csv
        cct = []
        with open(csv_file, 'r') as csvfile:

            filereader = csv.reader(csvfile, delimiter=',')
            data = list(filereader)
            cct_local = []
            mergeable_row = 0
            for idx,row in enumerate(data[1:]):
                #Check for break in circuit runs, empty data, and single run datasets (ie currently empty output)
                prev_was_ctrl = False
                #from IPython import embed; embed()
                if (row != "\n") and (row != [])  and (row != data[-1]):
                    if row[0][0:4] == "#!#{":
                        slice_label = row[0].rsplit("}")[0].rsplit("{")[-1]
                        slice_col = self.slice_line_column(slice_label, self.num_qubits)
                        cct_local.append(slice_col)

                    elif int(row[1]) <= 2**32:
                        prev_was_ctrl = True
                        cct_local.append(self.ctrl_line_column(row[0], int(row[1]), int(row[2]), self.num_qubits) )
                    
                    else:
                        #Single qubit value; max value here indicates that the output value for the ctrl line was 2^64, hence non-existent
                        curr_col = self.single_qubit_line_column(row[0], int(row[2]), self.num_qubits)
                        '''if (len(cct_local) > 0) and (prev_was_ctrl == False):
                            prev_col = cct_local[-1]

                            icurr = [idx for (idx,val) in enumerate(curr_col) if val != "\\qw & "]
                            iprev = [idx for (idx,val) in enumerate(prev_col) if val != "\\qw & "]

                            if icurr[0] != iprev[0] and len(icurr) == 1 and len(iprev) == 1:
                                curr_row[iprev[0]] = prev_row[iprev[0]]
                                del cct_local[-1]
                        '''
                        cct_local.append(curr_col)
                        prev_was_ctrl == False
                else:
                    cct.append(cct_local)
                    cct_local = []
        self.ccts = cct
        return cct

    def row_merge():
        """
        WIP: Merges rows between independent qubit operations to reduce printed circuit size
        e.g. [I, I, I, X], [I, Z, I, I] -> [I, Z, X, I]
        """
        data = self.ccts[0]
        for i in range(1,len(data)):
            prev_mod_vals = [(idx,val) for (idx,val) in enumerate(data[i-1]) if val != "\\qw & "]
            curr_mod_vals = [(idx,val) for (idx,val) in enumerate(data[i]) if val != "\\qw & "]
            for j in prev_mod_vals:
                prev_has_ctrl = []
                if "\\ctrl" in j[1]:
                    # Terrible, but it will do for now
                    range_len_prev = int(j[1].rsplit("}")[0].rsplit("{")[1])
                    prev_has_ctrl = [(j[0], j[0] + range_len_prev)]
                for k in curr_mod_vals:
                    curr_has_ctrl = []
                    if "\\ctrl" in k[1]:
                        # Still terrible, but it will do for now
                        range_len_curr = int(k[1].rsplit("}")[0].rsplit("{")[1])
                        curr_has_ctrl = [(k[0], k[0] + range_len_curr)]
                    # Continue this later... seems incredibly inefficient



    def latex_cct(self, data_file, file_name="cct", max_depth=16):
        """
        LaTeX file outputter for quantum circuit generation.
        """
        cct_array = self.load_data_csv(data_file)
        num_cct = len(cct_array)
        depth = len(cct_array[0])

        for cct_idx in range(num_cct):
            with open(file_name + "_" + str(cct_idx) + ".tex", "w") as f:
                f.write("\\documentclass{article} \n \\usepackage{amsmath} \\usepackage{adjustbox} \\usepackage{tikz} \\usetikzlibrary{quantikz} \\usepackage[margin=0.5cm]{geometry} \n \\begin{document} \centering\n")
                # Split the circuit on a given depth boundary

                # Due to issues with latex variables ending with numeric indices, appending letters to the temporary savebox commands allows us to generate multiple of these.
                # As depth is an issue with circuits, we currently expect the output not to exceed n-choose-k of 52-C-4 = 270725 combinations
                # This will suffice until later.
                import string, itertools
                s_list = [i for i in string.ascii_letters]
                box_str = r"\Box"
                label_iterator = itertools.combinations(s_list,4)
                box_labels = []

                for i in range(0, depth, max_depth):
                    local_label = box_str + "".join( next(label_iterator))
                    box_labels.append(local_label)
                    f.write(r"\newsavebox{{{}}}".format(local_label))
                    f.write(r"\savebox{{{}}}{{".format(local_label))
                    #f.write(r"\begin{tikzpicture}\node[scale=0.5] {")
                    f.write("\\begin{quantikz}[row sep={0.5cm,between origins}, column sep={0.75cm,between origins}, slice label style={inner sep=1pt,anchor=south west,rotate=40}]")
                    #Transposes the data so that q rows of length n exist, rather than n cols of length q
                    if(i + max_depth < depth):
                        cct_list_qubit= list(map(list, zip(*cct_array[cct_idx][i:i+max_depth])))
                    else:
                        cct_list_qubit= list(map(list, zip(*cct_array[cct_idx][i:])))

                    for q in range(len(cct_list_qubit)):
                        out_str = "\\qw & ".join(cct_list_qubit[q]) + " \\qw "
                        if(q != len(cct_list_qubit)-1):
                            out_str += " \\\\ "
                        f.write(out_str)
                    f.write(" \\end{quantikz}\n}\n")
                    f.write("\n")
                #f.write("\\begin{tabular}{c}\n")
                for idx,l in enumerate(box_labels):
                    f.write(r"\usebox{{{}}} \\".format(l))
                    f.write("\n \\vspace{2em}")
                    #if(idx % 4 == 0):
                    #    f.write(r"\pagebreak \\".format(l))
                #f.write("\\end{tabular}\n")
                f.write(r"\end{document}")

if __name__== "__main__":
    import os
    args = os.sys.argv
    if len(args) < 2:
        print("Please specify the file to load, and output filename to save: python cct.py <CSV> <>")
        exit()
    CCT = CircuitPrinter(num_qubits=10)
    CCT.latex_cct(args[1], args[2], max_depth=12)
