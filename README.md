# Runner
A framework designed to mitigate the overhead of working with datasets and experiments.

## Description
In **every** experimentation project there are some steps that are repeated. Typically, code is created from scratch from project to project and these steps are solved once and again. This is a problem since more work has to be done and no standardization is achieved, leading to more difficult code bases. These steps are the following:
1. Compile the project.
2. Select a group of instances from a dataset.
3. Run some experiments on those instances (*an experiment is an executable file plus a set of parameters*).
4. Get the output (and logs) of the experiment and send them to a file.
5. Visualize the results.

This framework scope is points (1)-(4). Before knowing how it copes with every task we need to know the parts that compose the framework.

## Getting Started
The following instructions will guide you through the steps to have Runner set up and running.

## Prerequisites
- macOS or Linux. 
- Python 3.6 or higher [(more info)](https://www.python.org/downloads/).
- CMake 2.8 or higher [(more info)](https://cmake.org/).
- memlimit parameter works only on Linux operating systems.
- valgrind (for valgrind or callgrind options to work) [(more info)](http://www.valgrind.org/).

## Including Runner in your project
Next we show you the steps to have runner working in your project.
1. Copy the runner.py and config.json files into a "runner" directory in your project root directory.
1. Create an "instances" directory in your project root directory.
1. Create one folder for each dataset named equal to the dataset ([see Instances Directory](#instances-directory)). For each dataset do the following:
    1. Add the instances to the dataset folder.
    1. Add an index.json file to the dataset folder ([see Index File](#index-file)).
    1. (Optional) Add a solutions.json file to the dataset folder ([see Solutions File](#solutions-file)).
1. Create an "experiments" directory in your project root directory.
    1. Define experiment files ([see Experiment File](#experiment-file)).
1. Create an "output" directory where all the output files will be stored.
1. Open the config.json file, change the paths (relative to where the runner.py file is):
    1. Set the _cmakelists\_dir_ value to the path where the project's main CMakeLists.txt is.
    2. Set the _instances\_dir_ value to the instances directory.
    3. Set the _output\_dir_ value to the output directory.
1. Everything is configured now. To run an experiment file execute:

```python3 runner.py <experiment_file1.json> [<experiment_file_2.json> ...]```

## Example
An example project can be found in the folder [example](example) where the knapsack problem is solved and executed on a set of instances.

The example has two implementations: knapsack_dp (*dynamic programming*) and knapsack_bt (*backtracking*). It has a dataset with two kinds of instances tagged "easy" and "hard". There are two experiment files, each one executes all implementations but each one on a different kind of instances.

To run the example:
```
python3 runner.py example/experiments/experiment_easy.json example/experiments/example_experiments_hard.json 
```
The output will be stored in the **example/output/** folder.

## Runner Overview
Runner will execute experiment files and for each one will generate an output file with the format "<date>-<experiment-file-name>.json" in the output directory.
The following pseudocode shows the main workflow:
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

## Instances Directory
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
### Index File
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

### Solutions File
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

### Instance File
Each instance file must be a JSON object with the instance information.
Example (knapsack instance):
```javascript
{"capacity":200, "item_count":5, "item_weights":[50,70,30,80,90], "item_values":[5,20,3,10,50]}
```

## Compiling
Runner compiles the project before executing any experiment. In order to do so, it uses the CMakeLists.txt file located in the folder specified in the config.json file. Depending on whether valgrind is present or not it compiles the project in Release or Debug mode into the obj/release or obj/debug folders.

## Experiment File
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
The output file contains a JSON object with information about the experiment executions. Each time an executable file gets executed it returns 3 values: exit_code, stdout, stderr.
The JSON object that results from an execution includes the following attributes:
- date: date when the experiment was started.
- time: total time (secs) spent running the experiments from the experimentation file.
- experiment_file: name of the experiment file executed.
- outputs: JSON array of Object with the following attributes for each experiment execution on an instance
  - experiment_name: name attribute of the current experiment.
  - dataset_name: name of the dataset being used.
  - instance_name: name of the instance being used.
  - exit_code: the execution exit_code (0 means successful).
  - stderr: string of the STDERR output of the execution.
  - stdout: JSON object returned in the STDOUT output of the execution.
  - time: execution time of the experiment process (in seconds).

> Note: if the output in the STDOUT is not a JSON object, then the *stdout* attribute will contain the raw string.

Example:
```javascript
{
  "date": "10-05-2019",
  "experiment_file": "experiment_easy",
  "outputs": [
    { "experiment_name": "preprocessing", "dataset_name": "dataset1", "instance_name": "easy_instance", exit_code: 0, stderr: "PREPROCESSING\nRUNNING\nFINISHED", stdout:{"solution": [0, 1, 3, 5], "value": 500}, "time": 500.12},
    { "experiment_name": "preprocessing", "dataset_name": "dataset2", "instance_name": "hard_instance", exit_code: 6, stderr: "PREPROCESSING\nRUNNING", stdout:"", "time": 200.0}
  ]
}
```

## Authors
* Gonzalo Lera-Romero

## License
This project is licensed under the MIT License - see the LICENSE.md file for details
