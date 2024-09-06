# About ProteinCentral
This project works with structures of protein-protein complexes and computes various centrality measures (computed for graphs defined either by distances between atoms or created based on Voronoi tessellation), which can then be used to predict important (hot-spot) parts in multimeric protein structures from the SKEMPI 2.0 database(https://life.bsc.es/pid/skempi2)
The main questions to be answered during the project are the following ones:

+ Do tessellation graph-based centralities differ significantly from distance graph-based centralities?
+ Are tessellation graph-based centralities more useful for predicting protein structural hot-spots?

This project relates to an existing study of Guillaume Brysbaert and Marc F. Lensink (https://www.ncbi.nlm.nih.gov/pmc/articles/PMC9581030/#s8), but advances the idea of this study with a different approach to graph creation.

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
This program processes the graph created by the **genetare_list_graph** program and uses the igraph library to compute several of its centrality measures: **Degree, Closeness, Betweenness, PageRank, Eigenvector and Average Edge Betweenness(several measures)**. The results are written to a separate **_.tsv_ file**. The program takes *three arguments*, each of which is a **_.tsv_ table**: the first contains information about the vertices of the graph, the second contains information about its edges, the third table will store the results of the program
### about Edge Betweenness measures
Calculations using edge betweenness were used in the project. There are 4 of them in total:
+ **s_av_Edge_Betweenness**(s - simple): in the calculation for each vertex of the graph the arithmetic mean of edge betweenness values for all edges of this vertex was used
+ **i_av_Edge_Betweenness**(i - indicator): weighted average of edge betweenness values for all edges of a given vertex was used in calculation for each vertex of the graph. The weights in this case are ones and zeros: if an edge unites two vertices of the same chain of residues - this edge is not taken into account in the calculation (its weight is 0), otherwise the edge gets weight 1
+ **w_av_Edge_Betweenness**(w - weight): weighted average of edge betweenness values for all edges of this vertex was used in calculation for each vertex of the graph. Weights in this case are weights of edges (in weighted graphs) or ones (in unweighted graphs)
+ **b_av_Edge_Betweenness**(b - both): in the calculation for each vertex of the graph the weighted average of edge betweenness values for all edges of this vertex was used. Weights in this case are products of weights of i_av_Edge_Betweenness and w_av_Edge_Betweenness metrics used in the calculation


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

## Running using scripts
To facilitate the use of the project, several scripts are already created in the folder **scripts**:

- **process_pdb_id.bash** runs through all parts of the project and generates a folder where the project results will be saved. During the running, script applies other several r scripts placed in **scripts** folder. The script requires arguments and is called with the command:

    ```bash
    `bash scripts/process_pdb_id.bash *PDB ID* *output_folder*
    ```
    Here, **PDB ID** is a PDB ID of an element(no more than 4 symbols), and **output_folder** is a path to a folder where the results of a script will be stored

- **generate_data_to_analyse.bash** is used to run the generation of results for all files, then merges all final_table.tsv into one table. The script requires no arguments to operate, so it can be run with the command:

    ```bash
    `bash scripts/generate_data_to_analyse.bash
    ``` 

# Results content
In the folder where you decide to save the results of the **generate_data_to_analyse.bash** script there will be folders with the corresponding PDB IDs in the name. Each of these folders will contain 12 tsv tables: 
- **atom_graph_nodes, atom_graph_links, residue_graph_nodes, residue_graph_links** - the files generated by voronota contain information about the connections of atoms in the residue and the connections of residues to each other.
- **dist_based_residue_graph_links** - the file generated by generate_list_graph contains information about residue connections between each other, with calculations performed in a different way than voronota's calculations.
- **sep1_dist_based_centrality_measures, sep1_tes_based_centrality_measures, sep2_dist_based_centrality_measures, sep2_tes_based_centrality_measures** - several files generated by calculate_centrality program, each contains information about centrality of each vertex, calculated in several ways (**Degree, Closeness, Betweenness, PageRank, Eigenvector**). The difference between the files is the presence of a separator, which affects the edges of the created graph (_sep2_ implies a smaller number of edges, as a consequence - other centrality results), as well as the choice of the value by which the graph will be created (the prefix *dist_* in the file name implies the use of the distance between residues in the calculation of the graph weights, while the prefix *tes_* indicates the presence of two approaches: using in calculation of the graph weights firstly the *distance* between residues, and then a *contact area* between residues).
- **merged_centrality_measures** - a file merging two files generated by calculate_centrality with the same separator.
- **z_scores_merged_centrality_measures** - a file where centrality columns contain z-scores of corresponding vertices instead of centrality values, i.e. deviations of the corresponding centrality index from the mean value.
- **final_table** - table combining information from *z-scores_merged_centrality* and corresponding data from **SKEMPI v2.0**.

# Analysis of results
Most of ProteinCentral is taken up by analysing the results using R scripts. all final_table files are first concatenated into one table(**concatenated_final_table.tsv**) using the **generate_final_table.r** script. Then the table is *filtered* so that the final version of it contains one possible mutation with the highest *ddG threshold*, then for each selected ddG threshold a folder with confusion matrices is created and several metrics are calculated(using the script **generate_confusion_matrices.r**). Thresholds for this work were chosen similar to the work of *Brysbaert* and *Lensink*, namely **0.592**, **1.184** and **2**. Further, in the script **generate_PR_curves.r** confusion matrices are processed and PR(Precision-Recall) curves for each method of centrality calculation are constructed using their data, combining files of one method with different thresholds into one pdf file

Also script **generate_auc_table.bash** is created to output in a separate file the results of **AUC_PR**(Area Under the Precision-Recall Curve) metric for each graph construction method and each centrality calculation method. So, the file **AUC_PR_results.tsv** contains the table of project results. The table is sorted by descending AUC_PR value and shows how *suitable* the graph construction and centrality calculation methods are for use in calculating hot-spots in a protein complex(1 - the metric is ideal for use in practice, 0 - the metric is not suitable for hot-spot calculation)