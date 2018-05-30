#!/bin/bash

if [[ $# -ne 1 ]]; then
  echo "Wrong number of arguments.
Description:
  Run a travis default_build
Usage:
  default_build.sh BDM_PROJECT_DIR
Arguments:
  BDM_PROJECT_DIR path to the biodynamo project directory
  "
  exit 1
fi

BDM_PROJECT_DIR=$1

set -e -x

echo ${TRAVIS_OS_NAME}

# git describe does not work if last commit tag is not checked out
git fetch --unshallow &>/dev/null || true

# add master branch
# https://github.com/travis-ci/travis-ci/issues/6069
git remote set-branches --add origin master

$BDM_PROJECT_DIR/install.sh . << EOF
y
EOF

# reload shell and source biodynamo
set +e +x
. $BDM_PROJECT_DIR/cmake/installation/common/util.sh
. $(BashrcFile)
$use_biodynamo
set -e -x

# print lines-of-code statistic
cloc --exclude-dir=build .

# output compiler information
echo ${CXX}
${CXX} --version || true
${CXX} -v || true

cd build
make check-submission
