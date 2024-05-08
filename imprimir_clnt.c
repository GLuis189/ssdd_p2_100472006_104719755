/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include <memory.h> /* for memset */
#include "imprimir.h"

/* Default timeout can be changed using clnt_control() */
static struct timeval TIMEOUT = { 25, 0 };

enum clnt_stat 
imprimir_nf_1(char *op, char *fecha, char *hora, char *user, int *clnt_res,  CLIENT *clnt)
{
	imprimir_nf_1_argument arg;
	arg.op = op;
	arg.fecha = fecha;
	arg.hora = hora;
	arg.user = user;
	return (clnt_call (clnt, IMPRIMIR_NF, (xdrproc_t) xdr_imprimir_nf_1_argument, (caddr_t) &arg,
		(xdrproc_t) xdr_int, (caddr_t) clnt_res,
		TIMEOUT));
}

enum clnt_stat 
imprimir_f_1(char *op, char *fecha, char *hora, char *user, char *file, int *clnt_res,  CLIENT *clnt)
{
	imprimir_f_1_argument arg;
	arg.op = op;
	arg.fecha = fecha;
	arg.hora = hora;
	arg.user = user;
	arg.file = file;
	return (clnt_call (clnt, IMPRIMIR_F, (xdrproc_t) xdr_imprimir_f_1_argument, (caddr_t) &arg,
		(xdrproc_t) xdr_int, (caddr_t) clnt_res,
		TIMEOUT));
}
