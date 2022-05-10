v3.4
problems to fix

1. segfault occurs occasionally (when score reaches around 10000?)
2. pruning sometimes doesn't work (when changing VISIBLE_BLOCKS or TREE_PRUNING)

problem fixed

1. uses lot of memory --> used tree pruning
2. pruned recommendations aren't great --> calculate holes to lessen weird recommendations

lessons learned

1. add -g option in gcc to run gdb
