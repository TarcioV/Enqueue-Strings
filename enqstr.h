#ifndef _ENQSTR_H_
#define _ENQSTR_H_

/*
 * Enqueue Strings
 * Seg 30 Dez 2013 13:05:34
 *
 */

struct _EnqStr{
    char *str;
    struct _EnqStr *next;
    struct _EnqStr *prev;
};

typedef struct _EnqStr* ENQSTR;

extern  ENQSTR  *   esopen( void );
extern  void        eswrite( ENQSTR *enqstrings, const char *str );
extern  char    *   esread( ENQSTR * enqstrings );
extern  void        esrewind( ENQSTR *enqstrings );
extern  void        esclose( ENQSTR *enqstring );

#endif /* _ENQSTR_H_ */
