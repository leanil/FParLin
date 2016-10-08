# FParLin
Parallel evaluation of general vector-arithmetic trees

Matrix-vector operations are usually easy to parallelize, and we often want to do so when dealing with huge amounts of data. But if the expression to evaluate is complex, then it’s far from trivial to find the point where the performance gain of the parallel execution overcomes the overhead of distributing the work and collecting the results.

We present a proof-of- concept application which can process the given arithmetic tree, find the optimal point for parallelization and generate a function, that evaluates the tree accordingly. This point is found by estimating the computation cost of the tree nodes, and comparing it to a user supplied threshold, which represents the parallelization overhead.

The tree processing (which already includes type checking) can be easily extended by other features like optimizations because it’s based on F-algebras. The generated parallel function utilizes only CPU threads, but it’s possible to make it use GPU-s and clusters as well with separate thresholds for each.