# Runner

## Requirements
- macOS or Linux. 
- Python 3.6 or higher.
- CMake 2.8 or higher.
- memlimit parameter works only on Linux operating systems.
- valgrind (for valgrind or callgrind options to work).

## Overview
In **every** experimentation project there are some steps that are repeated. Typically, code is created from scratch from project to project and these steps are solved once and again. This is a problem since more work has to be done and no standardization is achieved, leading to more difficult code bases. These steps are the following:
1. Compile the project.
2. Select a group of instances from a dataset.
3. Run some experiments on those instances (*an experiment is an executable file plus a set of parameters*).
4. Get the output (and logs) of the experiment and send them to a file.
5. Visualize the results.

This framework scope is points (1)-(4). Before knowing how it copes with every task we need to know the parts that compose the framework.

## Example usage
An example of the usage of Runner can be found in the folder ["example"](example) where the knapsack problem is solved and executed on some datasets of instances.

The example has two implementations: knapsack_dp (*dynamic programming*) and knapsack_bt (*backtracking*). It has a dataset with two kinds of instances "easy" and "hard". There are two experiment files, each one executes all implementations but each one on a different kind of instances.

To run the example:
```
python3 runner.py example/experiments/experiment_easy.json example_experiments_hard.json 
```
The output will be stored in the **example/output/** folder.

## Executing Runner
To run python3 we need to specify the experiment files to execute:

```python3 runner.py <experiment_file1.json> [<experiment_file_2.json> ...]```

Options:
- --instances -I <instance1> [<instance2> ...]: only execute instances with the specified names (default: all).
- --exps -E <experiment_name1> [<experiment_name2> ...]: only execute experiment with the specified names (default: all).
- --silent -S: do not print execution stderr (default: false).
- --memlimit -M <limit_in_gb>: memory limit for each experiment execution (in GB). (default: 64).
- --valgrind -V: execute experiments with valgrind (debug mode). (default: false).
- --callgrind -C: execute experiments with valgrind and the callgrind tool (debug mode). (default: false).

Runner will execute all experiment files and for each one will generate an output file with the format "date-<experiment-file-name>.json" in the output directory.
The following pseudocode shows what Runner does:
```
  for experiment_file in experiment_files:
    output = []
    for instance in experiment_file:
      for experiment in experiment_file:
        solutions <- the entries of solutions.json for the instance
        executable <- experiment["executable"]
        run command (echo experiment; echo instance; echo solutions) | executable
        parse command output and add it to output array.
    save output to output directory.
```
> Note: The executables receive three JSON objects in the STDIN with the experiment JSON, the instance JSON and the solutions JSON respectively.

## Configuring Runner
First modify the config.json file with the following information:
- cmakelists_dir: This is the directory where the CMakeLists.txt file that compiles the project is located.
- instances_dir: This is the directory that contains the datasets (see the structure that this directory must have below).
- output_dir: This is the directory where the output files of the experiments will be stored.

## Instances directory structure
The structure of the *instances* directory must be the following:
- instances
  - dataset_1
    - **index.json**
    - **solutions.json**
    - instance_file1.json
    - ...
    - instance_file_n.json
  - ...

Each dataset must have a directory and the directory name will be used as the dataset name.
Inside each dataset directory, two special files must be present:
### index.json
The file contains a JSON array with one entry per instance with the following format:
- **file_name**: file name inside the dataset directory (example: instance_file1.json).
- **instance_name**: name that will be used as the instance identifier.
- **tags**: array of strings that will be used in the experiments to select or filter certain instances from the dataset.

Example:
```javascript
[
  {"file_name": "instance_file1.json", "instance_name": "instance1", "tags": ["EASY", "RANDOM"]},
  {"file_name": "instance_file2.json", "instance_name": "instance2", "tags": ["HARD", "RANDOM"]}
] 
```

### solutions.json
This file contains the known solutions of the dataset. It is a JSON array with one entry per solution:
- **instance_name**: name of the solution's instance.
- other attributes - depend on the problem.

Example:
```javascript
[
  {"instance_name": "instance1", "value": 1000.00, "path": [0,1,4,0], "tags": ["OPTIMUM"]},
  {"instance_name": "instance1", "value": 1200.00, "path": [0,5,4,0], "tags": ["HEURISTIC"]},
  {"instance_name": "instance2", "value": 552.20, "path": [0,1,3,0], "tags": ["FEASIBLE"]}
]
```

### <instance_file>.json
Each instance file must be a JSON object with the instance information.
Example (knapsack instance):
```javascript
{"capacity":200, "item_count":5, "item_weights":[50,70,30,80,90], "item_values":[5,20,3,10,50]}
```

## Compiling
Runner compiles the project before executing any experiment. In order to do so, it uses the CMakeLists.txt file located in the folder specified in the config.json file. Depending on whether valgrind is present or not it compiles the project in Release or Debug mode into the obj/release or obj/debug folders.

## Experiment files
An experiment file specify the instances that must be used, the executable files, and the parameter sets to send as input to those executable files. The format is as follows:
- datasets: array of JSON objects with the following attributes
  - name: name of the dataset directory in the instances folder.
  - [select] (optional): a string that specifies which instances in the dataset to select. The string has the format "[!]TAG1 [!]TAG2 ... [!]TAGK | [!]TAGK+1 ... | ...". Groups of tags are separated by '|', each group has many tags separated by spaces. A tag may be preceded by a '!' character meaning __do not include__. An instance will be selected if it matches with at least one tag group.
- experiments: array of JSON objects with the following attriutes
  - name: name of the experiment.
  - executable: name of the executable file to run (as generated by CMakeLists.txt).
  - other parameters that are useful for the executable.

Example:
```javascript
{
  "datasets": [{"name": "dataset1"}, {"name": "dataset2", "select": "HARD !RANDOM | EASY"}],
  "experiments": [
    { "name": "preprocessing", "executable": "knapsack", "time_limit":3600, "preprocess": true },
    { "name": "no-preprocessing", "executable": "knapsack", "time_limit":3600, "preprocess": false },
  ]
}
```
In this example, two experiments will be executed, each one will execute the knapsack binary file, and has two extra parameters "time_limit" and "preprocess". They will be executed in all instances from *dataset1* and all instances from *dataset2* that either have the "EASY" tag in the **index.json** file or have the "HARD" tag but not the "RANDOM" tag.

## Output file
Each time an executable file gets executed it returns 3 values: exit_code, stdout, stderr.
The JSON object that results from an execution will depend on the exit_code. It will always include the following attributes:
- experiment_name: name attribute of the current experiment.
- dataset_name: name of the dataset being used.
- instance_name: name of the instance being used.
- exit_code: the execution exit_code (0 means successful).
- stderr: string of the STDERR output of the execution.
- stdout: JSON object returned in the STDOUT output of the execution.

> Note: if the output in the STDOUT is not a JSON object, then the *stdout* attribute will contain the raw string.
> For every execution of an experiment on an instance, one output JSON will be added to the output file.

Example:
```javascript
[
  { "experiment_name": "preprocessing", "dataset_name": "dataset1", "instance_name": "easy_instance", exit_code: 0, stderr: "PREPROCESSING\nRUNNING\nFINISHED", stdout:"{\"solution\": [0, 1, 3, 5], \"value\": 500}"},
  { "experiment_name": "preprocessing", "dataset_name": "dataset2", "instance_name": "hard_instance", exit_code: 6, stderr: "PREPROCESSING\nRUNNING", stdout:"", "error_message":"Out of memory (MAX=6GB)"}
]
```
