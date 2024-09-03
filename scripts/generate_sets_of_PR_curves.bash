#!/bin/bash

rscript scripts/generate_PR_curves.R "sep1_dist_distance_weighted_"
rscript scripts/generate_PR_curves.R "sep1_dist_unweighted_"
rscript scripts/generate_PR_curves.R "sep1_tes_unweighted_"
rscript scripts/generate_PR_curves.R "sep1_tes_distance_weighted_"
rscript scripts/generate_PR_curves.R "sep1_tes_area_weighted_"
rscript scripts/generate_PR_curves.R "sep2_dist_distance_weighted_"
rscript scripts/generate_PR_curves.R "sep2_dist_unweighted_"
rscript scripts/generate_PR_curves.R "sep2_tes_unweighted_"
rscript scripts/generate_PR_curves.R "sep2_tes_distance_weighted_"
rscript scripts/generate_PR_curves.R "sep2_tes_area_weighted_"