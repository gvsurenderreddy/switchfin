mkdir -p /etc/asterisk/httpd
lighttpd -f /etc/lighttpd.conf -D >/dev/null 2>/dev/null &
