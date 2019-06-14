// Contains definitions of terminal color characters
// Use by printing the color character before the
// string to be colored. Do not forget to reset the
// buffer back to normal colors by using the KNRM
// character.
//
// Example:
// printf("%sThis is a red string! %sAnd this is a normal one.", KRED, KNRM)


#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"