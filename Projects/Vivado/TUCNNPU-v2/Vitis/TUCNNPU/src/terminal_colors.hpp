/**
 * Contains definitions of terminal color characters.
 *
 * Use by printing the color character before the string to be colored.
 * Do not forget to reset the buffer back to normal colors by using the KNRM
 * character.
 *
 * Example:
 * printf("%sThis is a red string! %sAnd this is a normal one.", KRED, KNRM)
 */

#ifndef SRC_TERMINAL_COLORS_H_
#define SRC_TERMINAL_COLORS_H_

#define KNRM "\x1B[0m"  /** Normal */
#define KRED "\x1B[31m" /** Red */
#define KGRN "\x1B[32m" /** Green */
#define KYEL "\x1B[33m" /** Yellow */
#define KBLU "\x1B[34m" /** Blue */
#define KMAG "\x1B[35m" /** Magenta */
#define KCYN "\x1B[36m" /** Cyan */
#define KWHT "\x1B[37m" /** White */

#endif
