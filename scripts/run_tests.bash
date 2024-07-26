#!/bin/bash

cd "$(dirname $0)/.."

rm -rf ./tests

{
cat << 'EOF'
1AO7
2REX
4MNQ
EOF
} | xargs -L 1 -P 8 ./scripts/process_pdb_id.bash tests

git status ./tests
