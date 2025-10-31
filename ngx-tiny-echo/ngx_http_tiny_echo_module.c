#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

static char *ngx_http_tiny_echo(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static ngx_int_t ngx_http_tiny_echo_handler(ngx_http_request_t *r);

static ngx_command_t ngx_http_tiny_echo_cmds[] = {
    { ngx_string("tiny_echo"),
      NGX_HTTP_LOC_CONF|NGX_CONF_NOARGS,
      ngx_http_tiny_echo, 0, 0, NULL },
    ngx_null_command
};

static ngx_http_module_t ngx_http_tiny_echo_ctx = {
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
};

ngx_module_t ngx_http_tiny_echo_module = {
    NGX_MODULE_V1,
    &ngx_http_tiny_echo_ctx,
    ngx_http_tiny_echo_cmds,
    NGX_HTTP_MODULE,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NGX_MODULE_V1_PADDING
};

static char *ngx_http_tiny_echo(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_core_loc_conf_t *cl =
        ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    cl->handler = ngx_http_tiny_echo_handler;
    return NGX_CONF_OK;
}

static ngx_int_t ngx_http_tiny_echo_handler(ngx_http_request_t *r)
{
    if (!(r->method & (NGX_HTTP_GET|NGX_HTTP_HEAD))) return NGX_HTTP_NOT_ALLOWED;

    size_t n = r->method_name.len + 1 + r->uri.len + 6 + r->connection->addr_text.len + 2;
    u_char *b = ngx_pnalloc(r->pool, n);
    if (b == NULL) return NGX_HTTP_INTERNAL_SERVER_ERROR;

    ngx_snprintf(b, n, "%V %V from %V%N",
                 &r->method_name, &r->uri, &r->connection->addr_text);

    size_t len = ngx_strlen(b);

    r->headers_out.status = NGX_HTTP_OK;
    r->headers_out.content_length_n = len;
    ngx_str_set(&r->headers_out.content_type, "text/plain");

    ngx_table_elt_t *h = ngx_list_push(&r->headers_out.headers);
    if (h == NULL) return NGX_HTTP_INTERNAL_SERVER_ERROR;
    h->hash = 1;
    ngx_str_set(&h->key, "X-Tiny-Echo");
    ngx_str_set(&h->value, "tiny");

    ngx_buf_t *buf = ngx_create_temp_buf(r->pool, len);
    if (buf == NULL) return NGX_HTTP_INTERNAL_SERVER_ERROR;
    ngx_memcpy(buf->pos, b, len);
    buf->last = buf->pos + len;
    buf->memory = 1;
    buf->last_buf = 1;

    ngx_chain_t out = { buf, NULL };

    ngx_int_t rc = ngx_http_send_header(r);
    if (rc == NGX_ERROR || rc > NGX_OK || r->header_only) return rc;

    return ngx_http_output_filter(r, &out);
}
