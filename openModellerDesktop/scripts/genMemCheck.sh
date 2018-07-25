#!/bin/bash

valgrind --tool=memcheck --demangle=yes --log-file-exactly=openModellerDesktop-memcheck.log --time-stamp=yes --trace-children=no --leak-check=summary openModellerDesktop

