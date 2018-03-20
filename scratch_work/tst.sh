#!/bin/bash

cd build && cmake .. && make && ../../../dynamorio/build/bin64/drrun -c ./libshadow_stack_dr_client.so -only_from_app -- ./toy.out
