
#include<sys/types.h>
#include<wchar.h>

typedef wchar_t (*t_wslen)(wchar_t *);
typedef wchar_t (*t_wscpy)(wchar_t *,wchar_t *);
typedef wchar_t (*t_wsncpy)(wchar_t *,wchar_t *,size_t);

t_wslen  wslen  = (t_wslen )wcslen;
t_wscpy  wscpy  = (t_wscpy )wcscpy;
t_wsncpy wsncpy = (t_wsncpy)wcsncpy;
