## Name

**host-local** - forward and reverse DNS queries that use the POSIX builtin name resolver

## Synopsis

`host-local [-46tu] host-or-address [service]`

## Description

**host-local** uses the POSIX bultin name resolver to make forward ((getaddrinfo)[https://linux.die.net/man/3/getaddrinfo]) and reverse ((getnameinfo)[https://linux.die.net/man/3/getnameinfo]) queries.

## Options

<dl>
<dt>-h</dt>
<dd>Display the help message and exit.</dd>
<dt>-4</dt>
<dd>Query only IPv4 addresses.</dd>
<dt>-6</dt>
<dd>Query only IPv6 addresses.</dd>
<dt>-t</dt>
<dd>Query only TCP serivces.</dd>
<dt>-u</dt>
<dd>Query only UDP serivces.</dd>
</dl>
