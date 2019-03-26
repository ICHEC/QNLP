# Notes on oracle access in Wiebe et al.
The oracle $`O`$ is used to extract the value of the state vector corresponding to two indices: the index of the vector in the training set, $`\textrm{vectorIdx}`$, and the index of the value within that vector at location $`\textrm{valueIdx}`$. As a result, an empty register is loaded with a state corresponding to this selected value.
The operation is carried out as follows, and has a pseudo-code like implementation to demonstrate the general idea.

```math
\begin{array}{}
O\vert \textrm{vectorIdx} \rangle \otimes \vert \textrm{valueIdx} \rangle \otimes \vert \textrm{0...} \rangle \rightarrow \\\\ \vert \textrm{vectorIdx} \rangle \otimes \vert \textrm{valueIdx} \rangle \otimes \vert \textrm{stateVector[vectorIdx][valueIdx]} \rangle
\end{array}
```

```julia
function oracleO(vectorIdx::Int64, valueIdx::Int64)
    return stateVector[vectorIdx][valueIdx]
end
```

The oracle $`F`$ similarly is used to extract the value of the state vector corresponding to two indices: as before, the index of the vector in the training set, $`\textrm{vectorIdx}`$, and now the index of the $`\textrm{nzo}`$-th non-zero value in the vector, where $`\textrm{nzo}:=`$non zero offset, representing the indexed states with a non zero coefficient.  The second $`\textrm{nzo}`$ holding register is overwritten with this value. 

```math
\begin{array}{}
F\vert \textrm{vectorIdx} \rangle \otimes \vert \textrm{nzo} \rangle \rightarrow \\\\ 
\vert \textrm{vectorIdx} \rangle \otimes \vert f(\textrm{vectorIdx}, \textrm{nzo}) \rangle \rightarrow \\\\
\vert \textrm{vectorIdx} \rangle \otimes \vert \textrm{nzoIdx} \rangle
\end{array}
```

```julia
# Consider maintaining previous index to continue 
# searching for next value.
function oracleF(vectorIdx::Int64, nzo::Int64)
    counter=0
    for (idx, val) in enumerate( stateVector[vectorIdx] )
        if val != 0 && counter < nzo
            counter++
        else
            return idx
        end
    end
end
```

Given these two operations, the number of queries depends on the number of non-zeros in the vector given by `stateVector[vectorIdx]` and the largest value in the vector, given by `max(stateVector[vectorIdx])`: 
$`\textrm{Queries}:=\textrm{nnz}(\vert \textrm{vectorIdx} \rangle)\times \max(|\vert \textrm{vectorIdx} \rangle|)^2`$.

As an example, let's consider 2 vectors with 2 values each in our set $`U = \{|v_0\rangle, |v_1\rangle\}`$, where
```math
\begin{array}{}
|v_0\rangle = \begin{pmatrix} a_0 \\ b_0\end{pmatrix} = a_0\begin{pmatrix} 1 \\ 0 \end{pmatrix} + b_0\begin{pmatrix} 0 \\ 1 \end{pmatrix} = a_0 \vert 0 \rangle  + b_0 \vert 1 \rangle \\\\ 
|v_1\rangle = \begin{pmatrix} a_1 \\ b_1\end{pmatrix} = a_1\begin{pmatrix} 1 \\ 0 \end{pmatrix} + b_1\begin{pmatrix} 0 \\ 1 \end{pmatrix} = a_1 \vert 0 \rangle  + b_1 \vert 1 \rangle
\end{array}
```

For this, we can use 1 qubit to select the appropriate vector, and 1 qubit to select the respective value in that vector (2 vectors, 2 values, 4 total combinations). Assuming an initial state as $`\vert \Psi \rangle = \vert 0 \rangle \vert 0 \rangle \vert 0 \rangle`$, where each qubit from right to left is result, value index, and vector index respectively.

Using the oracle-based access pattern on this state for all possible patterns given the following:
```math
\begin{array}{}
O\vert 0 \rangle \vert 0 \rangle \vert 0 \rangle = a_0\vert 0 \rangle \vert 0 \rangle \vert 0 \rangle \\\\
O\vert 1 \rangle \vert 0 \rangle \vert 0 \rangle = a_1\vert 0 \rangle \vert 0 \rangle \vert 0 \rangle \\\\
O\vert 0 \rangle \vert 1 \rangle \vert 0 \rangle = b_0\vert 0 \rangle \vert 0 \rangle \vert 1 \rangle \\\\
O\vert 1 \rangle \vert 1 \rangle \vert 0 \rangle = b_1\vert 0 \rangle \vert 0 \rangle \vert 1 \rangle
\end{array}
```

## State generation
For the algorithm as proposed by Wiebe et al, we start with a state $`\vert A \rangle`$ defined as follows:
```math
\vert A \rangle = \frac{1}{\sqrt{d}}\displaystyle\sum\limits_{\textrm{nzo}=1}^{d} \vert \textrm{vectorIdx}\rangle \vert \textrm{nzo} \rangle \vert 0 \rangle \vert 0 \rangle
```
Next, we must find the indices of the $`\textrm{nzo}`$, which are the indexed values with non-zero terms, denoted $`\textrm{nzoIdx}`$ by applying $`F`$ to the above state, and where $`\vert \textrm{nzoIdx} \rangle = \vert f(\textrm{vectorIdx},\textrm{nzo}) \rangle`$.

```math
\vert B \rangle = F \vert A \rangle = \frac{1}{\sqrt{d}}\displaystyle\sum\limits_{\textrm{nzo}=1}^{d} \vert \textrm{vectorIdx}\rangle \vert \textrm{nzoIdx} \rangle \vert 0 \rangle \vert 0 \rangle.
```

With this state, we can now extract the value of the vector, indexed by both $`\textrm{vectorIdx}`$ and $`\textrm{nzoIdx}`$ by applying oracle $`O`$ as:
```math
\vert C \rangle = O\vert B \rangle = \frac{1}{\sqrt{d}}\displaystyle\sum\limits_{\textrm{nzo}=1}^{d} \vert \textrm{vectorIdx}\rangle \vert \textrm{nzoIdx} \rangle \vert v_{\textrm{vectorIdx}, \textrm{nzoIdx}} \rangle \vert 0 \rangle.
```

### Aside: qRAM data access
One thing to note at this point is the similarity of the above operations to accessing memory from a quantum RAM (qRAM), where the access of data from an index $`k`$ is performed as:
```math
\displaystyle\sum_{k}^{} \alpha_k\vert k \rangle \vert 0 \rangle \xrightarrow[]{\text{qRAM}} \displaystyle\sum_{k}^{} \alpha_k\vert k \rangle \vert m_k \rangle,
```
where $`m_k`$ is the $`k`$-th cell in memory $`m`$. Clearly there is a close relationship between the oracle-based access as discussed by Wiebe et al and that of a qRAM access pattern. As such, to implement the bounded accesses as discussed by Wiebe, it seems that a qRAM model is required (or at least approximated).

