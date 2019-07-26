#include <my_global.h>
#include <mysql.h>

void finish_with_error(MYSQL *con)
{
	fprintf(stderr, "%s\n", mysql_error(con));
	mysql_close(con);
	exit(1);				
}

typedef struct pmysql
{
	MYSQL *con;
} pmysql;

typedef struct pmysql_res
{
	MYSQL_RES *res;
	MYSQL_ROW row;
	MYSQL_FIELD *field;
	int fields_len;
	char **fields;	
} pmysql_res;

void pmysql_init( pmysql *s ) {
	s->con = mysql_init(NULL);
}

void pmysql_connect( pmysql *s, char *host, char *user, char *pass, char *db ) {
	if (mysql_real_connect(s->con, host, user, pass, db, 0, NULL, 0) == NULL) 
	{
		finish_with_error(s->con);
	}	
}

void pmysql_query( pmysql *s, char *sql ) {
	if (mysql_query(s->con, sql)) 
	{
			finish_with_error(s->con);
	}
}

void pmysql_res_init( pmysql *s, pmysql_res *r ) {
	r->res = mysql_store_result(s->con);
	
	if (r->res == NULL) 
	{
			finish_with_error(s->con);
	}

	r->fields_len = mysql_num_fields(r->res);

	r->fields = malloc( sizeof(void *) * r->fields_len );
	int i = 0;
	while(r->field = mysql_fetch_field(r->res)) 
	{
		r->fields[i++] = r->field->name;
	} 
}

MYSQL_ROW pmysql_res_next( pmysql_res *r ) {
	r->row = mysql_fetch_row(r->res);
	return r->row;
}

void pmysql_res_free( pmysql_res *r) {
	mysql_free_result(r->res);
	free(r->field);
	free(r->fields);
}

void pmysql_free( pmysql *s ) {
	mysql_close(s->con);
}


void *pmysql_res_val( pmysql_res *r, char *fname) {
	for( int i = 0; i < r->fields_len; i++ ) {
		if( strcmp( r->fields[i], fname ) == 0 ) {
			return r->row[i];
		} 
	}
}


int main(int argc, char **argv)
{			
	pmysql con;
	pmysql_init(&con);
	pmysql_connect(&con, "localhost", "peji", "12c456789", "test");

	pmysql_query(&con, "SELECT * FROM test");

	pmysql_res res;
	pmysql_res_init( &con, &res );

	while( pmysql_res_next(&res) ) {
		printf("%s ", pmysql_res_val(&res, "name") );
		printf("%s ", pmysql_res_val(&res, "id")  ); 
	}

	pmysql_res_free( &res );
	pmysql_free(&con);

}