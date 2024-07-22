#!/bin/bash

cd "$(dirname $0)/.."

rm -rf ./tests

{
cat << 'EOF'
1AO7
2REX
4MNQ
EOF
} | xargs -L 1 -P 8 ./scripts/get_tables_from_pdb.bash tests

git status ./tests
