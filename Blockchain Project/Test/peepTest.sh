#!/bin/bash

echo "Testing with input 2 1 a 4 (insert at position 1 letter a)...." 
RESULT=`./test5 < test5-insert`
echo "$RESULT"
echo "Expect result to have transactions of 1"

echo "Testing with input 2 1 a 4 (insert at position 1 letter a) and viewing the peep...." 
RESULT=`./test5 < test5-view`
echo "$RESULT"
echo "Expect result to be 1:a 2:c with transaction num of 1"

echo "Testing with multiple inserts and deletions"
RESULT=`./test5 < test5-multiple`
echo "$RESULT"
echo "Expect result to be 1:r   2:r   3:e with transaction number of 7"


echo "Testing with multiple inserts and deletions then viewing"
RESULT=`./test5 < test5-multipleView`
echo "$RESULT"
echo "Expect result to be 1:r   2:e   with transaction number of 8"

echo "Testing with more deletions than insertions"
RESULT=`./test5 < test5-deletion`
echo "$RESULT"
echo "Expect result to be invalid in with transaction number of 2"

echo "Testing with many insertions"
RESULT=`./test5 < test5-manyInserts`
echo "$RESULT"
echo "Expect result to be 1:q   2:e   3:d   4:f   5:h   6:n   7:c   8:l   9:p   10:q   11:e   12:r   13:t   14:y   15:u   16:q   17:i   18:o   with transaction number of 18"

echo "Testing with more deletions than insertions but viewing each time"
RESULT=`./test5 < test5-viewEach`
echo "$RESULT"
echo "Expect result to be 1:a   2:e   3:e   4:r   5:t   6:y   7:x with transaction number of 7"
