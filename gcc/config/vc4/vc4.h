
/* Please keep the contents of this file in the same order as gccint.pdf. */

/* Nonzero if ELF.  Redefined by vc4/elf.h.  */
#define TARGET_ELF 0

/* Target CPU builtins.  */
#define TARGET_CPU_CPP_BUILTINS()		\
  do						\
    {						\
      builtin_define ("__vc4__");		\
      builtin_assert ("cpu=vc4");		\
      builtin_assert ("machine=vc4");		\
    }						\
  while (0)

#ifndef TARGET_DEFAULT
#define TARGET_DEFAULT (MASK_UNIX_ASM)
#endif

/* 17.5 Storage Layout */

/* Define this if most significant bit is lowest numbered
   in instructions that operate on numbered bit-fields.
   This is not true on the VC4.  */
#define BITS_BIG_ENDIAN 0

/* Define this if most significant byte of a word is the lowest numbered.  */
/* That is not true on the VC4.  */
#define BYTES_BIG_ENDIAN 0

/* Define this if most significant word of a multiword number is the lowest
   numbered.  */
/* This is not true on the VC4.  */
#define WORDS_BIG_ENDIAN 0

/* Width of a word, in units (bytes).  */
#define UNITS_PER_WORD 4

/* Allocation boundary (in *bits*) for storing arguments in argument list.  */
#define PARM_BOUNDARY 32

/* Let's keep the stack somewhat aligned.  */
#define STACK_BOUNDARY 32

/* Allocation boundary (in *bits*) for the code of a function.  */
#define FUNCTION_BOUNDARY 16

/* No data type wants to be aligned rounder than this.  */
#define BIGGEST_ALIGNMENT 32

/* Set this nonzero if move instructions will actually fail to work
   when given unaligned data.  */
#define STRICT_ALIGNMENT 1

/* 17.6 Layout of Source Language Data Types */

/* We only support float's (at least at the moment). */
#define DOUBLE_TYPE_SIZE 32
#define LONG_DOUBLE_TYPE_SIZE 32

/* Define this as 1 if `char' should by default be signed; else as 0.  */
#define DEFAULT_SIGNED_CHAR 1

/* 17.7 Register Usage */

/* 17.7.1 Basic Characteristics of Registers */

/* Number of actual hardware registers.
   The hardware registers are assigned numbers for the compiler
   from 0 to just below FIRST_PSEUDO_REGISTER.
   All registers that the compiler knows about must be given numbers,
   even those that are not normally considered general registers.  */
#define FIRST_PSEUDO_REGISTER 32

/* 1 for registers that have pervasive standard uses
   and are not available for the register allocator.
   On the VC4, these are the AP, FP, SP and PC.  */
#define FIXED_REGISTERS {0, 0, 0, 0, 0, 0, 0, 0, \
                         0, 0, 0, 0, 0, 0, 0, 0, \
                         0, 0, 0, 0, 0, 0, 0, 0, \
                         1, 1, 1, 1, 1, 1, 1, 1}

/* 1 for registers not available across function calls.
   These must include the FIXED_REGISTERS and also any
   registers that can be used without being saved.
   The latter must include the registers where values are returned
   and the register where structure-value addresses are passed.
   Aside from that, you can include as many other registers as you like.  */
#define CALL_USED_REGISTERS {1, 1, 1, 1, 1, 1, 0, 0, \
                             0, 0, 0, 0, 0, 0, 0, 0, \
                             0, 0, 0, 0, 0, 0, 0, 0, \
                             1, 1, 1, 1, 1, 1, 1, 1}

/* VC4 pc is overloaded on a register.  */
#define PC_REGNUM 31

/* 17.7.2 Order of Allocation of Registers */

/* 17.7.3 How Values Fit in Registers */

#define HARD_REGNO_NREGS(REGNO, MODE)            \
  ((GET_MODE_SIZE (MODE) + UNITS_PER_WORD - 1)   \
   / UNITS_PER_WORD)

/* Value is 1 if hard register REGNO can hold a value of machine-mode MODE.
   On the VC4, all registers can hold all modes.  */
#define HARD_REGNO_MODE_OK(REGNO, MODE) 1

/* Value is 1 if it is a good idea to tie two pseudo registers
   when one has mode MODE1 and one has mode MODE2.
   If HARD_REGNO_MODE_OK could produce different values for MODE1 and MODE2,
   for any hard reg, then this must be 0 for correct output.  */
#define MODES_TIEABLE_P(MODE1, MODE2)  1

/* 17.7.4 Handling Leaf Functions */

/* 17.7.5 Registers That Form a Stack */

/* 17.8 Register Classes */

enum reg_class { NO_REGS, LOW_REGS, ALL_REGS, LIM_REG_CLASSES };

/* HERE: Maybe this should be LOW_REGS? */
#define GENERAL_REGS ALL_REGS

#define N_REG_CLASSES (int) LIM_REG_CLASSES

#define REG_CLASS_NAMES	\
  { "NO_REGS", "LOW_REGS", "ALL_REGS" }

#define REG_CLASS_CONTENTS {{0}, {0xffff}, {0xffffffff}}

#define REGNO_REG_CLASS(REGNO) (((REGNO) < 16) ? LOW_REGS : ALL_REGS)

#define BASE_REG_CLASS ALL_REGS

#define INDEX_REG_CLASS ALL_REGS

#define REGNO_OK_FOR_BASE_P(regno)	\
  ((regno) < FIRST_PSEUDO_REGISTER || reg_renumber[regno] >= 0)

#define REGNO_OK_FOR_INDEX_P(regno)	\
  ((regno) < FIRST_PSEUDO_REGISTER || reg_renumber[regno] >= 0)

/* 17.9 Obsolete Macros for Defining Constraints */

/* 17.10.1 Basic Stack Layout */

/* Define this if pushing a word on the stack
   makes the stack pointer a smaller address.  */
#define STACK_GROWS_DOWNWARD

/* Define this to nonzero if the nominal address of the stack frame
   is at the high-address end of the local variables;
   that is, each additional local variable allocated
   goes at a more negative offset in the frame.  */
#define FRAME_GROWS_DOWNWARD 1

/* Offset within stack frame to start allocating local variables at.
   If FRAME_GROWS_DOWNWARD, this is the offset to the END of the
   first local allocated.  Otherwise, it is the offset to the BEGINNING
   of the first local allocated.  */
#define STARTING_FRAME_OFFSET 0

/* Offset of first parameter from the argument pointer register value.  */
#define FIRST_PARM_OFFSET(FNDECL) 4

/* Offset from the frame pointer register value to the top of stack.  */
#define FRAME_POINTER_CFA_OFFSET(FNDECL) 0

/* 17.10.2 Exception Handling Support */

/* 17.10.4 Registers That Address the Stack Frame */

/* Register to use for pushing function arguments.  */
#define STACK_POINTER_REGNUM VC4_SP_REGNUM

/* Base register for access to local variables of the function.  */
#define FRAME_POINTER_REGNUM VC4_R15_REGNUM

/* Base register for access to arguments of the function.  */
#define ARG_POINTER_REGNUM VC4_SP_REGNUM

/* Register in which static-chain is passed to a function.  */
/* TODO */
#define STATIC_CHAIN_REGNUM 0

/* 17.10.5 Eliminating Frame Pointer and Arg Pointer */

/* Store in the variable DEPTH the initial difference between the
   frame pointer reg contents and the stack pointer reg contents,
   as of the start of the function body.  This depends on the layout
   of the fixed parts of the stack frame and on how registers are saved.

   On the VC4, FRAME_POINTER_REQUIRED is always 1, so the definition of this
   macro doesn't matter.  But it must be defined.  */
#define INITIAL_FRAME_POINTER_OFFSET(DEPTH) ((DEPTH) = 0)

/* 17.10.6 Passing Function Arguments on the Stack */

/* 17.10.7 Passing Arguments in Registers */

#undef TARGET_FUNCTION_ARG
#define TARGET_FUNCTION_ARG vc4_function_arg

typedef struct {
  int nregs;
} CUMULATIVE_ARGS;

#define INIT_CUMULATIVE_ARGS(CUM, FNTYPE, LIBNAME, INDIRECT, N_NAMED_ARGS) \
 ((CUM).nregs = 0)

#undef TARGET_FUNCTION_ARG_ADVANCE
#define TARGET_FUNCTION_ARG_ADVANCE vc4_function_arg_advance

/* 1 if N is a possible register number for function argument passing.
   On the VC4, the first 6 registers are used in this way.  */
#define FUNCTION_ARG_REGNO_P(regno) IN_RANGE ((regno), 0, 5)

/* 17.10.8 How Scalar Function Values Are Returned */

/* 17.10.9 How Large Values Are Returned */
/* 17.10.10 Caller-Saves Register Allocation */
/* 17.10.11 Function Entry and Exit */
/* 17.10.12 Generating Code for Profiling */
/* 17.10.13 Permitting tail calls */
/* 17.10.14 Stack smashing protection */
/* 17.11 Implementing the Varargs Macros */
/* 17.12 Trampolines for Nested Functions */
/* 17.13 Implicit Calls to Library Routines */
/* 17.14 Addressing Modes */
/* 17.15 Anchored Addresses */
/* 17.16.1 Representation of condition codes using (cc0) */
/* 17.16.2 Representation of condition codes using registers */
/* 17.16.3 Macros to control conditional execution */

/* 17.17 Describing Relative Costs of Operations */

/* Nonzero if access to memory by bytes is slow and undesirable.  */
#define SLOW_BYTE_ACCESS 0

/* 17.18 Adjusting the Instruction Scheduler */

/* 17.19 Dividing the Output into Sections (Texts, Data, ...) */
/* 17.20 Position Independent Code */
/* 17.21.1 The Overall Framework of an Assembler File */
/* 17.21.2 Output of Data */
/* 17.21.3 Output of Uninitialized Variables */
/* 17.21.4 Output and Generation of Labels */
/* 17.21.7 Output of Assembler Instructions */
/* 17.31 Miscellaneous Parameters */

/* Max number of bytes we can move from memory to memory
   in one reasonably fast instruction.  */
#define MOVE_MAX 4

/* end */


/* Length in units of the trampoline for entering a nested function.  */
#define TRAMPOLINE_SIZE 15

/* Maximum number of registers that can appear in a valid memory address.  */
#define MAX_REGS_PER_ADDRESS 2

/* Value is 1 if truncating an integer of INPREC bits to OUTPREC bits
   is done just by pretending it is already truncated.  */
#define TRULY_NOOP_TRUNCATION(OUTPREC, INPREC) 1

/* Specify the machine mode that pointers have.
   After generation of rtl, the compiler makes no further distinction
   between pointers and any other objects of this machine mode.  */
#define Pmode SImode

/* A function address in a call instruction
   is a byte address (for indexing purposes)
   so give the MEM rtx a byte's mode.  */
#define FUNCTION_MODE QImode

#define FUNCTION_VALUE_REGNO_P(N) ((N) == 0)

/* Specify the cost of a branch insn; roughly the number of extra insns that
   should be added to avoid a branch.

   Branches are extremely cheap on the VC4 while the shift insns often
   used to replace branches can be expensive.  */

#define BRANCH_COST(speed_p, predictable_p) 0

/* Tell final.c how to eliminate redundant test instructions.  */

/* Here we define machine-dependent flags and fields in cc_status
   (see `conditions.h').  No extra ones are needed for the VC4.  */

#define OUTPUT_JUMP(NORMAL, FLOAT, NO_OV)	\
  { if (cc_status.flags & CC_NO_OVERFLOW)	\
      return NO_OV;				\
    return NORMAL;				\
  }

#define ASM_GENERATE_INTERNAL_LABEL(LABEL,PREFIX,NUM)	\
  sprintf (LABEL, "*%s%ld", PREFIX, (long)(NUM))

/* This is how to output an assembler line
   that says to advance the location counter
   to a multiple of 2**LOG bytes.  */

#define ASM_OUTPUT_ALIGN(FILE,LOG)  \
  fprintf (FILE, "\t.align %d\n", (LOG))

/* Specify the machine mode that this machine uses
   for the index in the tablejump instruction.  */
#define CASE_VECTOR_MODE HImode

/* Output to assembler file text saying following lines
   may contain character constants, extra white space, comments, etc.  */

#define ASM_APP_ON "#APP\n"

/* Output to assembler file text saying following lines
   no longer contain unusual constructs.  */

#define ASM_APP_OFF "#NO_APP\n"

/* Output before read-only data.  */

#define TEXT_SECTION_ASM_OP "\t.text"

/* Output before writable data.  */

#define DATA_SECTION_ASM_OP "\t.data"

/* How to refer to registers in assembler output.
   This sequence is indexed by compiler's hard-register-number (see above).
   The register names will be prefixed by REGISTER_PREFIX, if any.  */

#define REGISTER_PREFIX ""
#define REGISTER_NAMES                                                 \
  { "r0", "r1",  "r2",  "r3", "r4", "r5", "r6", "r7"                   \
    , "r8", "r9", "r10", "r11", "r12", "r13", "r14"                    \
      , "r15", "r16", "r17", "r18", "r19", "r20"                       \
      , "r21", "r22", "r23", "r24", "r25", "r26", "r27"                \
      , "r28", "r29", "r30", "pc"}

#define VC4_FUNCTION_PROFILER_NAME "mcount"
#define FUNCTION_PROFILER(FILE, LABELNO)			\
  do								\
    {								\
      char label[256];						\
      ASM_GENERATE_INTERNAL_LABEL (label, "LP", (LABELNO));	\
      fprintf (FILE, "\tmovab ");				\
      assemble_name (FILE, label);				\
      asm_fprintf (FILE, ",%Rr0\n\tjsb %s\n",			\
		   VC4_FUNCTION_PROFILER_NAME);			\
    }								\
  while (0)

/* This is how to output an assembler line
   that says to advance the location counter
   to a multiple of 2**LOG bytes.  */

#define ASM_OUTPUT_ALIGN(FILE,LOG)  \
  fprintf (FILE, "\t.align %d\n", (LOG))

/* This is how to output an assembler line
   that says to advance the location counter by SIZE bytes.  */

#define ASM_OUTPUT_SKIP(FILE,SIZE)  \
  fprintf (FILE, "\t.space %u\n", (int)(SIZE))

/* This says how to output an assembler line
   to define a global common symbol.  */

#define ASM_OUTPUT_COMMON(FILE, NAME, SIZE, ROUNDED)	\
  ( fputs (".comm ", (FILE)),				\
    assemble_name ((FILE), (NAME)),			\
    fprintf ((FILE), ",%u\n", (int)(ROUNDED)))

/* This says how to output an assembler line
   to define a local common symbol.  */

#define ASM_OUTPUT_LOCAL(FILE, NAME, SIZE, ROUNDED)	\
  ( fputs (".lcomm ", (FILE)),				\
    assemble_name ((FILE), (NAME)),			\
    fprintf ((FILE), ",%u\n", (int)(ROUNDED)))

// /* Control how constructors and destructors are emitted.  */
// #define TARGET_ASM_CONSTRUCTOR  vc4_asm_out_constructor
// #define TARGET_ASM_DESTRUCTOR   vc4_asm_out_destructor

#define TARGET_ASM_GLOBALIZE_LABEL pa_globalize_label
