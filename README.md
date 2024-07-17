# About ProteinCentral
This project works with structures of protein-protein complexes and computes various centrality measures (computed for graphs defined by distances between atoms (or subsequently created based on Voronoi tessellation)), which can then be used to predict important (hot-spot) parts in multimeric protein structures from the SKEMPI 2.0 database
The main questions to be answered during the project are the following ones:

+Do tessellation graph-based centralities differ significantly from distance graph-based centralities?
+Are tessellation graph-based centralities more useful for predicting protein structural hot-spots?

The project consists of several parts and includes several existing tools such as the voronota application(version 1.29.4198) and the igraph data library(version 0.10.13)

# Installation of supporting resources
## Voronota

Below are several commands that install the latest version of Voronota and its expansions for the command line use in Unix-like systems.

```bash
# download the latest package
wget https://github.com/kliment-olechnovic/voronota/releases/download/v1.29.4198/voronota_1.29.4198.tar.gz

# unpack the package
tar -xf ./voronota_1.29.4198.tar.gz

# change to the package directory
cd ./voronota_1.29.4198

# run CMake
cmake . -DEXPANSION_JS=ON -DEXPANSION_LT=ON

# compile everything
make

# install everything
sudo make install
```

The installed files are listed in the 'install_manifest.txt' file.
They can be uninstalled by running

```bash
sudo xargs rm < ./install_manifest.txt
```

## Igraph
Igraph uses a CMake-based build system. To compile it, enter the directory where the igraph sources are:

```bash
cd igraph

# Create a new directory. This is where igraph will be built:

mkdir build
cd build

# Run CMake, which will automatically configure igraph, and report the configuration:

cmake ..
```
To set a non-default installation location, such as /opt/local, use:

```bash
cmake .. -DCMAKE_INSTALL_PREFIX=/opt/local
```
Check the output carefully, and ensure that all features you need are enabled. If CMake could not find certain libraries, some features such as GraphML support may have been automatically disabled.

To adjust the configuration, run:
```bash 
ccmake . 
```

Simply edit the CMakeCache.txt file. Some of the relevant options are listed below.

Once the configuration has been adjusted, once again,run 
```bash
cmake ..
```

Once igraph has been successfully configured, it can be built, tested and installed using:
```bash
cmake --build .
cmake --build . --target check
cmake --install .
```

# Project parts
### genetare_list_graph
This program creates a **graph** as a **_.tsv_ table** based on the data from the file produced by **voronota**, and writes the result to another **_.tsv_ file**. The program takes *two files* as arguments: the table where the working data comes from and the path to the file of the same format for writing the results
### calculate_centrality
This program processes the graph created by the **genetare_list_graph** program and uses the igraph library to compute several of its centrality measures: **Degree, Closeness, Betweenness, PageRank and Eigenvector**. The results are written to a separate **_.tsv_ file**. The program takes *three arguments*, each of which is a **_.tsv_ table**: the first contains information about the vertices of the graph, the second contains information about its edges, the third table will store the results of the program


# Building the project
You can build the ProteinCentral using CMake for makefile generation. In order for project to be built, you must first build each part of it
### Building generate_list_graph part
Run the following commands:

    cd tools/generate_list_graph/build/
    make
    cd ../../../

### Building calculate_centrality part
To build this part of the project, the commands below should be entered:

    cd tools/calculate_centrality/build/
    make
    cd ../../../

Notice that **igraph** already installed, should be placed in the same directory as the calculate_centrality program
# Running the project
After the project is build, we can work with it. There are several ways to do it
## Running by hand
To run the project by hand, at first it is needed to recieve a data using voronota tool.
Use a command below inserting your data in spaces

    ./tools/voronota_1.29.4198/voronota-js-lt-graph --input *input* --output-dir "*output*"

Here, **input** is a path to your input PDB file (e.g. ./PDBs/1A22.pdb), and **output** is a path to your output file (e.g. ./output)

Now, working with **generate_list_graph**, there is a command below:

    ./tools/generate_list_graph/build/generate_list_graph *input* *output*

Here, **input** is a path to a file from where the working data will be taken from, and **output** is a path to a file where the output of the program will be stored

After that we need to run the **calculate_centrality** part, to do so run a following command:

    ./tools/calculate_centrality/build/calculate_centrality *vertices_file* *edges_file* *output*

Here, **vertices_file** is a path to a file from where the data about graph's vertices will be taken, **edges_file** is a path to a file from where the data about graph's edges will be taken, and **output** is a path to a file where the output of the program will be stored

*to be continued...*

## Running using scripts
To facilitate the use of the project, several scripts are already created in the folder **scripts**:

1.**run_project.bash** runs through all parts of the project and generates a folder where the project results will be saved. The script requires arguments and is called with the command:
    bash scripts/run_project.bash *PDB ID* *output_folder*
Here, **PDB ID** is a PDB ID of an element(no more than 4 symbols), and **output_folder** is a path to a folder where the results of a script will be stored
2.**multiple_generate_list_graph** cycles through all files named *atom_graph_nodes.tsv* in the specified directory and runs the **generate_list_graph** function for each of them and saves the results in the output folder, so the program results are created for all PDBs at once. It doesn't require any arguments so to run it, simply use the following command:
    bash scripts/multiple_generate_list_graph.bash
3.**multiple_calculate_centrality** cycles through all files named *residue_graph_nodes.tsv* in the specified directory as well as through all files *dist_based_residue_graph_links.tsv* (created using the **generate_list_graph** program), runs the **calculate_centrality** function for each of them and saves the results in the output folder, thus the results of the program are created for all PDBs at once. It doesn't require any arguments so to run it, simply use the following command:
    bash scripts/multiple_generate_list_graph.bash

*to be continued...*