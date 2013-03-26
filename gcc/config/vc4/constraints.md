
(define_register_constraint "l" "LOW_REGS"
  "Registers from r0 to r15.")

(define_constraint "J"
  "Integer zero."
  (and (match_code "const_int")
       (match_test "ival == 0")))

(define_constraint "U05"
   "unsigned 5 bit value (0..31)"
   (and (match_code "const_int")
	(match_test "0 <= ival && ival < 32")))

(define_constraint "U07"
   "Unsigned 7 bit value"
   (and (match_code "const_int")
	(match_test "0 <= ival && ival < 128")))

(define_constraint "U08"
   "Unsigned 8 bit value"
   (and (match_code "const_int")
	(match_test "0 <= ival && ival < 256")))

(define_constraint "U16"
   "Unsigned 16 bit value"
   (and (match_code "const_int")
	(match_test "0 <= ival && ival < 65536")))

(define_constraint "S05"
   "signed 5 bit value"
   (and (match_code "const_int")
	(match_test "-16 <= ival && ival < 16")))

(define_constraint "S06"
   "signed 6 bit value"
   (and (match_code "const_int")
	(match_test "-32 <= ival && ival < 32")))

(define_constraint "S07"
   "signed 7 bit value"
   (and (match_code "const_int")
	(match_test "-64 <= ival && ival < 64")))

(define_constraint "S08"
   "signed 8 bit value"
   (and (match_code "const_int")
	(match_test "-128 <= ival && ival < 128")))

(define_constraint "S10"
   "signed 10 bit value"
   (and (match_code "const_int")
	(match_test "-512 <= ival && ival < 512")))

(define_constraint "S11"
   "signed 11 bit value"
   (and (match_code "const_int")
	(match_test "-1024 <= ival && ival < 1024")))

(define_constraint "S16"
   "signed 16 bit value"
   (and (match_code "const_int")
	(match_test "-32768 <= ival && ival < 32768")))
