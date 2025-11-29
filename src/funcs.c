#ifndef FUNCS_C
#define FUNCS_C

#ifndef FUNCS
#define FUNCS
#endif

// clang-format off
#define ARG_COUNT(...) ARG_COUNT_IMPL(__VA_ARGS__, 20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0)
#define ARG_COUNT_IMPL(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,N,...) N

#define FIELDS_0()
#define FIELDS_1(f1) f1;
#define FIELDS_2(f1, f2) f1; f2;
#define FIELDS_3(f1, f2, f3) f1; f2; f3;
#define FIELDS_4(f1, f2, f3, f4) f1; f2; f3; f4;
#define FIELDS_5(f1, f2, f3, f4, f5) f1; f2; f3; f4; f5;
#define FIELDS_6(f1, f2, f3, f4, f5, f6) f1; f2; f3; f4; f5; f6;
#define FIELDS_7(f1, f2, f3, f4, f5, f6, f7) f1; f2; f3; f4; f5; f6; f7;
#define FIELDS_8(f1, f2, f3, f4, f5, f6, f7, f8) f1; f2; f3; f4; f5; f6; f7; f8;
#define FIELDS_9(f1, f2, f3, f4, f5, f6, f7, f8, f9) f1; f2; f3; f4; f5; f6; f7; f8; f9;
#define FIELDS_10(f1, f2, f3, f4, f5, f6, f7, f8, f9, f10) f1; f2; f3; f4; f5; f6; f7; f8; f9; f10;
// clang-format on

#define FIELDS_DISPATCH(N) FIELDS_##N
#define FIELDS_IMPL(N, ...) FIELDS_DISPATCH(N)(__VA_ARGS__)
#define FIELDS(...) FIELDS_IMPL(ARG_COUNT(__VA_ARGS__), __VA_ARGS__)

#define FUNC(Name, ...) \
  __VA_OPT__(typedef struct {FIELDS(__VA_ARGS__)} Name##Args;)
FUNCS
#undef FUNC

#define FUNC(Name, ...)                                \
  static void Name##Func(__VA_OPT__(Name##Args args)); \
  static void Name##Wrapper(__VA_OPT__(void* args))    \
  {                                                    \
    Name##Func(__VA_OPT__(*(Name##Args*)args));        \
  }
FUNCS
#undef FUNC

#endif  // FUNCS_C
