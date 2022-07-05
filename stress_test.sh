####################################
#
# @file stress_test.sh
# @author cfq (you@domain.com)
# @brief 
# @version 0.1
# @date 2022-07-02
# 
# @copyright Copyright (c) 2022
# 
######################################

#!/bin/bash
g++ ./stress_test/stress_test.cpp -o ./bin/stress --std=c++11 -pthread
./bin/stress
