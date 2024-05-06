# 15618_project - Cache Optimizations using POPs Protocol

## Summary
We propose an optimization for cache coherence protocol for private and shared data inspired by POPS [1]. The optimization aims to delegate meta-data storage to only one of the caches (either L1 or L2), hence increasing effective cache capacity. We aim to utilize trace files to analyze the performance the proposed optimization and provide insightful metrics regarding performance and utilization.
## Background
Chip multiprocessors tend to organize their caches in increasing order of sizes,
with L1 − Li caches being private to a processor and Li+1 being shared by all
the processors. This arrangement reduces the latency to access DRAM or lower-level memory but requires significant work to keep the caches in the correct
state when multiple processors access a common resource. Cache coherence is
critical to maintain the correctness of the program and to allow the user to
carefully orchestrate their programs to produce an expected result. Popular
cache coherence protocols like MESIF and MOESI are based on snooping and
require the processor to actively listen to the messages on the interconnect, on
the other hand, directory-based protocols need to store additional information
about the processors with whom they are sharing memory locations. We propose
an optimization that is inspired by POPS [1] too.
## The Challenges
* Understanding the POPs protocol presented in the paper.
1
* Understanding the cache framework provided by Professor Railing because
we are making modifications on top of it to implement our design.
* The complexity of implementing the POPs protocol since we need to add
some hardware components, such as bloom filters and predictor tables, to
the existing cache architecture.
* Performance and analysis on the optimized cache and base cache.
## Resources
* Understanding the POPs protocol presented in the paper.
* Understanding the cache framework provided by Professor Railing because
we are making modifications on top of it to implement our design.
* The complexity of implementing the POPs protocol since we need to add
some hardware components, such as bloom filters and predictor tables, to
the existing cache architecture.
* Performance and analysis on the optimized cache and base cache.
## Goals and Deliverables
### 5.1 Plan to Achieve
* Simulate the POPS-inspired protocol on a simulator (preferably from 15-346) for multiprocessor NUMA caches (private L1 and shared L2).
* The simulator will provide the user with the functionality to set cache
parameters and processor count.
* The simulator will provide the ability to profile the proposed system
against MESIF and MOESI protocols and showcase relevant metrics like
cache hits/misses, latency and interconnect traffic load.
### 5.2 Hope to Achieve
* Profile the proposed protocol against other coherence protocols such as
directory based.
* The simulator will provide the ability to use a different interconnect.
### 5.3 Deliverables
* Analysis of MESIF/MOESI protocols against the proposed protocol.
* Ablation study of various tuneable parameters such as cache sizes, pro-
cessor count.
* Performance of the protocol under trace-driven workloads.
## Platform choice
* The main developing environment will be on GHC machine; however, we
might use PSC machine to do some in-depth analysis.
* We will build our design on top of the cache framework that Professor
Railing provided.
* We will us C++ as our programming language for the design.
* If we have extra time, we will build our design on top of gem5 simulator.
## Schedule 
| Week   | Tasks |
|--------|-------|
| Week 1 | Study the paper of cache optimization using POPS and study the cache simulator provided by professors |
| Week 2 | Start implementing our design |
| Week 3 | Finish implementation and verification on our design |
| Week 4 | Perform analysis and gather data for both our optimized design and base design |
| Week 5 | Work on the report and potentially scale our design from the simulator the professor provided to gem5 |

##Final Report
![image](https://github.com/chsu2023/15618_project.github.io/assets/70235411/f20212e6-d755-4170-ba97-88a3d3e03d4e)
![image](https://github.com/chsu2023/15618_project.github.io/assets/70235411/d1253cee-0179-4ec8-9db7-c9d3d5181f27)




