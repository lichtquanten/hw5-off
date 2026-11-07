#!/bin/bash
#set -e
rm -f grading.out
now=`date`
echo "Grading script started at: $now" > grading.out
if [ ! -d "grading/hw5.submission" ]; then
    FB="Submission directory doesn't exist. Did you submit your SHA?"
    curl --retry 3 -s "$CODIO_AUTOGRADE_V2_URL" -d grade=0 -d extra_credit=0 -d format=txt -d feedback="$FB"
	echo "FB" >> grading.out
    exit 0
fi
cd .guides/secure/
rm -f hw5.sh
rm -f datecomp.py
rm -f extension.py
rm -f due.py
URL="https://bytes.usc.edu/files/cs104/sp25/grading/hw5.sh"
wget --no-check-certificate -q -O hw5.sh $URL
URL2="https://bytes.usc.edu/files/cs104/sp25/grading/datecomp.py"
wget --no-check-certificate -q -O datecomp.py $URL2
URL3="https://bytes.usc.edu/files/cs104/sp25/grading/extension.py"
wget --no-check-certificate -q -O extension.py $URL3
URL4="https://bytes.usc.edu/files/cs104/sp25/grading/due.py"
wget --no-check-certificate -q -O due.py $URL4
URL5="https://bytes.usc.edu/files/cs104/sp25/grading/wordle.tests"
wget --no-check-certificate -q -O wordle.tests $URL5
URL6="https://bytes.usc.edu/files/cs104/sp25/grading/schedwork.tests"
wget --no-check-certificate -q -O schedwork.tests $URL6
cd /home/codio/workspace/
OUT=`timeout 170 bash .guides/secure/hw5.sh`
EXIT_STATUS=$?
MSG="timed grading exited with $EXIT_STATUS"
echo "$MSG"
echo "$MSG" >> grading.out
if [ $EXIT_STATUS -eq 124 ]
then
        FB="The grading script timed out. Please make sure your tests run in <180 seconds and does not have any tests that hang (infinite loop). grading.out might have more info."
        echo "$FB"
        echo "$FB" >> grading.out
        echo "$OUT" >> grading.out
        curl --retry 3 -s "$CODIO_AUTOGRADE_V2_URL" -d grade=0 -d extra_credit=0 -d format=txt -d feedback="$FB"
        exit 0
fi
if [ $EXIT_STATUS -ne 0 ]
then
        FB="The grading script didn't complete or crashed. Make sure your code can run all of the tests without crashing. Check grading.out for more info."
        echo "$FB"
        echo "$FB" >> grading.out
        echo "$OUT" >> grading.out
        curl --retry 3 -s "$CODIO_AUTOGRADE_V2_URL" -d grade=0 -d extra_credit=0 -d format=txt -d feedback="$FB"
        exit 0
fi
echo "$OUT"
LAST_LINE=$(echo "$OUT" | tail -n 1)
# Check if the last line matches the word:#:#:time:# format
if echo "$LAST_LINE" | grep -qE '[a-zA-Z_]+:[0-9]+.[0-9]+:[0-9]+:*:[0-9]'; then
    GRADE=`echo "$LAST_LINE"|awk -F ":" '{print $2}'`
    EC=`echo "$LAST_LINE"|awk -F ":" '{print $3}'`
    SUBMIT_UTC=`echo "$OUT"|tail -n 1|awk -F ":" '{print $4}'|tr ';' ':'`
    PENALTY=`echo "$OUT"|tail -n 1|awk -F ":" '{print $5}'`
    echo $GRADE
    echo $EC
    echo $SUBMIT_UTC
    echo $PENALTY
    EXTENSION=`echo $CODIO_AUTOGRADE_ENV|python3 /home/codio/workspace/.guides/secure/extension.py`
    echo "Calling curl to submit grade..."
    if [ "$PENALTY" -eq "0" ] && [ "$EXTENSION" -eq "0" ]
    then
 	    curl --retry 3 -s "$CODIO_AUTOGRADE_V2_URL" -d grade=$GRADE -d extra_credit=$EC -d format=txt -d feedback="$OUT"
    else
  	    curl --retry 3 -s "$CODIO_AUTOGRADE_V2_URL" -d grade=$GRADE -d extra_credit=$EC -d format=txt -d feedback="$OUT" -d penalty="$PENALTY"
    fi
    echo "$OUT" >> grading.out
else
    echo "Grading script failed to return a grade."
	echo "Grading script failed to return a grade. See below for output." >> grading.out
    FB="Grading script failed to return a grade. See below for the output."
    curl --retry 3 -s "$CODIO_AUTOGRADE_V2_URL" -d grade=0 -d extra_credit=0 -d format=txt -d feedback="$FB\n$OUT"
    echo "$OUT" >> grading.out
fi
rm -f .guides/secure/hw5.sh
rm -f .guides/secure/datecomp.py
rm -f .guides/secure/extension.py
rm -f .guides/secure/due.py
rm -f .guides/secure/*.tests
echo "$OUT" >> grading.out
