[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/7RAFczHS)
# Project 4 - HashTable

Project description can be found [in the repo](Project4_HashTable.pdf)

Place your complexity analysis below.

## Time Complexity Analysis

### Core Methods

| Method            | Time Complexity         | Justification                                                                 |
|------------------|-------------------------|------------------------------------------------------------------------------|
| `insert`         | O(1) average, O(n) worst | Uses pseudo-random probing; may require full table scan or resize.          |
| `remove`         | O(1) average, O(n) worst | Probes until key is found or ESS is hit.                                    |
| `contains`       | O(1) average, O(n) worst | Delegates to `get`; same probing behavior.                                  |
| `get`            | O(1) average, O(n) worst | Probes pseudo-randomly until match or ESS.                                  |
| `operator[]`     | O(1) average, O(n) worst | Same as `get`; inserts default if key is missing.                           |
| `keys`           | O(n)                    | Linear scan of all buckets to collect keys.                                 |
| `alpha`          | O(1)                    | Direct division of size and capacity.                                       |
| `capacity`       | O(1)                    | Returns vector size.                                                        |
| `size`           | O(1)                    | Returns internal counter `m_size`.                                          |

### Additional Methods

| Method              | Time Complexity       | Justification                                                                 |
|--------------------|-----------------------|------------------------------------------------------------------------------|
| `resize`           | O(n)                  | Rehashes all NORMAL entries into a new table.                               |
| `rehashBackwards`  | O(n log n)            | Sorts keys by ASCII sum, then reinserts.                                    |
| `debugDumpToJSON`  | O(n)                  | Iterates through all buckets and writes metadata to file.                   |


