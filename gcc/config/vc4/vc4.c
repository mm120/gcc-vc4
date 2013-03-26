
#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "rtl.h"
#include "regs.h"
#include "hard-reg-set.h"
#include "insn-config.h"
#include "conditions.h"
#include "insn-flags.h"
#include "output.h"
#include "insn-attr.h"
#include "flags.h"
#include "recog.h"
#include "reload.h"
#include "diagnostic-core.h"
#include "obstack.h"
#include "tree.h"
#include "expr.h"
#include "optabs.h"
#include "except.h"
#include "function.h"
#include "ggc.h"
#include "target.h"
#include "target-def.h"
#include "tm_p.h"
#include "langhooks.h"
#include "df.h"
          
/* Initialize the GCC target structure.  */
          
static int
vc4_address_register_rtx_p (rtx x, int strict_p);
static bool
vc4_legitimate_address_p (enum machine_mode mode, rtx x, bool strict_p);

static void
vc4_globalize_label (FILE *stream, const char *name)
{
  /* /\* We only handle DATA objects here, functions are globalized in */
  /*    ASM_DECLARE_FUNCTION_NAME.  *\/ */
  /* if (! FUNCTION_NAME_P (name)) */
  /* { */
    fputs ("\t.EXPORT ", stream);
    assemble_name (stream, name);
    fputs (",DATA\n", stream);
  /* } */
}

static enum machine_mode
vc4_promote_function_mode(const_tree type ATTRIBUTE_UNUSED,
						  enum machine_mode mode,
						  int *punsignedp ATTRIBUTE_UNUSED,
						  const_tree funtype ATTRIBUTE_UNUSED,
						  int for_return ATTRIBUTE_UNUSED)
{
	if (GET_MODE_CLASS(mode) == MODE_INT &&
	    GET_MODE_SIZE (mode) < UNITS_PER_WORD)
	    return SImode;

	return mode;
}

static rtx
vc4_function_value(const_tree type, const_tree func,
				   bool outgoing ATTRIBUTE_UNUSED)
{
	enum machine_mode mode;
	int unsignedp ATTRIBUTE_UNUSED;

	mode = TYPE_MODE (type);

	/* Promote integer types.  */
	if (INTEGRAL_TYPE_P (type))
		mode = vc4_promote_function_mode(type, mode, &unsignedp, func, 1);

	return gen_rtx_REG (mode, VC4_R0_REGNUM);
}

static bool
vc4_function_value_regno_p(const unsigned int regno)
{
	return regno == VC4_R0_REGNUM;
}

static void
vc4_print_condition (FILE *stream)
{
  fputs("condition code", stream);
}

static void
vc4_print_operand (FILE *stream, rtx x, int code)
{
  switch(code)
  {
  /* An integer that we want to print in HEX.  */
  /* case 'x': */
  /*   switch (GET_CODE (x)) */
  /*   { */

  /*   default: */
  /*     output_operand_lossage ("Unsupported operand for code '%c'", code); */
  /*   } */
  /*   return; */
  case '?':
    vc4_print_condition (stream);
    return;
  default:
    if (x == 0)
    {
      
      output_operand_lossage ("missing operand code: %d", code);
      return;
    }

    switch(GET_CODE(x))
    {
    case CONST_INT:
      fprintf (stream, HOST_WIDE_INT_PRINT_HEX, INTVAL (x));
      break;

    case REG:
      fprintf (stream, "r%d", REGNO (x));
      break;

    case MEM:
      /* output_memory_reference_mode = GET_MODE (x); */
      output_address (XEXP (x, 0));
      break;

    case CONST_DOUBLE:
      fprintf (stream, "const_double");
      break;

    case SYMBOL_REF:
      fprintf (stream, "%s", XSTR (x, 0));
      break;

    default:
      fprintf(stream, "print_operand %d %d", code, GET_CODE(x));

      /* gcc_assert (GET_CODE (x) != NEG); */
      /* fputc ('#', stream); */
      /* if (GET_CODE (x) == HIGH) */
      /*   { */
      /*     fputs (":lower16:", stream); */
      /*     x = XEXP (x, 0); */
      /*   } */
      
      /* output_addr_const (stream, x); */
      break;
    }
  }
}

static void
vc4_print_operand_address (FILE *stream, rtx x)
{
  if(REG_P(x))
  {
    fprintf(stream, "(r%d)", REGNO(x));
  }
  else if(GET_CODE(x) == PLUS || GET_CODE(x) == PLUS)
  {
    int is_minus = GET_CODE (x) == MINUS;
    rtx base = XEXP (x, 0);
    rtx index = XEXP (x, 1);
    HOST_WIDE_INT offset = 0;

    if (!REG_P (base)
	/*|| (REG_P (index) && REGNO (index) == VC4_SP_REGNUM)*/)
      {
	/* Ensure that BASE is a register.  */
	/* (one of them must be).  */
	/* Also ensure the SP is not used as in index register.  */
	rtx temp = base;
	base = index;
	index = temp;
      }

    switch (GET_CODE (index))
      {
      case CONST_INT:
	offset = INTVAL (index);
	if (is_minus)
	  offset = -offset;
	asm_fprintf (stream, "#%wd(r%d)",
		     offset, REGNO (base));
	break;

      case REG:
	if (is_minus)
	  gcc_unreachable ();

	asm_fprintf (stream, "(r%d, r%d)",
		     REGNO (base), 
		     REGNO (index));
	break;

      case MULT:
      case ASHIFTRT:
      case LSHIFTRT:
      case ASHIFT:
      case ROTATERT:
	{
	  asm_fprintf (stream, "(r%d, %sr%d",
		       REGNO (base), is_minus ? "-" : "",
		       REGNO (XEXP (index, 0)));
	  vc4_print_operand (stream, index, 'S');
	  fputs (")", stream);
	  break;
	}

      default:
	gcc_unreachable ();
      }
  }
  else
    fputs("print_operand_address", stream);
}

static bool
vc4_print_operand_punct_valid_p (unsigned char code)
{
  return true;
}

/* Return nonzero if INDEX is valid for an address index operand in
   VC4 state.  */
static int
vc4_legitimate_index_p (enum machine_mode mode, rtx index, RTX_CODE outer,
			int strict_p)
{
  HOST_WIDE_INT range;
  enum rtx_code code = GET_CODE (index);

  if (vc4_address_register_rtx_p (index, strict_p)
      && (GET_MODE_SIZE (mode) <= 4))
    return 1;

  if (mode == DImode || mode == DFmode)
    {
      if (code == CONST_INT)
	{
	  HOST_WIDE_INT val = INTVAL (index);

	  return val > -4096 && val < 4092; /* TODO */
	}

      return 0;
    }

  /* For VC4 we may be doing a sign-extend operation during the
     load.  */
  if (mode == HImode
      /*|| mode == HFmode*/
      || (outer == SIGN_EXTEND && mode == QImode))
    range = 256;
  else
    range = 4096;

  return (code == CONST_INT
	  && INTVAL (index) < range
	  && INTVAL (index) > -range);
}

/* Return nonzero if X is valid as an VC4 state addressing register.  */
static int
vc4_address_register_rtx_p (rtx x, int strict_p)
{
  int regno;

  if (!REG_P (x))
    return 0;

  regno = REGNO (x);

  if (strict_p)
    return REGNO_OK_FOR_BASE_P (regno);

  return (regno <= VC4_LAST_REGNUM
	  || regno >= FIRST_PSEUDO_REGISTER
	  || regno == FRAME_POINTER_REGNUM
	  || regno == ARG_POINTER_REGNUM);
}

static bool
vc4_legitimate_address_p (enum machine_mode mode, rtx x, bool strict_p)
{
  enum rtx_code code = GET_CODE (x);
  RTX_CODE outer = SET;

  if (vc4_address_register_rtx_p (x, strict_p))
    return 1;

  if (code == POST_INC || code == PRE_DEC)
    return vc4_address_register_rtx_p (XEXP (x, 0), strict_p);

  /* After reload constants split into minipools will have addresses
     from a LABEL_REF.  */
  else if (reload_completed
	   && (code == LABEL_REF
	       || (code == CONST
		   && GET_CODE (XEXP (x, 0)) == PLUS
		   && GET_CODE (XEXP (XEXP (x, 0), 0)) == LABEL_REF
		   && CONST_INT_P (XEXP (XEXP (x, 0), 1)))))
    return 1;

 else if (code == PLUS)
    {
      rtx xop0 = XEXP (x, 0);
      rtx xop1 = XEXP (x, 1);

      return ((vc4_address_register_rtx_p (xop0, strict_p)
	       && ((CONST_INT_P (xop1)
		    && vc4_legitimate_index_p (mode, xop1, outer, strict_p))
		   || (!strict_p)))
	      || (vc4_address_register_rtx_p (xop1, strict_p)
		  && vc4_legitimate_index_p (mode, xop0, outer, strict_p)));
    }

  return 0;
}

static rtx
vc4_function_arg (cumulative_args_t pcum_v, enum machine_mode mode,
		  const_tree type, bool named)
{
  CUMULATIVE_ARGS *pcum = get_cumulative_args (pcum_v);
  if (pcum->nregs < 6)
    {
      int t = pcum->nregs;
      return gen_rtx_REG (mode, t);
    }
  return NULL_RTX;
}

/* Define how to find the value returned by a library function
   assuming the value has mode MODE.  */

static rtx
vc4_libcall_value (enum machine_mode mode, const_rtx libcall ATTRIBUTE_UNUSED)
{
    return gen_rtx_REG (mode, VC4_R0_REGNUM);
}

static void
vc4_function_arg_advance (cumulative_args_t cum_v, enum machine_mode mode,
			  const_tree type, bool named ATTRIBUTE_UNUSED)
{
  CUMULATIVE_ARGS *cum = get_cumulative_args (cum_v);

  cum->nregs += ((mode != BLKmode
		  ? (GET_MODE_SIZE (mode) + 3) & ~3
		  : (int_size_in_bytes (type) + 3) & ~3)) / 4;
}

/* Place some comments into the assembler stream
   describing the current function.  */
#if 0
static void
vc4_output_function_prologue (FILE *f, HOST_WIDE_INT frame_size)
{
  unsigned long func_type;

  /* ??? Do we want to print some of the below anyway?  */
  if (TARGET_THUMB1)
    return;

  /* Sanity check.  */
  /*gcc_assert (!arm_ccfsm_state && !arm_target_insn);*/

  func_type = arm_current_func_type ();

  switch ((int) ARM_FUNC_TYPE (func_type))
    {
    default:
    case ARM_FT_NORMAL:
      break;
    case ARM_FT_INTERWORKED:
      asm_fprintf (f, "\t%@ Function supports interworking.\n");
      break;
    case ARM_FT_ISR:
      asm_fprintf (f, "\t%@ Interrupt Service Routine.\n");
      break;
    case ARM_FT_FIQ:
      asm_fprintf (f, "\t%@ Fast Interrupt Service Routine.\n");
      break;
    case ARM_FT_EXCEPTION:
      asm_fprintf (f, "\t%@ ARM Exception Handler.\n");
      break;
    }

  if (IS_NAKED (func_type))
    asm_fprintf (f, "\t%@ Naked Function: prologue and epilogue provided by programmer.\n");

  if (IS_VOLATILE (func_type))
    asm_fprintf (f, "\t%@ Volatile: function does not return.\n");

  if (IS_NESTED (func_type))
    asm_fprintf (f, "\t%@ Nested: function declared inside another function.\n");
  if (IS_STACKALIGN (func_type))
    asm_fprintf (f, "\t%@ Stack Align: May be called with mis-aligned SP.\n");

  asm_fprintf (f, "\t%@ args = %d, pretend = %d, frame = %wd\n",
	       crtl->args.size,
	       crtl->args.pretend_args_size, frame_size);

  asm_fprintf (f, "\t%@ frame_needed = %d, uses_anonymous_args = %d\n",
	       frame_pointer_needed,
	       cfun->machine->uses_anonymous_args);

  if (cfun->machine->lr_save_eliminated)
    asm_fprintf (f, "\t%@ link register save eliminated.\n");

  if (crtl->calls_eh_return)
    asm_fprintf (f, "\t@ Calls __builtin_eh_return.\n");

}
#endif

static void
vc4_output_function_epilogue (FILE *stream,
			      HOST_WIDE_INT frame_size ATTRIBUTE_UNUSED)
{

  const char* fnname;
  fnname = XSTR (XEXP (DECL_RTL (current_function_decl), 0), 0);

  fprintf(stream, "\tb lr\n");
  //  fputs ("\t.end\t", stream);
  /* assemble_name (stream, fnname); */
  /* fputs ("\n", stream); */
}

struct gcc_target targetm = TARGET_INITIALIZER;

