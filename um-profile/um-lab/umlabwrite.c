#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "fmt.h"
#include "seq.h"

extern void Um_write_sequence(FILE *output, Seq_T instructions);

extern void build_halt_test(Seq_T instructions);
extern void build_halt_twice_test(Seq_T stream);
extern void build_verbose_halt_test(Seq_T instructions);
extern void build_add_test(Seq_T stream);
extern void build_add_output_test(Seq_T stream);
extern void build_map_test(Seq_T stream);
extern void build_load_value_test(Seq_T stream);
extern void build_input_test(Seq_T stream);
extern void build_multiplication_test(Seq_T stream);
extern void build_division_test(Seq_T stream);
extern void build_division_by_zero_test(Seq_T stream); // expect error
extern void build_nand_test(Seq_T stream);
extern void build_true_conditional_move_test(Seq_T stream);
extern void build_false_conditional_move_test(Seq_T stream);
extern void build_segment_load_test(Seq_T stream);
extern void build_segment_store_test(Seq_T stream);
extern void build_map_segment_test(Seq_T stream);
extern void build_unmap_segment_test(Seq_T stream);
extern void build_load_program_test(Seq_T stream);
extern void build_combined(Seq_T stream);
extern void build_output_fail_test(Seq_T stream);
extern void build_out_of_bounds_prog_count_test(Seq_T stream);
extern void build_invalid_instr_at_prog_count_test(Seq_T stream);
extern void build_seg_load_unmapped(Seq_T stream);
extern void build_seg_store_unmapped(Seq_T stream);
extern void build_seg_load_out_of_bounds(Seq_T stream);
extern void build_seg_store_out_of_bounds(Seq_T stream);
extern void build_unmap_seg_zero(Seq_T stream);
extern void build_unmap_unmapped_seg(Seq_T stream);
extern void build_load_prog_from_not_mapped(Seq_T stream);
extern void build_load_prog_from_unmapped(Seq_T stream);
extern void build_exec_500k(Seq_T stream);

/* The array `tests` contains all unit tests for the lab. */

static struct test_info {
        const char *name;
        const char *test_input;          /* NULL means no input needed */
        const char *expected_output;
        /* writes instructions into sequence */
        void (*build_test)(Seq_T stream);
} tests[] = {
        { "halt",         NULL, "",       build_halt_test },
        { "halt-verbose", NULL, "", build_verbose_halt_test },
        { "add",          NULL, "d",      build_add_test },
        { "add-output",    NULL, "6",     build_add_output_test },
        { "load-val",     NULL, "6",      build_load_value_test},
        { "input",        "5", "5",       build_input_test},
        { "mult",         NULL, "3",      build_multiplication_test},
        { "div",          NULL, "2",      build_division_test},
        // { "FFdiv0",         NULL, "",       build_division_by_zero_test},
        { "nand",         "7", "7",      build_nand_test},
        { "tcmov",        NULL, "5",      build_true_conditional_move_test},
        { "fcmov",        NULL, "7",      build_false_conditional_move_test},
        { "seg-load",     NULL, "",       build_segment_load_test},
        { "seg-store",    NULL, "2",      build_segment_store_test},
        { "map",          NULL, "",       build_map_segment_test},
        { "unmap",        NULL, "2",       build_unmap_segment_test},
        { "loadp",        NULL, "",       build_load_program_test},
        // { "FFoutput-fail",  NULL, "",       build_output_fail_test},
        // { "FFout-of-bounds-prog-count",  NULL, "", build_out_of_bounds_prog_count_test },
        // { "FFinvalid-instr-at-prog-count",  NULL, "", build_invalid_instr_at_prog_count_test },
        // { "FFseg-load-unmapped",  NULL, "", build_seg_load_unmapped },
        // { "FFseg-store-unmapped",  NULL, "", build_seg_store_unmapped },
        // { "FFseg-load-out-of-bounds",  NULL, "", build_seg_load_out_of_bounds },
        // { "FFseg-store-out-of-bounds",  NULL, "", build_seg_store_out_of_bounds },
        // { "FFunmap-seg-zero",  NULL, "", build_unmap_seg_zero },
        // { "FFunmap-unmapped",  NULL, "", build_unmap_unmapped_seg },
        // { "FFload-prog-from-not-mapped",  NULL, "", build_load_prog_from_not_mapped },
        // { "FFload-prog-from-unmapped",  NULL, "", build_load_prog_from_unmapped },
        { "halt-twice", NULL, "", build_halt_twice_test },
        { "500k-instr", NULL, "", build_exec_500k }
};

  
#define NTESTS (sizeof(tests)/sizeof(tests[0]))

/*
 * open file 'path' for writing, then free the pathname;
 * if anything fails, checked runtime error
 */
static FILE *open_and_free_pathname(char *path);

/*
 * if contents is NULL or empty, remove the given 'path', 
 * otherwise write 'contents' into 'path'.  Either way, free 'path'.
 */
static void write_or_remove_file(char *path, const char *contents);

static void write_test_files(struct test_info *test);


int main (int argc, char *argv[])
{
        bool failed = false;
        if (argc == 1)
                for (unsigned i = 0; i < NTESTS; i++) {
                        printf("***** Writing test '%s'.\n", tests[i].name);
                        write_test_files(&tests[i]);
                }
        else
                for (int j = 1; j < argc; j++) {
                        bool tested = false;
                        for (unsigned i = 0; i < NTESTS; i++)
                                if (!strcmp(tests[i].name, argv[j])) {
                                        tested = true;
                                        write_test_files(&tests[i]);
                                }
                        if (!tested) {
                                failed = true;
                                fprintf(stderr,
                                        "***** No test named %s *****\n",
                                        argv[j]);
                        }
                }
        return failed; /* failed nonzero == exit nonzero == failure */
}


static void write_test_files(struct test_info *test)
{
        FILE *binary = open_and_free_pathname(Fmt_string("%s.um", test->name));
        Seq_T instructions = Seq_new(0);
        test->build_test(instructions);
        Um_write_sequence(binary, instructions);
        Seq_free(&instructions);
        fclose(binary);

        write_or_remove_file(Fmt_string("%s.0", test->name),
                             test->test_input);
        write_or_remove_file(Fmt_string("%s.1", test->name),
                             test->expected_output);
}


static void write_or_remove_file(char *path, const char *contents)
{
        if (contents == NULL || *contents == '\0') {
                remove(path);
        } else {
                FILE *input = fopen(path, "wb");
                assert(input != NULL);

                fputs(contents, input);
                fclose(input);
        }
        free(path);
}


static FILE *open_and_free_pathname(char *path)
{
        FILE *fp = fopen(path, "wb");
        assert(fp != NULL);

        free(path);
        return fp;
}
