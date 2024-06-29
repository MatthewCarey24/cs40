#!/bin/sh

testFiles="/h/mtropp01/cs40/um"

for testFile in "$testFiles"/* ; do

    if [ -f "$testFile" ] && [ "${testFile##*.}" = "um" ]; then
        testName=$(echo "$testFile" | sed -E 's/(.*).um/\1/')
        if [ -f "$testName.1" ] && [ "${testFile##*.}" = "um" ] && [ -f "$testName.0" ]; then
            ./um "$testFile" < "$testName.0" >> "$testName.out"
        elif [ -f "$testName.1" ]; then
            ./um "$testFile" >> "$testName.out"
            
            if [ $? -eq 0 ]; then
                echo "diff results for $(basename "$testName"): "
                diff "$testName.1" "$testName.out"
                
                # Clean up output files if needed
                rm "$testName.out"
            else
                echo "Error running ./um for $(basename "$testFile")"
            fi
        else 
            echo "$(basename "$testFile") has no output!"
            ./um "$testFile" >> "$testName.out"
            if [ $? -ne 0 ]; then
                echo "Error running ./um for $(basename "$testFile")"
                rm "$testName.out"
            fi
        fi
    fi
done
