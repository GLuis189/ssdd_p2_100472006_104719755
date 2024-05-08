/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "imprimir.h"


void
imprimir_1(char *host)
{
	CLIENT *clnt;
	enum clnt_stat retval_1;
	int result_1;
	char *imprimir_nf_1_op;
	char *imprimir_nf_1_fecha;
	char *imprimir_nf_1_hora;
	char *imprimir_nf_1_user;
	enum clnt_stat retval_2;
	int result_2;
	char *imprimir_f_1_op;
	char *imprimir_f_1_fecha;
	char *imprimir_f_1_hora;
	char *imprimir_f_1_user;
	char *imprimir_f_1_file;

#ifndef	DEBUG
	clnt = clnt_create (host, IMPRIMIR, IMPRIMIR_V1, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	/* DEBUG */

	retval_1 = imprimir_nf_1(imprimir_nf_1_op, imprimir_nf_1_fecha, imprimir_nf_1_hora, imprimir_nf_1_user, &result_1, clnt);
	if (retval_1 != RPC_SUCCESS) {
		clnt_perror (clnt, "call failed");
	}
	retval_2 = imprimir_f_1(imprimir_f_1_op, imprimir_f_1_fecha, imprimir_f_1_hora, imprimir_f_1_user, imprimir_f_1_file, &result_2, clnt);
	if (retval_2 != RPC_SUCCESS) {
		clnt_perror (clnt, "call failed");
	}
#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */
}


int
main (int argc, char *argv[])
{
	char *host;

	if (argc < 2) {
		printf ("usage: %s server_host\n", argv[0]);
		exit (1);
	}
	host = argv[1];
	imprimir_1 (host);
exit (0);
}