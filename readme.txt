problems to fix

1. fucking slow (optimization needed)
2. recommendations are absurd (need penalty system)

problem fixed

1. score, field corruption --> fixing AddBlockToField and DeleteLine solved it
2. block doesn't go all the way down --> fixing field corruption fixed it
3. doesn't recommend properly --> field was char type, recfield was int type (fixed to char)

lessons learned

1. don't hard code global variables into functions (use function parameters!)
2. watch out for data types (especially int and char!)
