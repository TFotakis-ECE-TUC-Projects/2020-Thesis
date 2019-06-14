/**
 * Contains definitions of terminal color characters
 * Use by printing the color character before the
 * string to be colored. Do not forget to reset the
 * buffer back to normal colors by using the KNRM
 * character.
 *
 * Example:
 * printf("%sThis is a red string! %sAnd this is a normal one.", KRED, KNRM)
 */


/** Normal */
#define KNRM  "\x1B[0m"


/** Red */
#define KRED  "\x1B[31m"


/** Green */
#define KGRN  "\x1B[32m"


/** Yellow */
#define KYEL  "\x1B[33m"


/** Blue */
#define KBLU  "\x1B[34m"


/** Magenta */
#define KMAG  "\x1B[35m"


/** Cyan */
#define KCYN  "\x1B[36m"


/** White */
#define KWHT  "\x1B[37m"