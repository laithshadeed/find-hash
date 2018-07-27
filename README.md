# Hash Finder

The problem is you have to find a phrase that satisfy the following conditions:
 - Its anagram is "poultry outwits ants"
 - Its md5 can be "e4820b45d2277f3844eac66c903e84be" or "23170acc097c24edb98fc5488ab033fe" or "665e5bcb0c20062fe8abaaf4628bb154"

The complexity is in writing an algorithm that is computable in feasible time.

### Non-computable solution
My initial thought in [generate-anagrams.rb](generate-anagrams.rb) is to generate all possible
anagrams for "poultry outwits ants" then hash them. It turns out this is not computable.
To generate all permutation for the known anagram it will require roughly factorial of the anagram length
= 20! = 2_432_902_008_176_640_000 (Accurate calculation should divide by the product of factorial of
duplicates).

## Computable solution
Then I thought to approach it in opposite direction. I will first get a list of all possible words
that could contribute to a potential anagram. We have 99175 in wordlist, the permutation of three
words of such list is huge: 99175 * 99175 * 99175 = 975_453_625_984_375. By applying specific
conditions documented in [find-hash.rb](find-hash) I managed to reduce the list to 1659 which has
4_566_034_179 permutations. It is still large but on DigitalOcean 24 cores it took 5 min to
compute:

```
root@ubuntu-s-24vcpu-128gb-ams3-01:~# time ./process-wordlist.rb wordlist
Total number of iteration: 4,566,034,179
You got 24 cores
printout stout yawls e4820b45d2277f3844eac66c903e84be (112.780418068s)
ty outlaws printouts 23170acc097c24edb98fc5488ab033fe (265.423688567s)

real    5m0.843s
user    119m37.377s
sys     0m0.799s
```
