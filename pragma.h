#pragma warning(disable:4013) //'function' undefined; assuming extern returning int
#pragma warning(disable:4024) //'function' : different types for formal and actual parameter 'number'
#pragma warning(disable:4047) //'operator' : 'identifier1' differs in levels of indirection from 'identifier2'
#pragma warning(disable:4133) //incompatible types - from x to y
#pragma warning(disable:4244) //conversion from 'x ' to 'y ', possible loss of data

#pragma warning(error:4189) //local variable initialized but not referenced
#pragma warning(error:4700) //local variable 'x' used without having been initialized
#pragma warning(error:4020) //too many actual parameters
#pragma warning(error:4101) //unreferenced local variable

#ifdef _DEBUG
#define free  my_free
void my_free(void *);

#define malloc  my_alloc
void * my_alloc(unsigned int);

#define realloc  my_realloc
void * my_realloc(void *,unsigned int);

#endif //_DEBUG