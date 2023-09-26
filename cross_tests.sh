make clean > /dev/null
make > /dev/null

any_failed=0

mkdir -p cross_tests
run_dir=$(mktemp -d cross_tests/XXX)

for dir in $(ls tests); do
    directives=$(cat "tests/${dir}/directive")

    for test in $(find tests/${dir} -type f -name *.test); do
        # create temporary files for logs
        testname=$(basename "$test" .test)
        test_out_dir="${run_dir}/${test%.*}"
        mkdir -p "$test_out_dir"
        program_out="${test_out_dir}/output"
        diff_out="${test_out_dir}/diff"
        valgrind_log="${test_out_dir}/valgrind_log"

        # run program on specific test, redirecting valgrind output and program output
        valgrind -s --leak-check=full --show-leak-kinds=all --suppressions=glib.supp --log-file="$valgrind_log" "./bin/compiler.out" $test $directives &> $program_out

        check="${test%.*}.check"
        # Check if program out is accordingly to expected
        sdiff -l "$check" "$program_out" | cat -n | grep -v -e '($' > "$diff_out"

        [ -s $diff_out ]
        passed_out_test=$?

        if  grep -q "definitely lost: 0 bytes in 0 blocks" "$valgrind_log" &&\
            grep -q "indirectly lost: 0 bytes in 0 blocks" "$valgrind_log" &&\
            grep -q "possibly lost: 0 bytes in 0 blocks" "$valgrind_log" &&\
            # Glib "g_hash_table_new_full" reachable blocks
            # grep -q "still reachable: 18,804 bytes in 9 blocks" "$valgrind_log" &&\
            grep -q "ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)" "$valgrind_log"; then
            passed_mem_test=1
        else
            passed_mem_test=0
        fi

        if (( $passed_out_test == 1 && $passed_mem_test == 1)); then
            # remove temporary files
            rm -r "$test_out_dir"
        else
            any_failed=1
            if (( $passed_out_test == 0 )); then
                echo "Test $test failed expected output. Logs are in ${test_out_dir}"
            else
                rm "$diff_out"
                rm "$program_out"
            fi
            if (( $passed_mem_test == 0 )); then
                echo "Test $test failed memory sanity. Logs are in ${test_out_dir}"
            else
                rm "$valgrind_log"
            fi
        fi
    done
done

find cross_tests/ -empty -type d -delete

if (( $any_failed == 0)); then
    make clean > /dev/null
fi
