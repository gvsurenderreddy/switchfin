/*
 * 	SQLite 3 - Realtime for asterisk as SQLite 
 * 	Author: <pawel@astfin.org>
 *
* Copyright (c) <2007>, <pawel@astfin.org> & Astfin
* All rights reserved.
* Copyright @ 2010 SwitchFin <dpn@switchfin.org>
*
* BSD License:
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the <organization> nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY <ASTFIN> ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL <ASTFIN> BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <asterisk.h>
#include <asterisk/channel.h>
#include <asterisk/logger.h>
#include <asterisk/config.h>
#include <asterisk/module.h>
#include <asterisk/lock.h>
#include <asterisk/options.h>
#include <asterisk/cli.h>
#include <asterisk/utils.h>
#include <asterisk/app.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <sqlite3.h>

#define AST_MODULE "res_sqlite3"

AST_MUTEX_DEFINE_STATIC(sqlite3_lock);
#define RES_SQLITE3_CONF "res_sqlite3.conf"
#define ARRAY_SIZE 256

static int    connected;
static time_t connect_time;

static int realtime_sqlite3_status(int fd, int argc, char **argv);


static char *moddesc = "SQLite3 RealTime Configuration Driver";

static char cli_realtime_sqlite3_status_usage[] =
        "Usage: realtime sqlite3 status\n"
        "       Shows connection information for the SQLite3 RealTime driver\n";

static struct ast_cli_entry cli_realtime_sqlite3_status = {
        { "realtime", "sqlite3", "status", NULL }, realtime_sqlite3_status,
        "Shows connection information for the SQLite3 RealTime driver", cli_realtime_sqlite3_status_usage, NULL };

sqlite3* conn = NULL;


static char *create_config_sql = 
"create table ast_config (\n"
"						 id integer primary key AUTOINCREMENT,\n"
"						 cat_metric int not null default 0,\n"
"						 var_metric int not null default 0,\n"
"						 commented int not null default 0,\n"
"						 filename varchar(128) not null,\n"
"						 category varchar(128) not null default 'default',\n"
"						 var_name varchar(128) not null,\n"
"						 var_val varchar(128) not null\n"
"						 );\n\n"
"CREATE INDEX ast_config_index_1 ON ast_config(filename);\n"
"CREATE INDEX ast_config_index_2 ON ast_config(filename,category);\n"
"CREATE INDEX ast_config_index_3 ON ast_config(filename,category,var_name);";



static char dbfile[ARRAY_SIZE];
static int parse_config(void);



int open_db(void) {
	char path[ARRAY_SIZE*2];
	//snprintf(path, sizeof(path), "%s/%s", ast_config_AST_LOG_DIR,dbfile);
	snprintf(path, sizeof(path), "%s", dbfile);

	ast_log(LOG_DEBUG,"SQLite3 Path:\n%s\n",path);
	if (conn) {
		sqlite3_close(conn);
		conn=NULL;
	}
	sqlite3_open(path,&conn);
	if (!conn) {
		ast_log(LOG_WARNING,"SQLite3 Error:\n %s\n",sqlite3_errmsg(conn));
		sqlite3_close(conn);
		conn=NULL;
		connected = 0;
		return 0;
	}
	connect_time = time(NULL); 
	connected = 1;
	return 1;
}
static struct ast_variable *realtime_sqlite3(const char *database, const char *table, va_list ap)
{
	int numFields, i, valsz;
	//char sql[512] = {NULL};
	char * sql = NULL;
	char buf[511]; /* Keep this size uneven as it is 2n+1. */
	char *stringp;
	char *chunk;
	char *op;
	const char *newparam, *newval;
	struct ast_variable *var=NULL, *prev=NULL;
	int res=0,running=0;
	sqlite3_stmt *stmt=NULL;

	ast_log(LOG_DEBUG, "SQLite3 RealTime: Loading...\n");
	
	if(!table) {
		ast_log(LOG_WARNING, "SQLite3 RealTime: No table specified.\n");
		return NULL;
	}

	/* Get the first parameter and first value in our list of passed paramater/value pairs */
	newparam = va_arg(ap, const char *);
	newval = va_arg(ap, const char *);
	if(!newparam || !newval)  {
		ast_log(LOG_WARNING, "SQLite3 RealTime: Realtime retrieval requires at least 1 parameter and 1 value to search on.\n");
		return NULL;
	}

	ast_mutex_lock(&sqlite3_lock);
	open_db();
	if (!conn) {
		ast_mutex_unlock(&sqlite3_lock);
		return NULL;
	}

	/* Create the first part of the query using the first parameter/value pairs we just extracted
	   If there is only 1 set, then we have our query. Otherwise, loop thru the list and concat */

	if(!strchr(newparam, ' ')) op = " ="; else op = "";	

	if ((valsz = strlen (newval)) * 2 + 1 > sizeof(buf))
		valsz = (sizeof(buf) - 1) / 2;

	sql = (char*)sqlite3_mprintf("SELECT * FROM %s WHERE %s%s '%q'", table, newparam,op, newval);

	while((newparam = va_arg(ap, const char *))) {
		newval = va_arg(ap, const char *);
		if(!strchr(newparam, ' ')) op = " ="; else op = "";
		if ((valsz = strlen (newval)) * 2 + 1 > sizeof(buf))
			valsz = (sizeof(buf) - 1) / 2;
	
	//	snprintf(sql + strlen(sql), sizeof(sql) - strlen(sql), (char*)sqlite3_mprintf(" AND %s%s '%q'", newparam, op, buf));
		sql = (char*)sqlite3_mprintf("%s AND %s%s '%q'", sql, newparam, op, newval);
	}
	va_end(ap);
	sql = (char *)sqlite3_mprintf("%s;",sql);
	ast_log(LOG_DEBUG, "SQLite3 RealTime [rt]: Retrieve SQL: %s\n", sql);
 	res = sqlite3_prepare(conn,sql,-1,&stmt,0);

        /* check for any errors */
        if (res) {
                ast_log(LOG_WARNING, "SQLite3 RealTime: Failed to query database. Check debug for more info.\n");
                ast_log(LOG_DEBUG, "SQLite3 RealTime: Query: %s\n", sql);
                ast_log(LOG_DEBUG, "SQLite3 RealTime: Query Failed because: %s\n", sqlite3_errmsg(conn));
                ast_mutex_unlock(&sqlite3_lock);
                return NULL;
        }	

        running = 1;
        while (running) {
                res = sqlite3_step(stmt);
		ast_log(LOG_DEBUG, "SQLite3 RealTime: Res: %d\n",res);
                running = 1;
                switch( res ){
                case SQLITE_DONE:       running = 0 ; break;
                case SQLITE_BUSY:       sleep(2); running = 2; break ;
                case SQLITE_ERROR:      running = 0; break;
                case SQLITE_MISUSE: running = 0; break;
                case SQLITE_ROW:
                default:
                        break;
                }
                if (!running)
                        break;

                if (running == 2)
                        continue;
		
		numFields = sqlite3_column_count(stmt);

		for(i = 0; i < numFields; i++) {
			stringp = (char*)sqlite3_column_text(stmt,i);
			ast_log(LOG_DEBUG, "SQLite3 Reatime Loop Field: %s\n", stringp);
			while(stringp) {
				chunk = strsep(&stringp, ";");
				if(chunk && !ast_strlen_zero(ast_strip(chunk))) {
					if(prev) {
						prev->next = ast_variable_new((char*)sqlite3_column_name(stmt,i), chunk);
						if (prev->next) {
							prev = prev->next;
						}
					} else {
						prev = var = ast_variable_new((char*)sqlite3_column_name(stmt,i), chunk);
					}
				}
			}
		}
	}
	
	
	ast_mutex_unlock(&sqlite3_lock);	
	sqlite3_free(sql);
	
	if ((sqlite3_finalize(stmt)))
                ast_log(LOG_ERROR,"SQLite3 [realtime_sqlite3] Error: %s\n",sqlite3_errmsg(conn));

	return var;

}
static int update_sqlite3(const char *database, const char *table, const char *keyfield, const char *lookup, va_list ap)
{
	char * sql=NULL;
	char buf[511] ; /* Keep this size uneven as it is 2n+1. */
	const char *newparam, *newval;
	int res;
	char * zErr=NULL;
	if(!table) {
		ast_log(LOG_WARNING, "SQLite3 RealTime: No table specified.\n");
               return -1;
	}

	/* Get the first parameter and first value in our list of passed paramater/value pairs */
	newparam = va_arg(ap, const char *);
	newval = va_arg(ap, const char *);
	if(!newparam || !newval)  {
		ast_log(LOG_WARNING, "SQLite3 RealTime: Realtime retrieval requires at least 1 parameter and 1 value to search on.\n");
		sqlite3_close(conn);
               return -1;
	}

	ast_mutex_lock(&sqlite3_lock);
	if (!open_db()) {
		ast_mutex_unlock(&sqlite3_lock);
		return -1;
	}

	 sql = (char*)sqlite3_mprintf("UPDATE %s SET %s = '%q'", table, newparam, newval);
                
        while((newparam = va_arg(ap, const char *))) {
                newval = va_arg(ap, const char *);
                                
                sql = (char*)sqlite3_mprintf("%s, %s = '%q'", sql, newparam, newval);
        }                                       
        va_end(ap);                                     
        sql = (char *)sqlite3_mprintf("%s WHERE %s = '%q';",sql,keyfield,newval);
        ast_log(LOG_DEBUG, "SQLite3 RealTime: Retrieve SQL: %s\n", sql);

	
	res = sqlite3_exec(conn, sql, NULL, NULL, &zErr);
        if (res) {
		ast_log(LOG_ERROR, "SQLite3 Error Update: %s\n %s\n", zErr,sql);
                sqlite3_free(zErr);
		return -1;
        }


	return 1;
}

static struct ast_config *config_sqlite3(const char *database, const char *table, const char *file, struct ast_config *cfg, int withcomments)
{

	unsigned long num_rows;
	struct ast_variable *new_v;
	struct ast_category *cur_cat;
	char sql[250] = "";
	char last[80] = "";
	int last_cat_metric = 0,res=0,i=0;
	sqlite3_stmt *stmt=NULL;
	int running=0;

	// reset 
	last[0] = '\0';

	if(!file || !strcmp(file, RES_SQLITE3_CONF)) {
		ast_log(LOG_WARNING, "SQLite3 RealTime: Cannot configure myself.\n");
		return NULL;
	}

	snprintf(sql, sizeof(sql), "SELECT category, var_name, var_val, cat_metric FROM %s WHERE filename='%s' and commented=0 ORDER BY filename, cat_metric desc, var_metric asc, category, var_name, var_val, id;", table, file);
	
	ast_log(LOG_DEBUG, "SQLite3 RealTime: Static SQL: %s\n", sql);


	/* Must connect to the server before anything else, as the escape function requires the sqlite3 handle. */
	ast_mutex_lock(&sqlite3_lock);
	
	//open_db();
	if (!open_db()) {
		ast_log(LOG_DEBUG, "SQLite3 RealTime: Static Could not open database\n");
		ast_mutex_unlock(&sqlite3_lock);
		return NULL;
	}
	res = sqlite3_prepare(conn,sql,-1,&stmt,0);

	/* check for any errors */
	if (res) { 
		ast_log(LOG_WARNING, "SQLite3 RealTime: Failed to query database. Check debug for more info.\n");
		ast_log(LOG_DEBUG, "SQLite3 RealTime: Query: %s\n", sql);
		ast_log(LOG_DEBUG, "SQLite3 RealTime: Query Failed because: %s\n", sqlite3_errmsg(conn));
		ast_mutex_unlock(&sqlite3_lock);
		return NULL;
	}
		
	ast_log(LOG_DEBUG, "SQLite3 RealTime: Res OK\n");	
	cur_cat = ast_config_get_current_category(cfg);	

	/**
	  * 0 - category
	  * 1 - var_name
	  * 2 - var_val
	  * 3 - cat_metric
	  */

	running = 1;
	while (running) {
		res = sqlite3_step(stmt);
		running = 1;
		ast_log(LOG_DEBUG, "SQLite3 RealTime: %d\n", res);
		switch( res ){
		case SQLITE_DONE:	running = 0 ; break;
		case SQLITE_BUSY:	sleep(2); running = 2; break ;
		case SQLITE_ERROR:	running = 0; break;
		case SQLITE_MISUSE: running = 0; break;
		case SQLITE_ROW:	
		default: 
			break;
		}	
		if (!running)
			break;
		
		if (running == 2)
			continue;



		if(option_verbose > 4)
			for(i=0;i<4;i++){
				ast_verbose(VERBOSE_PREFIX_3"SQLite3 Config: %d=%s\n",i,sqlite3_column_text(stmt,i));
			}

		
		if(!strcmp((char*)sqlite3_column_text(stmt,1), "#include")) {
			if (!ast_config_internal_load(
				(char*)sqlite3_column_text(stmt,2), cfg,0)
			   ) {
				ast_mutex_unlock(&sqlite3_lock);
				sqlite3_finalize(stmt);
				return NULL;
			}
			continue;
		}
		
		if (strcmp(last, (char*)sqlite3_column_text(stmt,0) ) || last_cat_metric != (sqlite3_column_int(stmt,3)) ) {	
			// Hello
			cur_cat = ast_category_new((char*)sqlite3_column_text(stmt,0));

			if (!cur_cat) {
				ast_log(LOG_WARNING, "Out of memory!\n");
				break;
			}	
			
			strcpy(last, (char*)sqlite3_column_text(stmt,0));
			last_cat_metric = (sqlite3_column_int(stmt,3));
			ast_category_append(cfg, cur_cat);
		}

		new_v = ast_variable_new((char*)sqlite3_column_text(stmt,1) , (char*)sqlite3_column_text(stmt,2) );
		ast_variable_append(cur_cat, new_v);


	}
	
	sqlite3_free(sql);
	if ((sqlite3_finalize(stmt))) 
		ast_log(LOG_ERROR,"SQLite3 [config_sqlite3] Error: %s\n",sqlite3_errmsg(conn));
	 
	ast_mutex_unlock(&sqlite3_lock);

	return cfg;			

}
static struct ast_config *realtime_multi_sqlite3(const char *database, const char *table, va_list ap)
{
	int numFields, i, valsz;
	char * sql= NULL;
	char buf[511]; /* Keep this size uneven as it is 2n+1. */
	const char *initfield = NULL;
	char *stringp;
	char *chunk;
	char *op;
	const char *newparam, *newval;
	struct ast_realloca ra;
	struct ast_variable *var=NULL;
	struct ast_config *cfg = NULL;
	struct ast_category *cat = NULL;
        int res=0,running=0;
        sqlite3_stmt *stmt=NULL;

	if(!table) {
		ast_log(LOG_WARNING, "SQLite3 RealTime: No table specified.\n");
		return NULL;
	}
	
	memset(&ra, 0, sizeof(ra));


	cfg = ast_config_new();
	if (!cfg) {
		/* If I can't alloc memory at this point, why bother doing anything else? */
		ast_log(LOG_WARNING, "Out of memory!\n");
		return NULL;
	}

	/* Get the first parameter and first value in our list of passed paramater/value pairs */
	newparam = va_arg(ap, const char *);
	newval = va_arg(ap, const char *);
	if(!newparam || !newval)  {
		ast_log(LOG_WARNING, "SQLite3 RealTime: Realtime retrieval requires at least 1 parameter and 1 value to search on.\n");
		sqlite3_close(conn);
		return NULL;
	}


	initfield = ast_strdupa(newparam);
	if(initfield && (op = strchr(initfield, ' '))) {
		*op = '\0';
	}

	ast_mutex_lock(&sqlite3_lock);
	if (!open_db()) {
		ast_mutex_unlock(&sqlite3_lock);
		return NULL;
	}

	/* Create the first part of the query using the first parameter/value pairs we just extracted
	   If there is only 1 set, then we have our query. Otherwise, loop thru the list and concat */

	if(!strchr(newparam, ' ')) op = " ="; else op = "";


	sql = (char*)sqlite3_mprintf("SELECT * FROM %s WHERE %s%s '%q'", table, newparam,op, newval);

        while((newparam = va_arg(ap, const char *))) {
                newval = va_arg(ap, const char *);
                if(!strchr(newparam, ' ')) op = " ="; else op = "";

                sql = (char*)sqlite3_mprintf("%s AND %s%s '%q'", sql, newparam, op, newval);
        }
        va_end(ap);
        sql = (char *)sqlite3_mprintf("%s;",sql);
        ast_log(LOG_DEBUG, "SQLite3 RealTime [multi]: Retrieve SQL: %s\n", sql);

	res = sqlite3_prepare(conn,sql,-1,&stmt,0);

        /* check for any errors */
        if (res) {
                ast_log(LOG_WARNING, "SQLite3 RealTime: Failed to query database. Check debug for more info.\n");
                ast_log(LOG_DEBUG, "SQLite3 RealTime: Query: %s\n", sql);
                ast_log(LOG_DEBUG, "SQLite3 RealTime: Query Failed because: %s\n", sqlite3_errmsg(conn));
                ast_mutex_unlock(&sqlite3_lock);
                return NULL;
        }
	
	running = 1;
        while (running) {
                res = sqlite3_step(stmt);
                running = 1;
                switch( res ){
                case SQLITE_DONE:       running = 0 ; break;
                case SQLITE_BUSY:       sleep(2); running = 2; break ;
                case SQLITE_ERROR:      running = 0; break;
                case SQLITE_MISUSE: running = 0; break;
                case SQLITE_ROW:
                default:
                        break;
                }
                if (!running)
                        break;
                
                if (running == 2)
                        continue;
                
                numFields = sqlite3_column_count(stmt);
                var = NULL;
		cat = ast_category_new("");
		if(!cat) {
			ast_log(LOG_WARNING, "Out of memory!\n");
			continue;
		}
 
                for(i = 0; i < numFields; i++) {
                        stringp = (char*)sqlite3_column_text(stmt,i);
                        while(stringp) {
                                chunk = strsep(&stringp, ";");
                                if(chunk && !ast_strlen_zero(ast_strip(chunk))) {
                                      
					if(initfield && !strcmp(initfield, (char*)sqlite3_column_name(stmt,i))) {
						ast_category_rename(cat, chunk);
                                        }
					var = ast_variable_new((char*)sqlite3_column_name(stmt,i), chunk);
					ast_variable_append(cat, var); 
                                }
                        }
                }
		ast_category_append(cfg, cat);

        }
	ast_mutex_unlock(&sqlite3_lock);
        sqlite3_free(sql);
        
        if ((sqlite3_finalize(stmt)))
                ast_log(LOG_ERROR,"SQLite3 [realtime_sqlite3] Error: %s\n",sqlite3_errmsg(conn));

	return cfg;
}

static struct ast_config_engine sqlite3_engine = {
        .name = "sqlite3",
        .load_func = config_sqlite3,
        .realtime_func = realtime_sqlite3,
        .realtime_multi_func = realtime_multi_sqlite3,
        .update_func = update_sqlite3 
}; 
static int parse_config (void)
{
	struct ast_config *config;
	const char *s;

	config = ast_config_load(RES_SQLITE3_CONF);

	if(config) {
		if(!(s=ast_variable_retrieve(config, "general", "dbfile"))) {
			ast_log(LOG_WARNING, "SQLite3 RealTime: No database file found, using 'asterisk' as default.\n");
			strncpy(dbfile, "asterisk", sizeof(dbfile) - 1);
		} else {
			strncpy(dbfile, s, sizeof(dbfile) - 1);
		}
	}

	ast_config_destroy(config);
	ast_log(LOG_DEBUG, "SQLite3 RealTime dbfile: %s\n", dbfile);



	return 1;
}

int load_module(void)
{
	int res=0;
	char *zErr;

	parse_config();
	ast_mutex_lock(&sqlite3_lock);
	if (!open_db()) {
		ast_log(LOG_WARNING, "SQLite3 RealTime: Couldn't establish connection. Check debug.\n");

	}

	/* Create the table that is needed */
	/* is the table there? */
        res = sqlite3_exec(conn, "SELECT COUNT(*) FROM ast_config;", NULL, NULL, NULL);
        if (res) {
                res = sqlite3_exec(conn, create_config_sql, NULL, NULL, &zErr);
                if (res) {
                        ast_log(LOG_ERROR, "SQLite3: Unable to create table 'ast_config': %s\n", zErr);
                        sqlite3_free(zErr);
                }
	}

	ast_config_engine_register(&sqlite3_engine);
	if(option_verbose) {
		ast_verbose("SQLite3 RealTime driver loaded.\n");
	}


	ast_cli_register(&cli_realtime_sqlite3_status);

	ast_mutex_unlock(&sqlite3_lock);
	return 0;
}

int unload_module(void)
{
	ast_mutex_lock(&sqlite3_lock);
	sqlite3_close(conn);
	
	ast_cli_unregister(&cli_realtime_sqlite3_status);
	ast_config_engine_deregister(&sqlite3_engine);
	if(option_verbose) {
		ast_verbose("SQLite3 RealTime unloaded.\n");
	}

	ast_module_user_hangup_all();

	/* Unlock so something else can destroy the lock. */
	ast_mutex_unlock(&sqlite3_lock);
	return 0;
}

int reload(void)
{
	/* Aquire control before doing anything to the module itself. */
	ast_mutex_lock(&sqlite3_lock);
	sqlite3_close(conn);

	connected = 0;
	parse_config();

	if (!open_db()) {
                ast_log(LOG_WARNING, "SQLite3 RealTime: Couldn't establish connection. Check debug.\n");

        }

	ast_verbose(VERBOSE_PREFIX_2 "SQLite3 RealTime reloaded.\n");

	/* Done reloading. Release lock so others can now use driver. */
	ast_mutex_unlock(&sqlite3_lock);
	return 0;
}

char *description(void)
{
        return moddesc;
}

int usecount(void)
{
	int res=0;
	return res;
}

char *key()
{
        return ASTERISK_GPL_KEY;
}

static int realtime_sqlite3_status(int fd, int argc, char **argv) {
	char status[256]="";
	int ctime = time(NULL) - connect_time;

	if (conn) {

		if (dbfile ) {
			snprintf(status, 255, "Connected to %s: ", dbfile );
		}
		
		if (ctime > 31536000) {
			ast_cli(fd, "%s for %d years, %d days, %d hours, %d minutes, %d seconds.\n", status,  ctime / 31536000, (ctime % 31536000) / 86400, (ctime % 86400) / 3600, (ctime % 3600) / 60, ctime % 60);
		} else if (ctime > 86400) {
			ast_cli(fd, "%s for %d days, %d hours, %d minutes, %d seconds.\n", status, ctime / 86400, (ctime % 86400) / 3600, (ctime % 3600) / 60, ctime % 60);
		} else if (ctime > 3600) {
			ast_cli(fd, "%s for %d hours, %d minutes, %d seconds.\n", status,  ctime / 3600, (ctime % 3600) / 60, ctime % 60);
		} else if (ctime > 60) {
			ast_cli(fd, "%s for %d minutes, %d seconds.\n", status,  ctime / 60, ctime % 60);
		} else {
			ast_cli(fd, "%s for %d seconds.\n", status, ctime);
		}

		return RESULT_SUCCESS;
	} else {
		return RESULT_FAILURE;
	}
}

AST_MODULE_INFO(ASTERISK_GPL_KEY, AST_MODFLAG_DEFAULT, "SQLite3 RealTime Configuration Driver",
       .load = load_module,
       .unload = unload_module,
       .reload = reload,
      );
  
