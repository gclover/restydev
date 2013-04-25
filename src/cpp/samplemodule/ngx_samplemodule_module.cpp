
// stub module to test header files' C++ compatibilty

extern "C" {
  #include <ngx_config.h>
  #include <ngx_core.h>
  #include <ngx_event.h>
  #include <ngx_event_connect.h>
  #include <ngx_event_pipe.h>

  #include <ngx_http.h>

  #include <ngx_mail.h>
  #include <ngx_mail_pop3_module.h>
  #include <ngx_mail_imap_module.h>
  #include <ngx_mail_smtp_module.h>
}

// nginx header files should go before other, because they define 64-bit off_t
// #include <string>


void ngx_samplemodule_handler(void *data);

void
ngx_samplemodule_handler(void *data)
{
    return;
}

//------------------------------------------------------------------------------------------

#define OUT_BUFSIZE 256

static char *ngx_http_hello_set(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static char *ngx_http_foo_set(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

static ngx_int_t ngx_http_hello_process_init(ngx_cycle_t *cycle);
static void ngx_http_hello_process_exit(ngx_cycle_t *cycle);

static ngx_int_t make_http_header(ngx_http_request_t *r);
static ngx_int_t make_http_get_body(ngx_http_request_t *r, char *out_buf);

static char g_foo_settings[64] = {0};

/* Commands */
static ngx_command_t  ngx_http_hello_commands[] = {
    { ngx_string("ngx_hello_module"),
      NGX_HTTP_LOC_CONF|NGX_CONF_NOARGS,
      ngx_http_hello_set,
      NGX_HTTP_LOC_CONF_OFFSET,
      0,
      NULL },

    { ngx_string("hello"),
      NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_http_foo_set,
      NGX_HTTP_LOC_CONF_OFFSET,
      0,
      NULL },  

      ngx_null_command
};

static ngx_http_module_t  ngx_http_hello_module_ctx = {
    NULL,                                  /* preconfiguration */
    NULL,                                     /* postconfiguration */

    NULL,                                  /* create main configuration */
    NULL,                                  /* init main configuration */

    NULL,                                  /* create server configuration */
    NULL,                                  /* merge server configuration */

    NULL,                                  /* create location configuration */
    NULL                                   /* merge location configuration */
};

/* hook */
ngx_module_t  ngx_http_hello_module = {
    NGX_MODULE_V1,
    &ngx_http_hello_module_ctx,              /* module context */
    ngx_http_hello_commands,                 /* module directives */
    NGX_HTTP_MODULE,                       /* module type */
    NULL,                                  /* init master */
    NULL,                                  /* init module */
    ngx_http_hello_process_init,             /* init process */
    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    ngx_http_hello_process_exit,             /* exit process */
    NULL,                                  /* exit master */
    NGX_MODULE_V1_PADDING
};

/* setting header for no-cache */
static ngx_int_t make_http_header(ngx_http_request_t *r){
    ngx_uint_t        i;
    ngx_table_elt_t  *cc, **ccp;

    r->headers_out.content_type.len = sizeof("text/html") - 1;
    r->headers_out.content_type.data = (u_char *) "text/html";
    ccp = r->headers_out.cache_control.elts;
    if (ccp == NULL) {

        if (ngx_array_init(&r->headers_out.cache_control, r->pool,
                           1, sizeof(ngx_table_elt_t *))
            != NGX_OK)
        {
            return NGX_ERROR;
        }

        ccp = ngx_array_push(&r->headers_out.cache_control);
        if (ccp == NULL) {
            return NGX_ERROR;
        }

        cc = ngx_list_push(&r->headers_out.headers);
        if (cc == NULL) {
            return NGX_ERROR;
        }

        cc->hash = 1;
        cc->key.len = sizeof("Cache-Control") - 1;
        cc->key.data = (u_char *) "Cache-Control";

        *ccp = cc;

    } else {
        for (i = 1; i < r->headers_out.cache_control.nelts; i++) {
            ccp[i]->hash = 0;
        }

        cc = ccp[0];
    }

    cc->value.len = sizeof("no-cache") - 1;
    cc->value.data = (u_char *) "no-cache";

    return NGX_OK;
}

static ngx_int_t make_http_get_body(ngx_http_request_t *r, char *out_buf){
    char *qs_start = (char *)r->args_start;
    char *qs_end = (char *)r->uri_end;
    char uri[128] = {0};
    char *id;

    if (qs_start == NULL || qs_end == NULL){
        return NGX_HTTP_BAD_REQUEST;
    }
    if ((memcmp(qs_start, "id=", 3) == 0)){
        id = qs_start + 3;
        *qs_end = '\0';
    }else{
        return NGX_HTTP_BAD_REQUEST;
    }
    snprintf(uri, r->uri.len + 1, "%s", r->uri.data);
    sprintf(out_buf, "Author: http://timyang.net/ nconfig=%snid=%snuri=%snret=%lxn", g_foo_settings, id, uri, ngx_random());
    return NGX_OK;
}

static ngx_int_t
ngx_http_hello_handler(ngx_http_request_t *r)
{
    ngx_int_t     rc;
    ngx_buf_t    *b;
    ngx_chain_t   out;

    /* Http Output Buffer */
    char out_buf[OUT_BUFSIZE] = {0};

    if (!(r->method & (NGX_HTTP_GET|NGX_HTTP_HEAD))) {
        return NGX_HTTP_NOT_ALLOWED;
    }

    rc = ngx_http_discard_request_body(r);

    if (rc != NGX_OK && rc != NGX_AGAIN) {
        return rc;
    }

    /* make http header */
    rc = make_http_header(r);
    if (rc != NGX_OK) {
        return rc;
    }

    if (r->method == NGX_HTTP_HEAD) {
        r->headers_out.status = NGX_HTTP_OK;
        return ngx_http_send_header(r);
    } else if (r->method == NGX_HTTP_GET) {
        /* make http get body buffer */
        rc = make_http_get_body(r, out_buf);
        if (rc != NGX_OK) {
            return rc;
        }
    } else {
        return NGX_HTTP_NOT_ALLOWED;
    }

    b = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));
    if (b == NULL) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    out.buf = b;
    out.next = NULL;

    b->pos = (u_char *)out_buf;
    b->last = (u_char *)out_buf + strlen(out_buf);
    b->memory = 1;
    b->last_buf = 1;
    r->headers_out.status = NGX_HTTP_OK;
    r->headers_out.content_length_n = strlen(out_buf);

    rc = ngx_http_send_header(r);

    if (rc == NGX_ERROR || rc > NGX_OK || r->header_only) {
        return rc;
    }

    return ngx_http_output_filter(r, &out);
}

static char *
ngx_http_hello_set(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_core_loc_conf_t *clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);

    /* register hanlder */
    clcf->handler = ngx_http_hello_handler;

    return NGX_CONF_OK;
}

static char *
ngx_http_foo_set(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_str_t *value = cf->args->elts;
    memcpy(g_foo_settings, value[1].data, value[1].len);
    g_foo_settings[value[1].len] = '�';

    return NGX_CONF_OK;
}

static ngx_int_t
ngx_http_hello_process_init(ngx_cycle_t *cycle)
{
    // do some init here
    return NGX_OK;
}

static void
ngx_http_hello_process_exit(ngx_cycle_t *cycle)
{
    return;
}
