# 15618_project

#### Title :

#### URL :

## Summary
We propose an optimization for cache coherence protocol for private and shared data inspired from POPS [1]. The optimization aims to delegate meta-data storage to only one of the caches (either L1 or L2), hence increasing effective cache capacity. We aim to utilize trace files to analyze the performance the proposed optimization and provide insightful metrics regarding performance and utilization.
## Background
Chip multiprocessors tend to organize their caches in an increasing order of sizes,
with L1 − Li caches being private to a processor and Li+1 being shared by all
the processors. This arrangement reduces the latency to access DRAM or lower
level memory but requires significant work to keep the caches in the correct
state when multiple processors access a common resource. Cache coherence is
critical to maintain the correctness of the program and to allow the user to
carefully orchestrate their programs to produce an expected result. Popular
cache coherence protocols like MESIF and MOESI are based on snooping and
require the processor to actively listen to the messages on the interconnect, on
the other hand directory based protocols need to store additional information
about the processors with whom they are sharing memory locations. We propose
an optimization that is inspired from POPS [1] too.
## Resources
• Understanding the POPs protocol presented on the paper.
• Understanding the cache framework provided by Professor Railing becasue
we are making modifications on top of it to implement our design.
• The complexity of implementing the POPs protocol since we need to add
some hardware components, such as bloom filters and predictor tables, to
the existing cache architecture.
• Performance and analysis on the optimized cache and base cache.
## Goals and Deiverables

### Platform choice

### Schedule 


