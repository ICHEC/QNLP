# Hamming-distance method for sentence similarity

 1. Define an orthogonal set of basis words
 - Using basis words, describe sentence as linear combinations of these words (extract words from corpus, build meanings from dict/thesaurus and similarity window)
 - Encode the resulting state into a quantum register, $`\vert q_0 \rangle`$.
 - Create a test state for comparison with that of $`\vert q_0 \rangle`$; encode into quantum register $`\vert q_1 \rangle`$.
 - Calculate Hamming distance between $`\vert q_0\rangle`$ and $`\vert q_1 \rangle`$, and encode into quantum register $`\vert q_1 \rangle`$, overwriting the test data, as $`\vert q_1^{\prime} \rangle`$.
 - Resulting state now $`\vert \psi \rangle = \vert q_0 \rangle \otimes \vert q_1^{\prime} \rangle`$.
 - Apply $`X`$ gate to all qubits in register $`\vert q_1^{\prime} \rangle`$, ensuring that the state with the largest number of flipped bits is the closest state to the test vector.
 - Extracting the closest state now requires some more thought:
 	- **Method 1**: Consider an additional ancillary qubit $`\vert q_2 \rangle = \vert 0 \rangle`$, giving $`\vert \Psi \rangle = \vert q_0 \rangle \otimes \vert q_1^{\prime} \rangle \otimes \vert q_2 \rangle`$. Applying controlled $`R_y(\theta)`$ gates to this state, wherein the control lines are each of the qubits in register $`\vert q_1^{\prime} \rangle`$, and the angle $`\theta = \pi\frac{1}{n_{1}}`$, with $`n_1`$ being the number of qubits in register $`\vert q_1^{\prime} \rangle`$. If all the bits are set in a specific register state, then the number of $`R_y`$ applications will be such that $`\vert 0 \rangle \rightarrow \vert 1 \rangle`$. Thus, measuring the ancillary qubit and collecting statistics of the resulting states should yield the closest match.
 
 	- **Method 2**: Using Durr-Hoyer algorithm, set a threshold index value $`i`$, and mark all values below this threshold in $`\vert q_1^{\prime} \rangle`$ by negating the sign of the state. If result of quantum exponential search over a vector $`V`$, given by index $`i^{\prime}`$, are such that $`V[i] > V[i^{\prime}]`$, choose $`i^{\prime}`$ as the new threshold and repeat.
 
 	- **Method 3**: The amplitude of the Hamming distance calculation can give the closest state the highest probability out of a set of states in some arbitrary superposition. A measurement will, similar to **Method 1**, resulting in the collapse to the most likely state with highest probability. Additionally, a combination of the methods may yield the best approach.

## Worked example
Starting with Step 1. above, we define a set of orthogonal words (ie words with no related meaning) through which we can define the meaning of other words. For this, it can make sense to examine the vocabulary of Ogden's *Basic English: A General Introduction with Rules and Grammar (1930)*. Here, the author defines about 850 words, through which all others may be constructed --- in essence, a minial set to encompass and construct the meaning of all others. However, for this test case, we can consider a much simpler set of words (and even opt for a few extras not in the list).

Following the work of Coecke and others, we consider the simplified noun-verb-noun sentence structure, and define sets of words for each space, through which we can construct our full meaning space.

For nouns, we have two sets, $`n_s = \{\textrm{adult},\textrm{child},\textrm{smith},\textrm{surgeon}\}`$ and $`n_o = \{\textrm{outside},\textrm{inside}\}`$, and for verbs, $`v = \{\textrm{stand},\textrm{sit},\textrm{move},\textrm{sleep}\}`$. Given the noun-verb-noun meaning space sentence structure, we can calculate this as

```math
\left(
\begin{array}{c}
\textrm{adult} \\\\
\textrm{child} \\\\
\textrm{smith} \\\\
\textrm{surgeon} \\\\
\end{array}
\right) \otimes
\left(
\begin{array}{c}
\textrm{stand} \\\\
\textrm{sit} \\\\
\textrm{move} \\\\
\textrm{sleep} \\\\
\end{array}
\right) \otimes
\left(
\begin{array}{c}
\textrm{outside} \\\\
\textrm{inside} \\\\
\end{array}
\right)
```

Given the above meaning-space, we can begin to define entities that exist in this space, and choose to encode them into quantum states as follows:

- $`\textrm{John is an adult, and a smith}`$. The resulting state is $`\vert \textrm{John} \rangle = 1/\sqrt{2}\left(\vert \textrm{adult} \rangle + \vert \textrm{smith} \rangle\right)`$, which is essentially just a superposition of the number of entities from the basis set.
- $`\textrm{Mary is a child, and a surgeon}`$. The resulting state is $`\vert \textrm{Mary} \rangle = 1/\sqrt{2}\left(\vert \textrm{child} \rangle + \vert \textrm{surgeon} \rangle\right)`$, again, following the same as above.

With these definitions, we can look at some sentences with John and Mary. An example corpus with meaning that can exist in our meaning-space is:
*"John rests outside, and Mary walks inside"*.

In this instance, we also require meanings for *rests* and *walks*. If we examine synonyms for *rests* and cross-compare with our given vocab, we can find *sit* and *sleep*. Similarly, for *walks* we can have *stand* and *move* (though matching these may involve look-ups in a dictionary/thesaurus, and subsequently attempting to match the found words with our basis; may be costly). We can define the states of these words as $`\vert \textrm{rest} \rangle = 1/\sqrt{2}\left(\vert \textrm{sit} \rangle + \vert \textrm{sleep} \rangle\right)`$ and $`\vert \textrm{walk} \rangle = 1/\sqrt{2}\left(\vert \textrm{stand} \rangle + \vert \textrm{move} \rangle\right)`$.

Now that we have a means to define the states in terms of our given vocabulary, we can begin constructing quantum states to encode the data: firstly, however, we must decide on how to encode the states.

We can give each entity in the sets of nouns and verbs a unique binary index, where the word can be mapped to the binary string (gives the encoding value), and inversely mapped back (decoding value). Classically, we can use a key-value pair for this task, wherein the key (word) gives the value (index), and vice versa. Given that we intend to encode this data into quantum states, we opt for a sufficient number of qubits to represent our states: namely, we require $`\lceil \log_2({\textrm{num elements})} \rceil`$ qubits to represent our data from each respective set.

For this example, we opt for the following mappings:

```math
\begin{array}{c|c|c}
\mathbf{Dataset} & \mathbf{Word} & \mathbf{Bin.~Index} & \\
\hline
{n_s} & \textrm{adult} & 00 \\
{n_s} & \textrm{child} & 01 \\
{n_s} & \textrm{smith} & 10 \\
{n_s} & \textrm{surgeon} & 11 \\
\\
{v} & \textrm{stand} & 00 \\
{v} & \textrm{move} & 01 \\
{v} & \textrm{sit} & 10 \\
{v} & \textrm{sleep} & 11 \\
\\
{n_o} & \textrm{inside} & 0 \\
{n_o} & \textrm{outside} & 1 \\
\end{array}
```

To represent our statement meaning of *"John rests outside, and Mary walks inside"*, we define the following quantum states:

```math
\begin{array}{c|c|c}
\mathbf{Dataset} & \mathbf{Word} & \mathbf{State} \\
\hline
{n_s} & \textrm{John} & (\vert 00 \rangle + \vert 10 \rangle)/\sqrt{2} \\
{n_s} & \textrm{Mary} & (\vert 01 \rangle + \vert 11 \rangle)/\sqrt{2} \\
{v} & \textrm{walk} & (\vert 00 \rangle + \vert 01 \rangle)/\sqrt{2} \\
{v} & \textrm{rest} & (\vert 10 \rangle + \vert 11 \rangle)/\sqrt{2} \\
{n_o} & \textrm{inside} & \vert 0 \rangle  \\
{n_o} & \textrm{outside} & \vert 1 \rangle  \\
\end{array}
```

We construct our quantum state byt tensoring these entities, following the NVN DisCo-like formalism. If we consider the John and Mary sentences separately for the moment, they are respectively given by the states $`(1/2)*(\vert 00 \rangle + \vert 10 \rangle)\otimes (\vert 10 \rangle + \vert 11 \rangle)\otimes \vert 0 \rangle`$ for John, and $`(1/2)*(\vert 01 \rangle + \vert 11 \rangle)\otimes (\vert 00 \rangle + \vert 01 \rangle)\otimes \vert 1 \rangle`$ for Mary. Tidying these up and multiplying out gives 

```math
\begin{array}{ccc}
\textrm{John rests outside} & \rightarrow & \frac{1}{2}(\vert 00100 \rangle + \vert 00110 \rangle +\vert 10100 \rangle +\vert 10110 \rangle) \rightarrow \vert J\rangle \\
\\
\textrm{Mary walks inside} & \rightarrow & \frac{1}{2}(\vert 01001 \rangle + \vert 01011 \rangle +\vert 11001 \rangle +\vert 11011 \rangle) \rightarrow \vert M\rangle\\
\end{array}
```

where the full meaning is given by $`\vert S\rangle = \frac{\vert J \rangle + \vert M \rangle}{\sqrt{2}}`$, which is a superposition of the 8 unique encodings defined by our meaning-space and sentence. 

From here, we can now introduce an additional register of equal size to $`\vert S\rangle`$, wherein we will encode a test vector for comparison against the given encoded meanings, denoted as $`\vert T \rangle`$. This test vector will be overwritten with the Hamming distance of it against the other states, with the resulting Hamming distance giving the intended meaning, or closest possible meaning to that of the test. Continuing with our example, we use *"Adults stand inside"*, which is encoded as $`\vert T \rangle = \vert 00000 \rangle`$. Constructing our query state, $`\vert Q \rangle = \vert T \rangle \otimes \vert S\rangle`$, we get the following fully expanded state

```math
\vert Q \rangle = \frac{1}{2/\sqrt{2}} (\vert 00000 \rangle\vert 00100 \rangle + \vert 00000 \rangle\vert 00110 \rangle + \vert 00000 \rangle\vert 10100 \rangle + \vert 00000 \rangle\vert 10110 \rangle \\+\vert 00000 \rangle\vert 01001 \rangle + \vert 00000 \rangle\vert 01011 \rangle + \vert 00000 \rangle\vert 11001 \rangle + \vert 00000 \rangle\vert 11011 \rangle).
```

Marking the Hamming distance between both registers, and overwriting the test register modifies the above state to

```math
\vert Q^{\prime} \rangle = \frac{1}{2/\sqrt{2}} (\vert 00100 \rangle\vert 00100 \rangle + \vert 00110 \rangle\vert 00110 \rangle + \vert 10100 \rangle\vert 10100 \rangle + \vert 10110 \rangle\vert 10110 \rangle \\+\vert 01001 \rangle\vert 01001 \rangle + \vert 01011 \rangle\vert 01011 \rangle + \vert 11001 \rangle\vert 11001 \rangle + \vert 11011 \rangle\vert 11011 \rangle).
```

This example is simple as any state differing from $`\vert 00\cdots\rangle`$ essentially just flips the test when a $`\vert 1\rangle`$ is seen in the training.

The resulting data in register $`\vert T \rangle`$ now encodes the number of different flips between both test and training data; as such, it can be seen as a similarity measure, and such the state with the fewest flips will give the closest meaning to the posed test. The above example has only a single data set with one flip, that of $`\vert 00100 \rangle`$. By decoding the result using the mapping provided earlier, we can see that this corresponds with a meaning of *"Adult(s) sit inside"*. We can subsequently see that this data will give a non-zero overlap with $`\vert J \rangle`$.

There are some remaining questions on implementing this, however. Firstly, ensuring that the Hamming distance is encoded correctly, and such that the amplitudes are adjusted to respect the minimal difference has the highest probability of outcome. Additionally, if we wish to compare that statement with the previous corpus, we need to determine which statement encoded the resulting data-set --- a problem unless we aim to calculate many overlap integrals.